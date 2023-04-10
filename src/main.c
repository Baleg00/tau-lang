#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "util.h"
#include "log.h"
#include "crumb.h"
#include "cli.h"
#include "timer.h"
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
    bool emit_ast_flat;
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
    cli_opt_flag((char*[]){ "--dump-ast-flat" }, 1, "Dump flat AST into json file.", &compiler_state.flags.emit_ast_flat),
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

  for (size_t i = 0; i < compiler_state.input_files.count; ++i)
  {
    char* path = compiler_state.input_files.paths[i];

    char file_name_buf[255];
    file_name(path, file_name_buf, sizeof(file_name_buf));

    log_trace("main", "(%s) File read.", file_name_buf);

    size_t src_len = file_read(path, NULL, 0);
    char* src = (char*)malloc((src_len + 1) * sizeof(char));

    file_read(path, src, src_len + 1);

    log_trace("main", "(%s) Lexical analysis.", file_name_buf);

    lexer_t* lex = lexer_init(path, src);
    time_it(lexer, lexer_lex(lex));

    if (compiler_state.flags.emit_tokens)
    {
      char toks_path_buf[255];
      strcpy(toks_path_buf, path);
      strcat(toks_path_buf, ".toks.json");

      FILE* toks_file = fopen(toks_path_buf, "w");
      assert(toks_file != NULL);
      token_list_json_dump(toks_file, lex->toks);
      fclose(toks_file);

      char toks_name_buf[255];
      file_name(toks_path_buf, toks_name_buf, sizeof(toks_name_buf));

      log_trace("main", "(%s) Token dump: %s", file_name_buf, toks_name_buf);
    }

    log_trace("main", "(%s) Syntax analysis.", file_name_buf);

    parser_t* par = parser_init(lex->toks);
    time_it(parser, parser_parse(par));

    if (compiler_state.flags.emit_ast)
    {
      char ast_path_buf[255];
      strcpy(ast_path_buf, path);
      strcat(ast_path_buf, ".ast.json");

      FILE* ast_file = fopen(ast_path_buf, "w");
      assert(ast_file != NULL);
      ast_json_dump(ast_file, par->root);
      fclose(ast_file);

      char ast_name_buf[255];
      file_name(ast_path_buf, ast_name_buf, sizeof(ast_name_buf));

      log_trace("main", "(%s) AST dump: %s", file_name_buf, ast_name_buf);
    }

    log_trace("main", "(%s) Semantic analysis.", file_name_buf);

    analyzer_t* analyzer = analyzer_init();
    time_it(analyzer, analyzer_analyze(analyzer, par->root));

    if (compiler_state.flags.emit_ast_flat)
    {
      char ast_path_buf[255];
      strcpy(ast_path_buf, path);
      strcat(ast_path_buf, ".ast.flat.json");

      FILE* ast_file = fopen(ast_path_buf, "w");
      assert(ast_file != NULL);
      ast_json_dump_flat(ast_file, par->root);
      fclose(ast_file);

      char ast_name_buf[255];
      file_name(ast_path_buf, ast_name_buf, sizeof(ast_name_buf));

      log_trace("main", "(%s) AST flat dump: %s", file_name_buf, ast_name_buf);
    }

    log_trace("main", "(%s) Cleanup.", file_name_buf);

    analyzer_free(analyzer);
    parser_free(par);
    lexer_free(lex);
    free(src);
  }

  return EXIT_SUCCESS;
}
