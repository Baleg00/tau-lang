/**
 * \file
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "compiler/compiler.h"

#include "llvm.h"
#include "ast/ast.h"
#include "ast/registry.h"
#include "compiler/options.h"
#include "stages/analysis/ctrlflow.h"
#include "stages/analysis/symtable.h"
#include "stages/lexer/lexer.h"
#include "stages/lexer/token/registry.h"
#include "stages/lexer/token/token.h"
#include "stages/parser/parser.h"
#include "utils/common.h"
#include "utils/crumb.h"
#include "utils/timer.h"
#include "utils/io/file.h"

struct compiler_t
{
  options_ctx_t* options;
};

static void compiler_dump_tokens(path_t* path, vector_t* tokens)
{
  path_t* tokens_path = path_replace_extension(path, "tokens.json");

  string_t* tokens_path_str = path_to_string(tokens_path);

  FILE* tokens_file = fopen(string_begin(tokens_path_str), "w");
  ASSERT(tokens_file != NULL);

  string_free(tokens_path_str);
  path_free(tokens_path);

  token_json_dump_vector(tokens_file, tokens);

  fclose(tokens_file);
}

static void compiler_dump_ast(path_t* path, ast_node_t* root)
{
  path_t* ast_path = path_replace_extension(path, "ast.json");

  string_t* ast_path_str = path_to_string(ast_path);

  FILE* ast_file = fopen(string_begin(ast_path_str), "w");
  ASSERT(ast_file != NULL);

  string_free(ast_path_str);
  path_free(ast_path);

  ast_node_dump_json(ast_file, root);

  fclose(ast_file);
}

static void compiler_emit_ll(path_t* path, LLVMModuleRef llvm_module)
{
  path_t* ll_path = path_replace_extension(path, "ll");

  string_t* ll_path_str = path_to_string(ll_path);

  char* error_str = NULL;

  if (LLVMPrintModuleToFile(llvm_module, string_begin(ll_path_str), &error_str))
  {
    log_error("LLVM", "Failed to emit LL file (%s): %s", string_begin(ll_path_str), error_str);
    LLVMDisposeMessage(error_str);
  }

  string_free(ll_path_str);
  path_free(ll_path);
}

static void compiler_emit_bc(path_t* path, LLVMModuleRef llvm_module)
{
  path_t* bc_path = path_replace_extension(path, "bc");

  string_t* bc_path_str = path_to_string(bc_path);

  if (LLVMWriteBitcodeToFile(llvm_module, string_begin(bc_path_str)))
    log_error("LLVM", "Failed to emit bitcode file (%s).", string_begin(bc_path_str));

  string_free(bc_path_str);
  path_free(bc_path);
}

static void compiler_emit_obj(path_t* path, LLVMModuleRef llvm_module)
{
  path_t* obj_path = path_replace_extension(path, "obj");

  string_t* obj_path_str = path_to_string(obj_path);

  char* error_str = NULL;

  if (LLVMTargetMachineEmitToFile(llvm_get_machine(), llvm_module, string_begin(obj_path_str), LLVMObjectFile, &error_str))
  {
    log_error("LLVM", "Failed to emit object file (%s): %s", string_begin(obj_path_str), error_str);
    LLVMDisposeMessage(error_str);
  }

  string_free(obj_path_str);
  path_free(obj_path);
}

static void compiler_emit_asm(path_t* path, LLVMModuleRef llvm_module)
{
  path_t* asm_path = path_replace_extension(path, "asm");

  string_t* asm_path_str = path_to_string(asm_path);

  char* error_str = NULL;

  if (LLVMTargetMachineEmitToFile(llvm_get_machine(), llvm_module, string_begin(asm_path_str), LLVMAssemblyFile, &error_str))
  {
    log_error("LLVM", "Failed to emit assembly file (%s): %s", string_begin(asm_path_str), error_str);
    LLVMDisposeMessage(error_str);
  }

  string_free(asm_path_str);
  path_free(asm_path);
}

environment_t* compiler_process_file(compiler_t* compiler, path_t* path)
{
  if (!file_exists(path))
  {
    string_t* path_str = path_to_string(path);

    log_error("main", "File does not exist: %s", string_begin(path_str));

    string_free(path_str);

    return NULL;
  }

  environment_t* env = environment_init(
    symtable_init(NULL),
    typebuilder_init(llvm_get_context(), llvm_get_data()),
    typetable_init(),
    llvm_get_context(),
    llvm_get_data(),
    LLVMModuleCreateWithNameInContext("module", llvm_get_context()),
    LLVMCreateBuilderInContext(llvm_get_context())
  );

  size_t path_len = path_to_cstr(path, NULL, 0);
  char* path_cstr = (char*)malloc(sizeof(char) * (path_len + 1));
  path_to_cstr(path, path_cstr, path_len + 1);

  vector_push(env->paths, path_cstr);

  size_t src_len = file_read(path, NULL, 0);
  char* src_cstr = (char*)malloc((src_len + 1) * sizeof(char));
  file_read(path, src_cstr, src_len + 1);

  vector_push(env->sources, src_cstr);

  {
    error_bag_t* errors = error_bag_init(10);

    lexer_t* lexer = lexer_init();

    time_it("lexer", lexer_lex(lexer, path_cstr, src_cstr, env->tokens, errors));

    lexer_free(lexer);

    if (!error_bag_empty(errors))
    {
      while (!error_bag_empty(errors))
      {
        error_t error;

        if (error_bag_get(errors, &error))
          error_print(error);
      }

      exit(EXIT_FAILURE);
    }

    error_bag_free(errors);
  }

  if (options_get_dump_tokens(compiler->options))
    compiler_dump_tokens(path, env->tokens);

  ast_node_t* root_node = NULL;

  {
    error_bag_t* errors = error_bag_init(10);

    parser_t* parser = parser_init();

    time_it("parser", root_node = parser_parse(parser, env->tokens, errors));

    parser_free(parser);

    if (!error_bag_empty(errors))
    {
      while (!error_bag_empty(errors))
      {
        error_t error;

        if (error_bag_get(errors, &error))
          error_print(error);
      }

      exit(EXIT_FAILURE);
    }

    error_bag_free(errors);
  }

  if (options_get_dump_ast(compiler->options))
    compiler_dump_ast(path, root_node);

  {
    nameres_ctx_t* nameres_ctx = nameres_ctx_init(env->symtable);

    time_it("analysis:nameres", ast_node_nameres(nameres_ctx, root_node));

    nameres_ctx_free(nameres_ctx);
  }

  {
    typecheck_ctx_t* typecheck_ctx = typecheck_ctx_init(env->typebuilder, env->typetable);

    time_it("analysis:typecheck", ast_node_typecheck(typecheck_ctx, root_node));

    typecheck_ctx_free(typecheck_ctx);
  }

  {
    ctrlflow_ctx_t* ctrlflow_ctx = ctrlflow_ctx_init();

    time_it("analysis:ctrlflow", ast_node_ctrlflow(ctrlflow_ctx, root_node));

    ctrlflow_ctx_free(ctrlflow_ctx);
  }

  {
    codegen_ctx_t* codegen_ctx = codegen_ctx_init(
      env->typebuilder,
      env->typetable,
      env->llvm_context,
      env->llvm_layout,
      env->llvm_module,
      env->llvm_builder
    );

    time_it("codegen", ast_node_codegen(codegen_ctx, root_node));

    codegen_ctx_free(codegen_ctx);
  }

  if (options_get_dump_ll(compiler->options))
    compiler_emit_ll(path, env->llvm_module);

  LLVMVerifyModule(env->llvm_module, LLVMAbortProcessAction, NULL);

  LLVMPassBuilderOptionsRef llvm_pass_builder_options = LLVMCreatePassBuilderOptions();
  LLVMPassBuilderOptionsSetVerifyEach(llvm_pass_builder_options, true);

#ifdef TAU_DEBUG
  LLVMPassBuilderOptionsSetDebugLogging(llvm_pass_builder_options, true);
#endif

  // LLVMRunPasses(llvm_module, "default<O3>", llvm_get_machine(), llvm_pass_builder_options);

  LLVMDisposePassBuilderOptions(llvm_pass_builder_options);

  if (options_get_dump_bc(compiler->options))
    compiler_emit_bc(path, env->llvm_module);

  if (options_get_dump_asm(compiler->options))
    compiler_emit_asm(path, env->llvm_module);

  compiler_emit_obj(path, env->llvm_module);

  return env;
}

compiler_t* compiler_init(void)
{
  compiler_t* compiler = (compiler_t*)malloc(sizeof(compiler_t));

  log_set_stream(stdout);
  crumb_set_stream(stdout);

  compiler->options = options_ctx_init();

  return compiler;
}

void compiler_free(compiler_t* compiler)
{
  if (!options_get_should_exit(compiler->options))
  {
    ast_registry_free();
    token_registry_free();
    llvm_free();
  }

  options_ctx_free(compiler->options);

  free(compiler);
}

int compiler_main(compiler_t* compiler, int argc, const char* argv[])
{
  options_parse(compiler->options, argc, argv);

  if (options_get_should_exit(compiler->options))
    return EXIT_SUCCESS;

  log_set_verbose(options_get_is_verbose(compiler->options));
  log_set_level(options_get_log_level(compiler->options));

  time_it("LLVM:init", llvm_init());

  if (vector_empty(options_get_input_files(compiler->options)))
  {
    log_fatal("main", "No input files provided! ");
    return EXIT_FAILURE;
  }

  vector_t* input_files = options_get_input_files(compiler->options);

  VECTOR_FOR_LOOP(i, input_files)
  {
    const char* path_cstr = (const char*)vector_get(input_files, i);
    path_t* path = path_init_with_cstr(path_cstr);

    environment_t* env = compiler_process_file(compiler, path);

    VECTOR_FOR_LOOP(i, env->paths)
    {
      free(vector_get(env->paths, i));
    }

    VECTOR_FOR_LOOP(i, env->sources)
    {
      free(vector_get(env->sources, i));
    }

    LLVMDisposeBuilder(env->llvm_builder);
    LLVMDisposeModule(env->llvm_module);

    typetable_free(env->typetable);
    typebuilder_free(env->typebuilder);
    symtable_free(env->symtable);

    environment_free(env);

    path_free(path);
  }

  return EXIT_SUCCESS;
}
