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

struct tau_compiler_t
{
  tau_options_ctx_t* options;
};

static void tau_compiler_dump_tokens(tau_path_t* path, tau_vector_t* tokens)
{
  tau_path_t* tokens_path = tau_path_replace_extension(path, "tokens.json");

  tau_string_t* tokens_path_str = tau_path_to_string(tokens_path);

  FILE* tokens_file = fopen(tau_string_begin(tokens_path_str), "w");
  TAU_ASSERT(tokens_file != NULL);

  tau_string_free(tokens_path_str);
  tau_path_free(tokens_path);

  tau_token_json_dump_vector(tokens_file, tokens);

  fclose(tokens_file);
}

static void tau_compiler_dump_ast(tau_path_t* path, tau_ast_node_t* root)
{
  tau_path_t* tau_ast_path = tau_path_replace_extension(path, "ast.json");

  tau_string_t* tau_ast_path_str = tau_path_to_string(tau_ast_path);

  FILE* tau_ast_file = fopen(tau_string_begin(tau_ast_path_str), "w");
  TAU_ASSERT(tau_ast_file != NULL);

  tau_string_free(tau_ast_path_str);
  tau_path_free(tau_ast_path);

  tau_ast_node_dump_json(tau_ast_file, root);

  fclose(tau_ast_file);
}

static void tau_compiler_emit_ll(tau_path_t* path, LLVMModuleRef llvm_module)
{
  tau_path_t* ll_path = tau_path_replace_extension(path, "ll");

  tau_string_t* ll_path_str = tau_path_to_string(ll_path);

  char* tau_error_str = NULL;

  if (LLVMPrintModuleToFile(llvm_module, tau_string_begin(ll_path_str), &tau_error_str))
  {
    tau_log_error("LLVM", "Failed to emit LL file (%s): %s", tau_string_begin(ll_path_str), tau_error_str);
    LLVMDisposeMessage(tau_error_str);
  }

  tau_string_free(ll_path_str);
  tau_path_free(ll_path);
}

static void tau_compiler_emit_bc(tau_path_t* path, LLVMModuleRef llvm_module)
{
  tau_path_t* bc_path = tau_path_replace_extension(path, "bc");

  tau_string_t* bc_path_str = tau_path_to_string(bc_path);

  if (LLVMWriteBitcodeToFile(llvm_module, tau_string_begin(bc_path_str)))
    tau_log_error("LLVM", "Failed to emit bitcode file (%s).", tau_string_begin(bc_path_str));

  tau_string_free(bc_path_str);
  tau_path_free(bc_path);
}

static void tau_compiler_emit_obj(tau_path_t* path, LLVMModuleRef llvm_module)
{
  tau_path_t* obj_path = tau_path_replace_extension(path, "obj");

  tau_string_t* obj_path_str = tau_path_to_string(obj_path);

  char* tau_error_str = NULL;

  if (LLVMTargetMachineEmitToFile(tau_llvm_get_machine(), llvm_module, tau_string_begin(obj_path_str), LLVMObjectFile, &tau_error_str))
  {
    tau_log_error("LLVM", "Failed to emit object file (%s): %s", tau_string_begin(obj_path_str), tau_error_str);
    LLVMDisposeMessage(tau_error_str);
  }

  tau_string_free(obj_path_str);
  tau_path_free(obj_path);
}

static void tau_compiler_emit_asm(tau_path_t* path, LLVMModuleRef llvm_module)
{
  tau_path_t* asm_path = tau_path_replace_extension(path, "asm");

  tau_string_t* asm_path_str = tau_path_to_string(asm_path);

  char* tau_error_str = NULL;

  if (LLVMTargetMachineEmitToFile(tau_llvm_get_machine(), llvm_module, tau_string_begin(asm_path_str), LLVMAssemblyFile, &tau_error_str))
  {
    tau_log_error("LLVM", "Failed to emit assembly file (%s): %s", tau_string_begin(asm_path_str), tau_error_str);
    LLVMDisposeMessage(tau_error_str);
  }

  tau_string_free(asm_path_str);
  tau_path_free(asm_path);
}

