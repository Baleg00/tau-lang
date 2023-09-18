#include "compiler.h"

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "allocator.h"
#include "analzyer.h"
#include "arena.h"
#include "ast.h"
#include "cli.h"
#include "crumb.h"
#include "file.h"
#include "generator.h"
#include "lexer.h"
#include "list.h"
#include "log.h"
#include "memtrace.h"
#include "parser.h"
#include "symtable.h"
#include "timer.h"
#include "token.h"
#include "typetable.h"
#include "util.h"

#define COMPILER_MAX_BUFFER_SIZE 256

struct compiler_s
{
  allocator_t* allocator;

  list_t* input_files;

  struct
  {
    bool verbose;
    bool dump_tokens;
    bool dump_ast;
    bool dump_ast_flat;
    bool dump_ll;
  } flags;

  struct
  {
    log_level_t log_level;
  } args;
};

static void input_file_callback(cli_t* cli, queue_t* que, cli_opt_t* opt, const char* arg, void* user_data)
{
  unused(cli);
  unused(que);
  unused(opt);

  compiler_t* compiler = (compiler_t*)user_data;

  list_push_back(compiler->input_files, (void*)arg);
}

static void compiler_dump_tokens(compiler_t* compiler, const char* input_file_path, const char* input_file_name, list_t* tokens)
{
  unused(compiler);

  char tokens_file_path[COMPILER_MAX_BUFFER_SIZE];
  strcpy(tokens_file_path, input_file_path);
  strcat(tokens_file_path, ".tokens.json");

  FILE* toks_file = fopen(tokens_file_path, "w");
  assert(toks_file != NULL);
  token_list_json_dump(toks_file, tokens);
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
  ast_json_dump(ast_file, root);
  fclose(ast_file);

  log_trace("main", "(%s) AST dump: %s", input_file_name, ast_file_path);
}

static void compiler_dump_ast_flat(compiler_t* compiler, const char* input_file_path, const char* input_file_name, ast_node_t* root)
{
  unused(compiler);

  char ast_file_path[COMPILER_MAX_BUFFER_SIZE];
  strcpy(ast_file_path, input_file_path);
  strcat(ast_file_path, ".ast.flat.json");

  FILE* ast_file = fopen(ast_file_path, "w");
  assert(ast_file != NULL);
  ast_json_dump_flat(ast_file, root);
  fclose(ast_file);

  log_trace("main", "(%s) AST flat dump: %s", input_file_name, ast_file_path);
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

compiler_t* compiler_init(void)
{
  compiler_t* compiler = (compiler_t*)allocator_allocate(allocator_global(), sizeof(compiler_t));

  log_set_stream(stdout);
  crumb_set_stream(stdout);

  compiler->allocator = arena_init();

  compiler->input_files = list_init();

  compiler->flags.verbose = false;
  compiler->flags.dump_tokens = false;
  compiler->flags.dump_ast = false;
  compiler->flags.dump_ast_flat = false;
  compiler->flags.dump_ll = false;

  compiler->args.log_level = LOG_LEVEL_WARN;

  return compiler;
}

void compiler_free(compiler_t* compiler)
{
  list_free(compiler->input_files);

  typedesc_cleanup();
  ast_cleanup();

  allocator_free(compiler->allocator);
  allocator_deallocate(allocator_global(), compiler);
}

int compiler_main(compiler_t* compiler, int argc, const char* argv[])
{
  cli_opt_t opts[] = {
    cli_opt_help(),
    cli_opt_version(TAU_VERSION),
    cli_opt_verbose(&compiler->flags.verbose),

    cli_opt_flag(cli_names("--dump-toks"),     1, "Dump tokens into json file.",   &compiler->flags.dump_tokens),
    cli_opt_flag(cli_names("--dump-ast"),      1, "Dump AST into json file.",      &compiler->flags.dump_ast),
    cli_opt_flag(cli_names("--dump-ast-flat"), 1, "Dump flat AST into json file.", &compiler->flags.dump_ast_flat),
    cli_opt_flag(cli_names("--dump-ll"),       1, "Dump LLVM IR into ll file.",    &compiler->flags.dump_ll),
    
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

    lexer_t* lexer = lexer_init(input_file_path, src);

    list_t* toks = list_init();

    time_it("lexer", lexer_lex(lexer, toks));

    if (compiler->flags.dump_tokens)
      compiler_dump_tokens(compiler, input_file_path, input_file_name, toks);

    log_trace("main", "(%s) Syntax analysis.", input_file_name);

    parser_t* parser = parser_init();

    ast_node_t* root = ast_node_init(AST_PROG);

    time_it("parser", parser_parse(parser, toks, root));

    if (compiler->flags.dump_ast)
      compiler_dump_ast(compiler, input_file_path, input_file_name, root);

    log_trace("main", "(%s) Semantic analysis.", input_file_name);

    analyzer_t analyzer;
    analyzer_init(compiler->allocator, &analyzer);

    symtable_t* symtable = symtable_init(NULL);
    typetable_t* typetable = typetable_init();

    time_it("analyzer", analyzer_analyze(&analyzer, root, symtable, typetable));

    if (compiler->flags.dump_ast_flat)
      compiler_dump_ast_flat(compiler, input_file_path, input_file_name, root);

    log_trace("main", "(%s) IR generation.", input_file_name);

    generator_t* generator = generator_init();
    LLVMModuleRef llvm_module = NULL;

    time_it("generator", llvm_module = generator_generate(generator, root));

    if (compiler->flags.dump_ll)
      compiler_dump_ll(compiler, input_file_path, input_file_name, llvm_module);

    log_trace("main", "(%s) Cleanup.", input_file_name);

    generator_free(generator);

    symtable_free(symtable);
    typetable_free(typetable);

    analyzer_free(&analyzer);
    
    parser_free(parser);

    list_for_each(toks, (list_for_each_func_t)token_free);
    list_free(toks);
    lexer_free(lexer);
    
    free(src);
  }

  return EXIT_SUCCESS;
}
