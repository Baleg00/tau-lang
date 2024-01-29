#include "compiler.h"

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include <llvm-c/BitWriter.h>
#include <llvm-c/Core.h>
#include <llvm-c/Target.h>
#include <llvm-c/TargetMachine.h>

#include "ast/ast.h"
#include "ast/registry.h"
#include "stages/analysis/symtable.h"
#include "stages/analysis/typetable.h"
#include "stages/lexer/lexer.h"
#include "stages/lexer/token.h"
#include "stages/parser/parser.h"
#include "utils/collections/list.h"
#include "utils/common.h"
#include "utils/crumb.h"
#include "utils/io/cli.h"
#include "utils/io/file.h"
#include "utils/io/log.h"
#include "utils/memory/memtrace.h"
#include "utils/timer.h"

#define COMPILER_MAX_BUFFER_SIZE 256

struct compiler_t
{
  LLVMContextRef llvm_context;
  LLVMTargetRef llvm_target;
  LLVMTargetDataRef llvm_layout;
  LLVMTargetMachineRef llvm_machine;

  list_t* input_files;

  struct
  {
    bool verbose;
    bool dump_tokens;
    bool dump_ast;
    bool dump_ll;
    bool dump_bc;
  } flags;

  struct
  {
    log_level_t log_level;
  } args;
};

static void compiler_llvm_init(compiler_t* compiler)
{
  if (LLVMInitializeNativeTarget())
  {
    log_fatal("llvm", "Failed to initialize native target.");
    exit(EXIT_FAILURE);
  }

  char* error_str = NULL;

  compiler->llvm_context = LLVMContextCreate();

  char* target_triple = LLVMGetDefaultTargetTriple();

  if (target_triple == NULL)
  {
    log_fatal("llvm", "Failed to get target triple.");
    exit(EXIT_FAILURE);
  }
  
  if (LLVMGetTargetFromTriple(target_triple, &compiler->llvm_target, &error_str))
  {
    log_fatal("llvm", "Failed to get target from triple.");
    fputs(error_str, stderr);
    exit(EXIT_FAILURE);
  }

  char* cpu_name = LLVMGetHostCPUName();

  if (cpu_name == NULL)
  {
    log_fatal("llvm", "Failed to get target CPU name.");
    exit(EXIT_FAILURE);
  }

  char* cpu_features = LLVMGetHostCPUFeatures();

  if (cpu_features == NULL)
  {
    log_fatal("llvm", "Failed to get target CPU features.");
    exit(EXIT_FAILURE);
  }

  compiler->llvm_machine = LLVMCreateTargetMachine(
    compiler->llvm_target,
    target_triple,
    cpu_name,
    cpu_features,
    LLVMCodeGenLevelDefault,
    LLVMRelocDefault,
    LLVMCodeModelDefault
  );

  if (compiler->llvm_machine == NULL)
  {
    log_fatal("llvm", "Failed to create target machine.");
    exit(EXIT_FAILURE);
  }

  compiler->llvm_layout = LLVMCreateTargetDataLayout(compiler->llvm_machine);

  if (compiler->llvm_layout == NULL)
  {
    log_fatal("llvm", "Failed to create target data layout.");
    exit(EXIT_FAILURE);
  }
}

static void compiler_llvm_free(compiler_t* compiler)
{
  LLVMDisposeTargetData(compiler->llvm_layout);
  LLVMDisposeTargetMachine(compiler->llvm_machine);
  LLVMContextDispose(compiler->llvm_context);
}

static void input_file_callback(cli_t* cli, queue_t* que, cli_opt_t* opt, const char* arg, void* user_data)
{
  unused(cli);
  unused(que);
  unused(opt);

  compiler_t* compiler = (compiler_t*)user_data;

  list_push_back(compiler->input_files, (void*)arg);
}