tau_environment_t* tau_compiler_process_file(tau_compiler_t* compiler, tau_path_t* path)
{
  if (!tau_file_exists(path))
  {
    tau_string_t* tau_path_str = tau_path_to_string(path);

    tau_log_error("main", "File does not exist: %s", tau_string_begin(tau_path_str));

    tau_string_free(tau_path_str);

    return NULL;
  }

  tau_environment_t* env = tau_environment_init(
    tau_symtable_init(NULL),
    tau_typebuilder_init(tau_llvm_get_context(), tau_llvm_get_data()),
    tau_typetable_init(),
    tau_llvm_get_context(),
    tau_llvm_get_data(),
    LLVMModuleCreateWithNameInContext("module", tau_llvm_get_context()),
    LLVMCreateBuilderInContext(tau_llvm_get_context())
  );

  size_t tau_path_len = tau_path_to_cstr(path, NULL, 0);
  char* tau_path_cstr = (char*)malloc(sizeof(char) * (tau_path_len + 1));
  tau_path_to_cstr(path, tau_path_cstr, tau_path_len + 1);

  tau_vector_push(env->paths, tau_path_cstr);

  size_t src_len = tau_file_read(path, NULL, 0);
  char* src_cstr = (char*)malloc((src_len + 1) * sizeof(char));
  tau_file_read(path, src_cstr, src_len + 1);

  tau_vector_push(env->sources, src_cstr);

  tau_error_bag_t* errors = tau_error_bag_init(10);

  {
    tau_lexer_t* lexer = tau_lexer_init();

    tau_time_it("lexer", tau_lexer_lex(lexer, tau_path_cstr, src_cstr, env->tokens, errors));

    tau_lexer_free(lexer);

    if (!tau_error_bag_empty(errors))
    {
      tau_error_bag_print(errors);
      tau_error_bag_free(errors);
      exit(EXIT_FAILURE);
    }
  }

  if (tau_options_get_dump_tokens(compiler->options))
    tau_compiler_dump_tokens(path, env->tokens);

  tau_ast_node_t* root_node = NULL;

  {
    tau_parser_t* parser = tau_parser_init();

    tau_time_it("parser", root_node = tau_parser_parse(parser, env->tokens, errors));

    tau_parser_free(parser);

    if (!tau_error_bag_empty(errors))
    {
      tau_error_bag_print(errors);
      tau_error_bag_free(errors);
      exit(EXIT_FAILURE);
    }
  }

  if (tau_options_get_dump_ast(compiler->options))
    tau_compiler_dump_ast(path, root_node);

  {
    tau_nameres_ctx_t* tau_nameres_ctx = tau_nameres_ctx_init(env->symtable, errors);

    tau_time_it("analysis:nameres", tau_ast_node_nameres(tau_nameres_ctx, root_node));

    tau_nameres_ctx_free(tau_nameres_ctx);

    if (!tau_error_bag_empty(errors))
    {
      tau_error_bag_print(errors);
      tau_error_bag_free(errors);
      exit(EXIT_FAILURE);
    }
  }

  {
    tau_typecheck_ctx_t* tau_typecheck_ctx = tau_typecheck_ctx_init(env->typebuilder, env->typetable, errors);

    tau_time_it("analysis:typecheck", tau_ast_node_typecheck(tau_typecheck_ctx, root_node));

    tau_typecheck_ctx_free(tau_typecheck_ctx);

    if (!tau_error_bag_empty(errors))
    {
      tau_error_bag_print(errors);
      tau_error_bag_free(errors);
      exit(EXIT_FAILURE);
    }
  }

  {
    tau_ctrlflow_ctx_t* tau_ctrlflow_ctx = tau_ctrlflow_ctx_init(errors);

    tau_time_it("analysis:ctrlflow", tau_ast_node_ctrlflow(tau_ctrlflow_ctx, root_node));

    tau_ctrlflow_ctx_free(tau_ctrlflow_ctx);

    if (!tau_error_bag_empty(errors))
    {
      tau_error_bag_print(errors);
      tau_error_bag_free(errors);
      exit(EXIT_FAILURE);
    }
  }

  {
    tau_codegen_ctx_t* tau_codegen_ctx = tau_codegen_ctx_init(
      env->typebuilder,
      env->typetable,
      env->llvm_context,
      env->llvm_layout,
      env->llvm_module,
      env->llvm_builder
    );

    tau_time_it("codegen", tau_ast_node_codegen(tau_codegen_ctx, root_node));

    tau_codegen_ctx_free(tau_codegen_ctx);
  }

  tau_error_bag_free(errors);

  if (tau_options_get_dump_ll(compiler->options))
    tau_compiler_emit_ll(path, env->llvm_module);

  LLVMVerifyModule(env->llvm_module, LLVMAbortProcessAction, NULL);

  LLVMPassBuilderOptionsRef llvm_pass_builder_options = LLVMCreatePassBuilderOptions();
  LLVMPassBuilderOptionsSetVerifyEach(llvm_pass_builder_options, true);

#if TAU_DEBUG
  LLVMPassBuilderOptionsSetDebugLogging(llvm_pass_builder_options, true);
#endif

  // LLVMRunPasses(llvm_module, "default<O3>", tau_llvm_get_machine(), llvm_pass_builder_options);

  LLVMDisposePassBuilderOptions(llvm_pass_builder_options);

  if (tau_options_get_dump_bc(compiler->options))
    tau_compiler_emit_bc(path, env->llvm_module);

  if (tau_options_get_dump_asm(compiler->options))
    tau_compiler_emit_asm(path, env->llvm_module);

  tau_compiler_emit_obj(path, env->llvm_module);

  return env;
}

