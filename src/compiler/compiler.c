/**
 * \file
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "compiler/compiler.h"

#include <stdbool.h>
#include <stdint.h>

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
#include "utils/io/log.h"
#include "utils/memory/memtrace.h"

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

void compiler_process_file(compiler_t* compiler, path_t* path)
{
  size_t path_len = path_to_cstr(path, NULL, 0);
  char* path_cstr = (char*)malloc(sizeof(char) * (path_len + 1));
  path_to_cstr(path, path_cstr, path_len + 1);

  size_t src_len = file_read(path, NULL, 0);
  char* src = (char*)malloc((src_len + 1) * sizeof(char));
  file_read(path, src, src_len + 1);

  lexer_t* lexer = lexer_init();
  vector_t* tokens = NULL;
  time_it("lexer", tokens = lexer_lex(lexer, path_cstr, src));

  if (options_get_dump_tokens(compiler->options))
    compiler_dump_tokens(path, tokens);

  parser_t* parser = parser_init();
  ast_node_t* root_node = NULL;
  time_it("parser", root_node = parser_parse(parser, tokens));

  if (options_get_dump_ast(compiler->options))
    compiler_dump_ast(path, root_node);

  nameres_ctx_t* nameres_ctx = nameres_ctx_init();
  time_it("analysis:nameres", ast_node_nameres(nameres_ctx, root_node));

  typebuilder_t* typebuilder = typebuilder_init(llvm_get_context(), llvm_get_data());
  typetable_t* typetable = typetable_init();

  typecheck_ctx_t* typecheck_ctx = typecheck_ctx_init(typebuilder, typetable);
  time_it("analysis:typecheck", ast_node_typecheck(typecheck_ctx, root_node));

  ctrlflow_ctx_t* ctrlflow_ctx = ctrlflow_ctx_init();
  time_it("analysis:ctrlflow", ast_node_ctrlflow(ctrlflow_ctx, root_node));

  LLVMModuleRef llvm_mod = LLVMModuleCreateWithNameInContext("module", llvm_get_context());
  LLVMBuilderRef llvm_builder = LLVMCreateBuilderInContext(llvm_get_context());

  codegen_ctx_t* codegen_ctx = codegen_ctx_init(typecheck_ctx->typetable, llvm_get_context(), llvm_get_data(), llvm_mod, llvm_builder);
  time_it("codegen", ast_node_codegen(codegen_ctx, root_node));

  LLVMVerifyModule(llvm_mod, LLVMAbortProcessAction, NULL);

  LLVMPassBuilderOptionsRef llvm_pass_builder_options = LLVMCreatePassBuilderOptions();
  LLVMPassBuilderOptionsSetVerifyEach(llvm_pass_builder_options, true);

#ifdef TAU_DEBUG
  LLVMPassBuilderOptionsSetDebugLogging(llvm_pass_builder_options, true);
#endif

  // LLVMRunPasses(llvm_module, "default<O3>", llvm_get_machine(), llvm_pass_builder_options);

  LLVMDisposePassBuilderOptions(llvm_pass_builder_options);

  if (options_get_dump_ll(compiler->options))
    compiler_emit_ll(path, llvm_mod);

  if (options_get_dump_bc(compiler->options))
    compiler_emit_bc(path, llvm_mod);

  if (options_get_dump_asm(compiler->options))
    compiler_emit_asm(path, llvm_mod);

  compiler_emit_obj(path, llvm_mod);

  LLVMDisposeBuilder(llvm_builder);
  LLVMDisposeModule(llvm_mod);

  codegen_ctx_free(codegen_ctx);
  ctrlflow_ctx_free(ctrlflow_ctx);
  typecheck_ctx_free(typecheck_ctx);
  typetable_free(typetable);
  typebuilder_free(typebuilder);
  nameres_ctx_free(nameres_ctx);
  parser_free(parser);

  vector_free(tokens);
  lexer_free(lexer);

  free(src);
  free(path_cstr);
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

    compiler_process_file(compiler, path);

    path_free(path);
  }

  return EXIT_SUCCESS;
}
