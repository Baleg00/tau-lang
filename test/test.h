#ifndef TAU_TEST_H
#define TAU_TEST_H

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <setjmp.h>

#include "esc_seq.h"

#ifdef assert
# undef assert
#endif

typedef enum test_context_kind_e
{
  TEST_CONTEXT_TEST,
  TEST_CONTEXT_BEFORE_EACH,
  TEST_CONTEXT_AFTER_EACH,
  TEST_CONTEXT_DESCRIBE,
  TEST_CONTEXT_IT,
} test_context_kind_t;

/** Represents a test context. */
typedef struct test_context_s test_context_t;

struct test_context_s
{
  test_context_kind_t kind; /** Context kind. */

  test_context_t* parent; /** Parent context. */
  size_t depth; /** Context depth. */

  int result; /** Context result. (EXIT_SUCCESS or EXIT_FAILURE) */

  volatile bool has_before_each_jmp_buf; /** Indicates that the context has a before_each block. */
  jmp_buf before_each_jmp_buf; /** Jump buffer of the context's before_each block. */

  volatile bool has_after_each_jmp_buf; /** Indicates that the context has an after_each block. */
  jmp_buf after_each_jmp_buf; /** Jump buffer of the context's after_each block. */
  
  jmp_buf child_jmp_buf; /** Jump buffer of a child of the context. */
};

#define end()\
    switch (ctx.kind)\
    {\
    case TEST_CONTEXT_TEST:\
    {\
      return ctx.result;\
    }\
    case TEST_CONTEXT_BEFORE_EACH:\
    {\
      longjmp(ctx.parent->child_jmp_buf, 1);\
    }\
    case TEST_CONTEXT_AFTER_EACH:\
    {\
      longjmp(ctx.parent->child_jmp_buf, 1);\
    }\
    case TEST_CONTEXT_DESCRIBE:\
    {\
      if (ctx.result == EXIT_FAILURE)\
      {\
        ctx.parent->result = EXIT_FAILURE;\
      }\
      break;\
    }\
    case TEST_CONTEXT_IT:\
    {\
      printf(ESC_FG_BRIGHT_GREEN " passed\n" ESC_RESET);\
      break;\
    }\
    }\
  }

