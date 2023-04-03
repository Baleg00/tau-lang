#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "util.h"
#include "log.h"
#include "crumb.h"
#include "cli.h"

#include "file.h"
#include "token.h"
#include "ast.h"
#include "lexer.h"
#include "parser.h"
#include "analzyer.h"

#include "memtrace.h"

#define MAX_INPUT_FILES 32

static struct {
  struct {
    bool verbose;
    bool emit_tokens;
    bool emit_ast;
  } flags;

  struct {
    int log_level;
  } params;

  struct {
    char* paths[MAX_INPUT_FILES];
    size_t count;
  } input_files;
} compiler_state;

void compiler_state_init(void)
{
  compiler_state.flags.verbose = false;
  compiler_state.flags.emit_tokens = false;
  compiler_state.flags.emit_ast = false;

  compiler_state.params.log_level = LOG_LEVEL_INFO;

  memset(compiler_state.input_files.paths, 0, sizeof(compiler_state.input_files.paths));
  compiler_state.input_files.count = 0;
}

void flag_callback(cli_t* cli, queue_t* que, cli_opt_t* opt, void* user_ptr)
{
  *(bool*)user_ptr = true;
}

int main(int argc, char *argv[])
{
  log_set_file(stdout);
  crumb_set_file(stdout);

  compiler_state_init();

  cli_opt_t opts[] = {
    cli_opt_help(),
    cli_opt_version(TAU_VERSION),
    cli_opt_verbose(&compiler_state.flags.verbose),
    
    cli_opt_flag((char*[]){ "--dump-tokens" }, 1, "Dump tokens into json file.", &compiler_state.flags.emit_tokens),
    cli_opt_flag((char*[]){ "--dump-ast" }, 1, "Dump AST into json file.", &compiler_state.flags.emit_ast),
    cli_opt_int((char*[]){ "--log-level" }, 1, 'N', 1, &compiler_state.params.log_level, NULL, NULL, "Set log level.", NULL, NULL),

    cli_opt_sink(MAX_INPUT_FILES, compiler_state.input_files.paths, &compiler_state.input_files.count, NULL, NULL)
  };

  const char* usages[] = { "tau [option...] file..." };

  cli_t* cli = cli_init(opts, sizeof(opts) / sizeof(cli_opt_t), usages, sizeof(usages) / sizeof(const char*));
  cli_parse(cli, argc, argv);
  cli_free(cli);

  log_set_verbose(compiler_state.flags.verbose);
  log_set_level(compiler_state.params.log_level);

  if (compiler_state.input_files.count == 0)
  {
    log_fatal("main", "No input files provided! ");
    return EXIT_FAILURE;
  }

  log_debug("main", "Verbose mode: %s", compiler_state.flags.verbose ? "Enabled" : "Disabled");
  log_debug("main", "Log level: %s", log_level_to_string(compiler_state.params.log_level));
  log_debug("main", "Source files: (%d)", compiler_state.input_files.count);

  for (size_t i = 0; i < compiler_state.input_files.count; ++i)
  {
    char file_name_buf[255];
    file_name(compiler_state.input_files.paths[i], file_name_buf, sizeof(file_name_buf));
    log_debug("main", "    %s", file_name_buf);
  }

  for (size_t i = 0; i < compiler_state.input_files.count; ++i)
  {
    char* path = compiler_state.input_files.paths[i];

    char file_name_buf[255];
    file_name(path, file_name_buf, sizeof(file_name_buf));

    log_debug("main", "Processing source file: %s", file_name_buf);

    size_t src_len = file_read(path, NULL, 0);
    char* src = (char*)malloc((src_len + 1) * sizeof(char));

    file_read(path, src, src_len + 1);

    log_debug("main", "Performing lexical analysis...");

    lexer_t* lex = lexer_init(path, src);
    lexer_lex(lex);

    if (compiler_state.flags.emit_tokens)
    {
      char* tokens_path = malloc(strlen(path) + 11);
      strcpy(tokens_path, path);
      strcat(tokens_path, ".toks.json");

      FILE* tokens_file = fopen(tokens_path, "w");
      assert(tokens_file != NULL);
      token_list_json_dump(tokens_file, lex->toks);
      fclose(tokens_file);

      log_debug("main", "Dumped tokens: %s", tokens_path);

      free(tokens_path);
    }

    log_debug("main", "Performing syntax analysis...");

    parser_t* par = parser_init(lex->toks);
    parser_parse(par);

    if (compiler_state.flags.emit_ast)
    {
      char* ast_path = malloc(strlen(path) + 10);
      strcpy(ast_path, path);
      strcat(ast_path, ".ast.json");

      FILE* ast_file = fopen(ast_path, "w");
      assert(ast_file != NULL);
      ast_json_dump(ast_file, par->root);
      fclose(ast_file);

      log_debug("main", "Dumped AST: %s", ast_path);

      free(ast_path);
    }

    log_debug("main", "Performing semantic analysis...");

    analyzer_t* analyzer = analyzer_init();
    analyzer_analyze(analyzer, par->root);

    log_debug("main", "Cleaning up...");

    analyzer_free(analyzer);
    parser_free(par);
    lexer_free(lex);
    free(src);
  }

  return EXIT_SUCCESS;
}