static void compiler_dump_tokens(compiler_t* compiler, const char* input_file_path, const char* input_file_name, vector_t* tokens)
{
  unused(compiler);

  char tokens_file_path[COMPILER_MAX_BUFFER_SIZE];
  strcpy(tokens_file_path, input_file_path);
  strcat(tokens_file_path, ".tokens.json");

  FILE* toks_file = fopen(tokens_file_path, "w");
  assert(toks_file != NULL);
  token_json_dump_vector(toks_file, tokens);
  fclose(toks_file);

  log_trace("main", "(%s) Token dump: %s", input_file_name, tokens_file_path);
}

static void compiler_dump_ast(compiler_t* compiler, const char* input_file_path, const char* input_file_name, ast_node_t* root)
{
  unused(compiler);

  char ast_file_path[COMPILER_MAX_BUFFER_SIZE];
  strcpy(ast_file_path, input_file_path);
  strcat(ast_file_path, ".ast.json");

  FILE* ast_file = fopen(ast_file_path, "w");
  assert(ast_file != NULL);
  ast_node_dump_json(ast_file, root);
  fclose(ast_file);

  log_trace("main", "(%s) AST dump: %s", input_file_name, ast_file_path);
}

static void compiler_dump_ll(compiler_t* compiler, const char* input_file_path, const char* input_file_name, LLVMModuleRef llvm_module)
{
  unused(compiler);

  char ll_file_path[COMPILER_MAX_BUFFER_SIZE];
  strcpy(ll_file_path, input_file_path);
  strcat(ll_file_path, ".ll");

  FILE* ll_file = fopen(ll_file_path, "w");
  assert(ll_file != NULL);

  char* ll_str = LLVMPrintModuleToString(llvm_module);
  fputs(ll_str, ll_file);
  LLVMDisposeMessage(ll_str);

  fclose(ll_file);

  log_trace("main", "(%s) LLVM IR dump: %s", input_file_name, ll_file_path);
}

static void compiler_dump_bc(compiler_t* compiler, const char* input_file_path, const char* input_file_name, LLVMModuleRef llvm_module)
{
  unused(compiler);

  char bc_file_path[COMPILER_MAX_BUFFER_SIZE];
  strcpy(bc_file_path, input_file_path);
  strcat(bc_file_path, ".bc");

  LLVMWriteBitcodeToFile(llvm_module, bc_file_path);
  
  log_trace("main", "(%s) LLVM bitcode dump: %s", input_file_name, bc_file_path);
}

compiler_t* compiler_init(void)
{
  compiler_t* compiler = (compiler_t*)malloc(sizeof(compiler_t));

  log_set_stream(stdout);
  crumb_set_stream(stdout);

  compiler->input_files = list_init();

  compiler->flags.verbose = false;
  compiler->flags.dump_tokens = false;
  compiler->flags.dump_ast = false;
  compiler->flags.dump_ll = false;

  compiler->args.log_level = LOG_LEVEL_WARN;

  time_it("LLVM:init", compiler_llvm_init(compiler));

  return compiler;
}

void compiler_free(compiler_t* compiler)
{
  compiler_llvm_free(compiler);

  ast_registry_free();
  
  list_free(compiler->input_files);

  free(compiler);
}

