#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>

#include "log.h"
#include "crumb.h"
#include "cli.h"

#include "lexer.h"
#include "parser.h"

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
    cli_opt_integer((char*[]){ "--log-level" }, 1, 'N', 1, &compiler_state.params.log_level, NULL, NULL, "Set log level.", NULL, NULL),

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
    log_debug("main", "    %s", compiler_state.input_files.paths[i]);

  for (size_t i = 0; i < compiler_state.input_files.count; ++i)
  {
    const char* source_path = compiler_state.input_files.paths[i];

    log_debug("main", "Processing source file: %s", source_path);
    log_debug("main", "Performing lexical analysis...");

    lexer_t* lex = lexer_init(source_path);
    lexer_lex(lex);

    if (compiler_state.flags.emit_tokens)
    {
      char* tokens_path = malloc(strlen(source_path) + 11);
      strcpy(tokens_path, source_path);
      strcat(tokens_path, ".toks.json");

      log_debug("main", "Dumping tokens: %s", tokens_path);

      FILE* tokens_file = fopen(tokens_path, "w");
      assert(tokens_file != NULL);
      token_list_json_dump(tokens_file, lex->toks);
      fclose(tokens_file);

      free(tokens_path);
    }

    log_debug("main", "Performing syntax analysis...");

    parser_t* par = parser_init(lex->toks);
    parser_parse(par);

    if (compiler_state.flags.emit_ast)
    {
      char* ast_path = malloc(strlen(source_path) + 10);
      strcpy(ast_path, source_path);
      strcat(ast_path, ".ast.json");

      log_debug("main", "Dumping AST: %s", ast_path);

      FILE* ast_file = fopen(ast_path, "w");
      assert(ast_file != NULL);
      ast_json_dump(ast_file, par->root);
      fclose(ast_file);

      free(ast_path);
    }

    parser_free(par);
    lexer_free(lex);
  }

  return EXIT_SUCCESS;
}