#define test()\
  int main(void)\
  {\
    test_context_t ctx = {\
      .kind = TEST_CONTEXT_TEST,\
      .parent = NULL,\
      .depth = 0,\
      .result = EXIT_SUCCESS,\
      .has_before_each_jmp_buf = false,\
      .has_after_each_jmp_buf = false,\
    };\

#define before_each()\
  for (size_t j = 0; j == 0; j = 1)\
  {\
    test_context_t* parent = &ctx;\
    test_context_t ctx = {\
      .kind = TEST_CONTEXT_BEFORE_EACH,\
      .parent = parent,\
      .depth = parent->depth + 1,\
      .result = EXIT_SUCCESS,\
      .has_before_each_jmp_buf = false,\
      .has_after_each_jmp_buf = false,\
    };\
    if (setjmp(ctx.parent->before_each_jmp_buf) == 0)\
    {\
      ctx.parent->has_before_each_jmp_buf = true;\
      break;\
    }\

#define describe(WHAT)\
  {\
    test_context_t* parent = &ctx;\
    test_context_t ctx = {\
      .kind = TEST_CONTEXT_DESCRIBE,\
      .parent = parent,\
      .depth = parent->depth + 1,\
      .result = EXIT_SUCCESS,\
      .has_before_each_jmp_buf = false,\
      .has_after_each_jmp_buf = false,\
    };\
    if (ctx.parent->has_before_each_jmp_buf)\
    {\
      if (setjmp(ctx.parent->child_jmp_buf) == 0)\
      {\
        longjmp(ctx.parent->before_each_jmp_buf, 1);\
      }\
    }\
    for (size_t i = 0; i < ctx.depth; ++i)\
    {\
      printf("  ");\
    }\
    printf("%s\n", (WHAT));\

#define it(SHOULD)\
  for (size_t j = 0; j == 0; j = 1)\
  {\
    test_context_t* parent = &ctx;\
    test_context_t ctx = {\
      .kind = TEST_CONTEXT_IT,\
      .parent = parent,\
      .depth = parent->depth + 1,\
      .result = EXIT_SUCCESS,\
      .has_before_each_jmp_buf = false,\
      .has_after_each_jmp_buf = false,\
    };\
    if (ctx.parent->has_before_each_jmp_buf)\
    {\
      if (setjmp(ctx.parent->child_jmp_buf) == 0)\
      {\
        longjmp(ctx.parent->before_each_jmp_buf, 1);\
      }\
    }\
    for (size_t i = 0; i < ctx.depth; ++i)\
    {\
      printf("  ");\
    }\
    printf(ESC_FG_BRIGHT_BLACK "%s" ESC_RESET, (SHOULD));

#define assert(COND)\
  if (!(COND))\
  {\
    printf(ESC_FG_BRIGHT_RED " failed\n");\
    for (size_t i = 0; i < ctx.depth + 1; ++i)\
    {\
      printf("  ");\
    }\
    printf("assert(%s)\n" ESC_RESET, #COND);\
    ctx.parent->result = EXIT_FAILURE;\
    break;\
  }

#define assert_true(COND)\
  if (!(COND))\
  {\
    printf(ESC_FG_BRIGHT_RED " failed\n");\
    for (size_t i = 0; i < ctx.depth + 1; ++i)\
    {\
      printf("  ");\
    }\
    printf("assert_true(%s)\n" ESC_RESET, #COND);\
    ctx.parent->result = EXIT_FAILURE;\
    break;\
  }

#define assert_false(COND)\
  if ((COND))\
  {\
    printf(ESC_FG_BRIGHT_RED " failed\n");\
    for (size_t i = 0; i < ctx.depth + 1; ++i)\
    {\
      printf("  ");\
    }\
    printf("assert_false(%s)\n" ESC_RESET, #COND);\
    ctx.parent->result = EXIT_FAILURE;\
    break;\
  }

#define assert_null(VALUE)\
  if ((VALUE) != NULL)\
  {\
    printf(ESC_FG_BRIGHT_RED " failed\n");\
    for (size_t i = 0; i < ctx.depth + 1; ++i)\
    {\
      printf("  ");\
    }\
    printf("assert_null(%s)\n" ESC_RESET, #VALUE);\
    ctx.parent->result = EXIT_FAILURE;\
    break;\
  }

#define assert_not_null(VALUE)\
  if ((VALUE) == NULL)\
  {\
    printf(ESC_FG_BRIGHT_RED " failed\n");\
    for (size_t i = 0; i < ctx.depth + 1; ++i)\
    {\
      printf("  ");\
    }\
    printf("assert_not_null(%s)\n" ESC_RESET, #VALUE);\
    ctx.parent->result = EXIT_FAILURE;\
    break;\
  }

#define assert_equal(ACTUAL, EXPECTED)\
  if ((ACTUAL) != (EXPECTED))\
  {\
    printf(ESC_FG_BRIGHT_RED " failed\n");\
    for (size_t i = 0; i < ctx.depth + 1; ++i)\
    {\
      printf("  ");\
    }\
    printf("assert_equal(%s, %s)\n" ESC_RESET, #ACTUAL, #EXPECTED);\
    ctx.parent->result = EXIT_FAILURE;\
    break;\
  }

#define assert_not_equal(ACTUAL, UNEXPECTED)\
  if ((ACTUAL) == (UNEXPECTED))\
  {\
    printf(ESC_FG_BRIGHT_RED " failed\n");\
    for (size_t i = 0; i < ctx.depth + 1; ++i)\
    {\
      printf("  ");\
    }\
    printf("assert_not_equal(%s, %s)\n" ESC_RESET, #ACTUAL, #UNEXPECTED);\
    ctx.parent->result = EXIT_FAILURE;\
    break;\
  }

#endif