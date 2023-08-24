#ifndef TAU_COMPILER_H
#define TAU_COMPILER_H

/**
 * \brief Compiler context.
 */
typedef struct compiler_s compiler_t;

compiler_t* compiler_init(void);
void compiler_free(compiler_t* compiler);

int compiler_main(compiler_t* compiler, int argc, const char* argv[]);

#endif