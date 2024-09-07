#include "compiler/compiler.h"

int main(int argc, const char* argv[])
{
  compiler_t* compiler = compiler_init();

  int result = compiler_main(compiler, argc, argv);

  compiler_free(compiler);

  return result;
}
