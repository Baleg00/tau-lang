#include "compiler/compiler.h"

int main(int argc, const char* argv[])
{
  tau_compiler_t* compiler = tau_compiler_init();

  int result = tau_compiler_main(compiler, argc, argv);

  tau_compiler_free(compiler);

  return result;
}
