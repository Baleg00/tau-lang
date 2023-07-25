#ifndef TAU_COMPILER_H
#define TAU_COMPILER_H

#include <stdbool.h>

#include "arena.h"
#include "list.h"
#include "log.h"
#include "typedefs.h"

struct compiler_s
{
  arena_t* arena;

  list_t* input_files;

  struct
  {
    bool verbose;
    bool dump_tokens;
    bool dump_ast;
    bool dump_ast_flat;
    bool dump_tasm;
  } flags;

  struct
  {
    log_level_t log_level;
  } args;
};

void compiler_init(compiler_t* compiler);
void compiler_free(compiler_t* compiler);

int compiler_main(compiler_t* compiler, int argc, const char* argv[]);

#endif