int compiler_main(compiler_t* compiler, int argc, const char* argv[])
{
  cli_opt_t opts[] = {
    cli_opt_help(),
    cli_opt_version(TAU_VERSION),
    cli_opt_verbose(&compiler->flags.verbose),

    cli_opt_flag(cli_names("--dump-toks"),     1, "Dump tokens into json file.",     &compiler->flags.dump_tokens),
    cli_opt_flag(cli_names("--dump-ast"),      1, "Dump AST into json file.",        &compiler->flags.dump_ast),
    cli_opt_flag(cli_names("--dump-ll"),       1, "Dump LLVM IR into ll file.",      &compiler->flags.dump_ll),
    cli_opt_flag(cli_names("--dump-bc"),       1, "Dump LLVM bitcode into bc file.", &compiler->flags.dump_bc),
    
    cli_opt_int(cli_names("--log-level"), 1, 'N', 1, &compiler->args.log_level, NULL, NULL, "Set log level.", NULL, NULL),
    
    cli_opt_sink(SIZE_MAX, NULL, NULL, input_file_callback, compiler)
  };

  const char* usages[] = { "tau [option...] file..." };

  cli_t* cli = cli_init(opts, countof(opts), usages, countof(usages));
  cli_parse(cli, argc, argv);
  cli_free(cli);

  log_set_verbose(compiler->flags.verbose);
  log_set_level(compiler->args.log_level);

  if (list_size(compiler->input_files) == 0)
  {
    log_fatal("main", "No input files provided! ");
    return EXIT_FAILURE;
  }

  char cwd[COMPILER_MAX_BUFFER_SIZE];
  file_dir(argv[0], cwd, sizeof(cwd));

  LIST_FOR_LOOP(it, compiler->input_files)
  {
    const char* input_file_path = (const char*)list_node_get(it);

    char input_file_name[COMPILER_MAX_BUFFER_SIZE];
    file_name(input_file_path, input_file_name, sizeof(input_file_name));

    log_trace("main", "(%s) File read.", input_file_name);

    size_t src_len = file_read(input_file_path, NULL, 0);
    char* src = (char*)malloc((src_len + 1) * sizeof(char));
    file_read(input_file_path, src, src_len + 1);

    log_trace("main", "(%s) Lexical analysis.", input_file_name);

    lexer_t* lexer = lexer_init();
    vector_t* toks = NULL;

    time_it("lexer", toks = lexer_lex(lexer, input_file_path, src));

    if (compiler->flags.dump_tokens)
      compiler_dump_tokens(compiler, input_file_path, input_file_name, toks);

    log_trace("main", "(%s) Syntax analysis.", input_file_name);

    parser_t* parser = parser_init();
    ast_node_t* root_node = NULL;

    time_it("parser", root_node = parser_parse(parser, toks));

    if (compiler->flags.dump_ast)
      compiler_dump_ast(compiler, input_file_path, input_file_name, root_node);

    log_trace("main", "(%s) Semantic analysis.", input_file_name);

    nameres_ctx_t* nameres_ctx = nameres_ctx_init();
    
    time_it("analysis:nameres", ast_node_nameres(nameres_ctx, root_node));

    typecheck_ctx_t* typecheck_ctx = typecheck_ctx_init(compiler->llvm_context, compiler->llvm_layout);
    
    time_it("analysis:typecheck", ast_node_typecheck(typecheck_ctx, root_node));

    log_trace("main", "(%s) LLVM IR generation.", input_file_name);

    LLVMModuleRef llvm_module = LLVMModuleCreateWithNameInContext("module", compiler->llvm_context);
    
    codegen_ctx_t* codegen_ctx = codegen_ctx_init(typecheck_ctx->typetable, compiler->llvm_context, compiler->llvm_layout, llvm_module);

    time_it("codegen", ast_node_codegen(codegen_ctx, root_node));

    if (compiler->flags.dump_ll)
      compiler_dump_ll(compiler, input_file_path, input_file_name, llvm_module);

    if (compiler->flags.dump_bc)
      compiler_dump_bc(compiler, input_file_path, input_file_name, llvm_module);

    log_trace("main", "(%s) Cleanup.", input_file_name);

    LLVMDisposeModule(llvm_module);

    codegen_ctx_free(codegen_ctx);
    typecheck_ctx_free(typecheck_ctx);
    nameres_ctx_free(nameres_ctx);
    parser_free(parser);

    VECTOR_FOR_LOOP(i, toks)
      token_free((token_t*)vector_get(toks, i));

    vector_free(toks);
    lexer_free(lexer);
    
    free(src);
  }

  return EXIT_SUCCESS;
}
