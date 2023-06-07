#ifndef TAU_TEST_H
#define TAU_TEST_H

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include <setjmp.h>

#include "esc_seq.h"

typedef enum test_context_kind_e
{
  TEST_CONTEXT_TEST,
  TEST_CONTEXT_BEFORE,
  TEST_CONTEXT_AFTER,
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

  volatile int result; /** Context result. (EXIT_SUCCESS or EXIT_FAILURE) */
  
  volatile bool has_before_jmp_buf; /** Indicates that the context has a before block. */
  jmp_buf before_jmp_buf; /** Jump buffer of the context's before block. */

  volatile bool has_after_jmp_buf; /** Indicates that the context has an after block. */
  jmp_buf after_jmp_buf; /** Jump buffer of the context's after block. */
  
  volatile bool has_before_each_jmp_buf; /** Indicates that the context has a before_each block. */
  jmp_buf before_each_jmp_buf; /** Jump buffer of the context's before_each block. */
  
  volatile bool has_after_each_jmp_buf; /** Indicates that the context has an after_each block. */
  jmp_buf after_each_jmp_buf; /** Jump buffer of the context's after_each block. */
  
  jmp_buf exit_jmp_buf; /** Jump buffer for exiting the context. */

  jmp_buf return_jmp_buf; /** Jump buffer to return to. */
};

#define _print_indent(COUNT)\
  do {\
    for (size_t i = 1; i < (COUNT); ++i)\
    {\
      printf("  ");\
    }\
  } while (0)\

#define _do_before()\
  do {\
    if (_ctx.parent->has_before_jmp_buf)\
    {\
      _ctx.parent->has_before_jmp_buf = false;\
      if (setjmp(_ctx.parent->return_jmp_buf) == 0)\
      {\
        longjmp(_ctx.parent->before_jmp_buf, 1);\
      }\
    }\
  } while (0)\

#define _do_after()\
  do {\
    if (_ctx.has_after_jmp_buf)\
    {\
      _ctx.has_after_jmp_buf = false;\
      if (setjmp(_ctx.return_jmp_buf) == 0)\
      {\
        longjmp(_ctx.after_jmp_buf, 1);\
      }\
    }\
  } while (0)\

#define _do_before_each()\
  do {\
    if (_ctx.parent->has_before_each_jmp_buf)\
    {\
      if (setjmp(_ctx.parent->return_jmp_buf) == 0)\
      {\
        longjmp(_ctx.parent->before_each_jmp_buf, 1);\
      }\
    }\
  } while (0)\

#define _do_after_each()\
  do {\
    if (_ctx.parent->has_after_each_jmp_buf)\
    {\
      if (setjmp(_ctx.parent->return_jmp_buf) == 0)\
      {\
        longjmp(_ctx.parent->after_each_jmp_buf, 1);\
      }\
    }\
  } while (0)\

#define end()\
    switch (_ctx.kind)\
    {\
    case TEST_CONTEXT_TEST:\
    {\
      _do_after();\
      return _ctx.result;\
    }\
    case TEST_CONTEXT_BEFORE:\
    {\
      longjmp(_ctx.parent->return_jmp_buf, 1);\
    }\
    case TEST_CONTEXT_AFTER:\
    {\
      longjmp(_ctx.parent->return_jmp_buf, 1);\
    }\
    case TEST_CONTEXT_BEFORE_EACH:\
    {\
      longjmp(_ctx.parent->return_jmp_buf, 1);\
    }\
    case TEST_CONTEXT_AFTER_EACH:\
    {\
      longjmp(_ctx.parent->return_jmp_buf, 1);\
    }\
    case TEST_CONTEXT_DESCRIBE:\
    {\
      _do_after_each();\
      _do_after();\
      if (_ctx.result == EXIT_FAILURE)\
      {\
        _ctx.parent->result = EXIT_FAILURE;\
      }\
      longjmp(_ctx.exit_jmp_buf, 1);\
    }\
    case TEST_CONTEXT_IT:\
    {\
      _do_after_each();\
      _do_after();\
      printf(ESC_FG_BRIGHT_GREEN " passed\n" ESC_RESET);\
      longjmp(_ctx.exit_jmp_buf, 1);\
    }\
    }\
  }

