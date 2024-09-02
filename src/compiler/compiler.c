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

static void compiler_dump_tokens(const char* path, vector_t* tokens)
{
  string_t* tokens_path = string_init_with_cstr(path);
  string_append_cstr(tokens_path, ".tokens.json");

  FILE* tokens_file = fopen(string_begin(tokens_path), "w");
  ASSERT(tokens_file != NULL);

  token_json_dump_vector(tokens_file, tokens);

  fclose(tokens_file);

  string_free(tokens_path);
}

static void compiler_dump_ast(const char* path, ast_node_t* root)
{
  string_t* ast_path = string_init_with_cstr(path);
  string_append_cstr(ast_path, ".ast.json");

  FILE* ast_file = fopen(string_begin(ast_path), "w");
  ASSERT(ast_file != NULL);

  ast_node_dump_json(ast_file, root);
  
  fclose(ast_file);

  string_free(ast_path);
}

static void compiler_emit_ll(const char* path, LLVMModuleRef llvm_module)
{
  string_t* ll_path = string_init_with_cstr(path);
  string_append_cstr(ll_path, ".ll");

  char* error_str = NULL;
  
  if (LLVMPrintModuleToFile(llvm_module, string_begin(ll_path), &error_str))
  {
    log_error("LLVM:LLVMPrintModuleToFile", error_str);
    LLVMDisposeMessage(error_str);
    string_free(ll_path);
    return;
  }

  string_free(ll_path);
}

static void compiler_emit_bc(const char* path, LLVMModuleRef llvm_module)
{
  string_t* bc_path = string_init_with_cstr(path);
  string_append_cstr(bc_path, ".bc");

  if (LLVMWriteBitcodeToFile(llvm_module, string_begin(bc_path)))
  {
    log_error("LLVM:LLVMWriteBitcodeToFile", "Failed to write bitcode to file.");
    string_free(bc_path);
    return;
  }

  string_free(bc_path);
}

static void compiler_emit_obj(const char* path, LLVMModuleRef llvm_module)
{
  string_t* obj_path = string_init_with_cstr(path);
  string_append_cstr(obj_path, ".obj");

  char* error_str = NULL;

  if (LLVMTargetMachineEmitToFile(llvm_get_machine(), llvm_module, string_begin(obj_path), LLVMObjectFile, &error_str))
  {
    log_error("LLVM:LLVMTargetMachineEmitToFile", error_str);
    LLVMDisposeMessage(error_str);
    string_free(obj_path);
    return;
  }

  string_free(obj_path);
}

static void compiler_emit_asm(const char* path, LLVMModuleRef llvm_module)
{
  string_t* asm_path = string_init_with_cstr(path);
  string_append_cstr(asm_path, ".asm");

  char* error_str = NULL;

  if (LLVMTargetMachineEmitToFile(llvm_get_machine(), llvm_module, string_begin(asm_path), LLVMAssemblyFile, &error_str))
  {
    log_error("LLVM:LLVMTargetMachineEmitToFile", error_str);
    LLVMDisposeMessage(error_str);
    string_free(asm_path);
    return;
  }

  string_free(asm_path);
}

static void compiler_process_file(compiler_t* compiler, const char* path)
{
  size_t src_len = file_read(path, NULL, 0);
  char* src = (char*)malloc((src_len + 1) * sizeof(char));
  file_read(path, src, src_len + 1);

  lexer_t* lexer = lexer_init();
  vector_t* toks = NULL;
  time_it("lexer", toks = lexer_lex(lexer, path, src));

  if (options_get_dump_tokens(compiler->options))
    compiler_dump_tokens(path, toks);

  parser_t* parser = parser_init();
  ast_node_t* root_node = NULL;
  time_it("parser", root_node = parser_parse(parser, toks));

  if (options_get_dump_ast(compiler->options))
    compiler_dump_ast(path, root_node);

  nameres_ctx_t* nameres_ctx = nameres_ctx_init();
  time_it("analysis:nameres", ast_node_nameres(nameres_ctx, root_node));

  typecheck_ctx_t* typecheck_ctx = typecheck_ctx_init(llvm_get_context(), llvm_get_data());
  time_it("analysis:typecheck", ast_node_typecheck(typecheck_ctx, root_node));

  ctrlflow_ctx_t* ctrlflow_ctx = ctrlflow_ctx_init();
  time_it("analysis:ctrlflow", ast_node_ctrlflow(ctrlflow_ctx, root_node));

  LLVMModuleRef llvm_module = LLVMModuleCreateWithNameInContext("module", llvm_get_context());
  codegen_ctx_t* codegen_ctx = codegen_ctx_init(typecheck_ctx->typetable, llvm_get_context(), llvm_get_data(), llvm_module);
  time_it("codegen", ast_node_codegen(codegen_ctx, root_node));

  LLVMVerifyModule(llvm_module, LLVMAbortProcessAction, NULL);

  LLVMPassBuilderOptionsRef llvm_pass_builder_options = LLVMCreatePassBuilderOptions();
  LLVMPassBuilderOptionsSetVerifyEach(llvm_pass_builder_options, true);

#ifdef TAU_DEBUG
  LLVMPassBuilderOptionsSetDebugLogging(llvm_pass_builder_options, true);
#endif

  // LLVMRunPasses(llvm_module, "default<O3>", llvm_get_machine(), llvm_pass_builder_options);

  LLVMDisposePassBuilderOptions(llvm_pass_builder_options);

  if (options_get_dump_ll(compiler->options))
    compiler_emit_ll(path, llvm_module);
  
  if (options_get_dump_bc(compiler->options))
    compiler_emit_bc(path, llvm_module);

  if (options_get_dump_asm(compiler->options))
    compiler_emit_asm(path, llvm_module);

  compiler_emit_obj(path, llvm_module);

  LLVMDisposeModule(llvm_module);

  codegen_ctx_free(codegen_ctx);
  ctrlflow_ctx_free(ctrlflow_ctx);
  typecheck_ctx_free(typecheck_ctx);
  nameres_ctx_free(nameres_ctx);
  parser_free(parser);

  vector_free(toks);
  lexer_free(lexer);
  
  free(src);
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
  ast_registry_free();
  token_registry_free();
  options_ctx_free(compiler->options);
  llvm_free();
  free(compiler);
}

int compiler_main(compiler_t* compiler, int argc, const char* argv[])
{
  options_parse(compiler->options, argc, argv);

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
    compiler_process_file(compiler, (const char*)vector_get(input_files, i));

  return EXIT_SUCCESS;
}