tau_compiler_t* tau_compiler_init(void)
{
  tau_compiler_t* compiler = (tau_compiler_t*)malloc(sizeof(tau_compiler_t));

  tau_log_set_stream(stdout);
  tau_crumb_set_stream(stdout);

  compiler->options = tau_options_ctx_init();

  return compiler;
}

void tau_compiler_free(tau_compiler_t* compiler)
{
  if (!tau_options_get_should_exit(compiler->options))
  {
    tau_ast_registry_free();
    tau_token_registry_free();
    tau_llvm_free();
  }

  tau_options_ctx_free(compiler->options);

  free(compiler);
}

int tau_compiler_main(tau_compiler_t* compiler, int argc, const char* argv[])
{
  tau_options_parse(compiler->options, argc, argv);

  if (tau_options_get_should_exit(compiler->options))
    return EXIT_SUCCESS;

  tau_log_set_verbose(tau_options_get_is_verbose(compiler->options));
  tau_log_set_level(tau_options_get_log_level(compiler->options));

  tau_time_it("LLVM:init", tau_llvm_init());

  if (tau_vector_empty(tau_options_get_input_files(compiler->options)))
  {
    tau_log_fatal("main", "No input files provided! ");
    return EXIT_FAILURE;
  }

  tau_vector_t* input_files = tau_options_get_input_files(compiler->options);

  TAU_VECTOR_FOR_LOOP(i, input_files)
  {
    const char* tau_path_cstr = (const char*)tau_vector_get(input_files, i);
    tau_path_t* path = tau_path_init_with_cstr(tau_path_cstr);

    tau_environment_t* env = tau_compiler_process_file(compiler, path);

    TAU_VECTOR_FOR_LOOP(i, env->paths)
    {
      free(tau_vector_get(env->paths, i));
    }

    TAU_VECTOR_FOR_LOOP(i, env->sources)
    {
      free(tau_vector_get(env->sources, i));
    }

    LLVMDisposeBuilder(env->llvm_builder);
    LLVMDisposeModule(env->llvm_module);

    tau_typetable_free(env->typetable);
    tau_typebuilder_free(env->typebuilder);
    tau_symtable_free(env->symtable);

    tau_environment_free(env);

    tau_path_free(path);
  }

  return EXIT_SUCCESS;
}