#define test()\
  int main(void)\
  {\
    test_context_t _ctx = {\
      .kind = TEST_CONTEXT_TEST,\
      .parent = NULL,\
      .depth = 0,\
      .result = EXIT_SUCCESS,\
      .has_before_jmp_buf = false,\
      .has_after_jmp_buf = false,\
      .has_before_each_jmp_buf = false,\
      .has_after_each_jmp_buf = false,\
    };\
    if (setjmp(_ctx.exit_jmp_buf) != 0)\
    {\
      return _ctx.result;\
    }\

#define before()\
  for (;;)\
  {\
    test_context_t* _parent = &_ctx;\
    test_context_t _ctx = {\
      .kind = TEST_CONTEXT_BEFORE,\
      .parent = _parent,\
    };\
    if (setjmp(_ctx.parent->before_jmp_buf) == 0)\
    {\
      _ctx.parent->has_before_jmp_buf = true;\
      break;\
    }\

#define after()\
  for (;;)\
  {\
    test_context_t* _parent = &_ctx;\
    test_context_t _ctx = {\
      .kind = TEST_CONTEXT_AFTER,\
      .parent = _parent,\
    };\
    if (setjmp(_ctx.parent->after_jmp_buf) == 0)\
    {\
      _ctx.parent->has_after_jmp_buf = true;\
      break;\
    }\

#define before_each()\
  for (;;)\
  {\
    test_context_t* _parent = &_ctx;\
    test_context_t _ctx = {\
      .kind = TEST_CONTEXT_BEFORE_EACH,\
      .parent = _parent,\
    };\
    if (setjmp(_ctx.parent->before_each_jmp_buf) == 0)\
    {\
      _ctx.parent->has_before_each_jmp_buf = true;\
      break;\
    }\

#define after_each()\
  for (;;)\
  {\
    test_context_t* _parent = &_ctx;\
    test_context_t _ctx = {\
      .kind = TEST_CONTEXT_AFTER_EACH,\
      .parent = _parent,\
    };\
    if (setjmp(_ctx.parent->after_each_jmp_buf) == 0)\
    {\
      _ctx.parent->has_after_each_jmp_buf = true;\
      break;\
    }\

#define describe(WHAT)\
  for (;;)\
  {\
    test_context_t* _parent = &_ctx;\
    test_context_t _ctx = {\
      .kind = TEST_CONTEXT_DESCRIBE,\
      .parent = _parent,\
      .depth = _parent->depth + 1,\
      .result = EXIT_SUCCESS,\
      .has_before_each_jmp_buf = false,\
      .has_after_each_jmp_buf = false,\
    };\
    if (setjmp(_ctx.exit_jmp_buf) != 0)\
    {\
      break;\
    }\
    _do_before();\
    _do_before_each();\
    _print_indent(_ctx.depth);\
    printf("%s\n", (WHAT));\

#define it(SHOULD)\
  for (;;)\
  {\
    test_context_t* _parent = &_ctx;\
    test_context_t _ctx = {\
      .kind = TEST_CONTEXT_IT,\
      .parent = _parent,\
      .depth = _parent->depth + 1,\
      .result = EXIT_SUCCESS,\
    };\
    if (setjmp(_ctx.exit_jmp_buf) != 0)\
    {\
      break;\
    }\
    _do_before();\
    _do_before_each();\
    _print_indent(_ctx.depth);\
    printf(ESC_FG_BRIGHT_BLACK "%s" ESC_RESET, (SHOULD));\

#define _assert_impl(COND, STR)\
  do {\
    if (!(COND))\
    {\
      printf(ESC_FG_BRIGHT_RED " failed\n");\
      _print_indent(_ctx.depth + 1);\
      printf("%s\n" ESC_RESET, (STR));\
      _ctx.parent->result = EXIT_FAILURE;\
      longjmp(_ctx.exit_jmp_buf, 1);\
    }\
  } while (0)\

#define assert_true(COND)\
  _assert_impl((bool)(COND), "assert_true(" #COND ")")

#define assert_false(COND)\
  _assert_impl(!(bool)(COND), "assert_false(" #COND ")")

#define assert_null(VALUE)\
  _assert_impl((void*)(VALUE) == NULL, "assert_null(" #VALUE ")")

#define assert_not_null(VALUE)\
  _assert_impl((void*)(VALUE) != NULL, "assert_not_null(" #VALUE ")")

#define assert_equal(ACTUAL, EXPECTED)\
  _assert_impl((ACTUAL) == (EXPECTED), "assert_equal(" #ACTUAL ", " #EXPECTED ")")

#define assert_not_equal(ACTUAL, EXPECTED)\
  _assert_impl((ACTUAL) != (EXPECTED), "assert_not_equal(" #ACTUAL ", " #EXPECTED ")")

#define assert_ptr_equal(ACTUAL, EXPECTED)\
  _assert_impl((void*)(ACTUAL) == (void*)(EXPECTED), "assert_ptr_equal(" #ACTUAL ", " #EXPECTED ")")

#define assert_ptr_not_equal(ACTUAL, EXPECTED)\
  _assert_impl((void*)(ACTUAL) != (void*)(EXPECTED), "assert_ptr_not_equal(" #ACTUAL ", " #EXPECTED ")")

#define assert_str_equal(ACTUAL, EXPECTED)\
  _assert_impl(strcmp((ACTUAL), (EXPECTED)) == 0, "assert_str_equal(" #ACTUAL ", " #EXPECTED ")")

#define assert_str_n_equal(ACTUAL, EXPECTED, MAX_LEN)\
  _assert_impl(strncmp((ACTUAL), (EXPECTED), (MAX_LEN)) == 0, "assert_str_n_equal(" #ACTUAL ", " #EXPECTED ", " #MAX_LEN ")")

#define assert_str_not_equal(ACTUAL, EXPECTED)\
  _assert_impl(strcmp((ACTUAL), (EXPECTED)) != 0, "assert_str_not_equal(" #ACTUAL ", " #EXPECTED ")")

#endif