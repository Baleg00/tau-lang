/**
 * \file
 *
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_TEST_H
#define TAU_TEST_H

#ifndef TAU_TEST
# error "TAU_TEST must be defined when using `test.h`!"
#endif

#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils/esc_seq.h"

#define TEST_MAIN()\
  void test_main_impl(void);\
  int main(void) {\
    g_test_main_result = EXIT_SUCCESS;\
    test_main_impl();\
    return g_test_main_result;\
  }\
  void test_main_impl(void)

#define TEST_CASE(TEST_NAME)\
  void test_case_##TEST_NAME##_impl(void);\
  void test_case_##TEST_NAME(void)\
  {\
    printf(TAU_ESC_FG_BRIGHT_BLACK "[" #TEST_NAME "]" TAU_ESC_RESET "\n");\
    g_test_case_status = TEST_STATUS_PASSED;\
    test_case_##TEST_NAME##_impl();\
    switch (g_test_case_status) {\
    case TEST_STATUS_PASSED: printf(TAU_ESC_FG_GREEN "PASSED" TAU_ESC_RESET "\n"); break;\
    case TEST_STATUS_IGNORED: printf(TAU_ESC_FG_YELLOW "IGNORED" TAU_ESC_RESET "\n"); break;\
    case TEST_STATUS_FAILED: fprintf(stderr, TAU_ESC_FG_RED "FAILED" TAU_ESC_RESET "\n"); break;\
    default: break;\
    }\
  }\
  void test_case_##TEST_NAME##_impl(void)

#define TEST_RUN(TEST_NAME)\
  do {\
    test_case_##TEST_NAME();\
    if (g_test_case_status == TEST_STATUS_FAILED)\
      g_test_main_result = EXIT_FAILURE;\
  } while (0)

#define TEST_PASS()\
  do {\
    g_test_case_status = TEST_STATUS_PASSED;\
    return;\
  } while (0)

#define TEST_PASS_REASON(MSG)\
  do {\
    printf(TAU_ESC_FG_GREEN "Pass: " MSG TAU_ESC_RESET "\n");\
    g_test_case_status = TEST_STATUS_PASSED;\
    return;\
  } while (0)

#define TEST_IGNORE()\
  do {\
    g_test_case_status = TEST_STATUS_IGNORED;\
    return;\
  } while (0)

#define TEST_IGNORE_REASON(MSG)\
  do {\
    printf(TAU_ESC_FG_YELLOW "Ignore: " MSG TAU_ESC_RESET "\n");\
    g_test_case_status = TEST_STATUS_IGNORED;\
    return;\
  } while (0)

#define TEST_FAIL()\
  do {\
    g_test_case_status = TEST_STATUS_FAILED;\
    return;\
  } while (0)

#define TEST_FAIL_REASON(MSG)\
  do {\
    fprintf(stderr, TAU_ESC_FG_RED "Fail: " MSG TAU_ESC_RESET "\n");\
    g_test_case_status = TEST_STATUS_FAILED;\
    return;\
  } while (0)

#define TEST_LOG(MSG, ...)\
  do {\
    printf(TAU_ESC_FG_BRIGHT_BLACK MSG TAU_ESC_RESET "\n", ##__VA_ARGS__);\
  } while (0)

#define TEST_ASSERT(COND)\
  test_assert((COND), __FILE__, __LINE__, "TEST_ASSERT(" #COND ")")

#define TEST_ASSERT_TRUE(COND)\
  test_assert((COND), __FILE__, __LINE__, "TEST_ASSERT_TRUE(" #COND ")")

#define TEST_ASSERT_FALSE(COND)\
  test_assert(!(COND), __FILE__, __LINE__, "TEST_ASSERT_FALSE(" #COND ")")

#define TEST_ASSERT_NULL(PTR)\
  test_assert((const void*)(PTR) == NULL, __FILE__, __LINE__, "TEST_ASSERT_NULL(" #PTR ")")

#define TEST_ASSERT_NOT_NULL(PTR)\
  test_assert((const void*)(PTR) != NULL, __FILE__, __LINE__, "TEST_ASSERT_NOT_NULL(" #PTR ")")

#define TEST_ASSERT_EQUAL(ACTUAL, EXPECTED)\
  test_assert((ACTUAL) == (EXPECTED), __FILE__, __LINE__, "TEST_ASSERT_EQUAL(" #ACTUAL ", " #EXPECTED ")")

#define TEST_ASSERT_NOT_EQUAL(ACTUAL, NOT_EXPECTED)\
  test_assert((ACTUAL) != (NOT_EXPECTED), __FILE__, __LINE__, "TEST_ASSERT_NOT_EQUAL(" #ACTUAL ", " #NOT_EXPECTED ")")

#define TEST_ASSERT_PTR_EQUAL(ACTUAL, EXPECTED)\
  test_assert((void*)(ACTUAL) == (void*)(EXPECTED), __FILE__, __LINE__, "TEST_ASSERT_PTR_EQUAL(" #ACTUAL ", " #EXPECTED ")")

#define TEST_ASSERT_PTR_NOT_EQUAL(ACTUAL, NOT_EXPECTED)\
  test_assert((void*)(ACTUAL) != (void*)(NOT_EXPECTED), __FILE__, __LINE__, "TEST_ASSERT_PTR_NOT_EQUAL(" #ACTUAL ", " #NOT_EXPECTED ")")

#define TEST_ASSERT_STR_EQUAL(ACTUAL, EXPECTED)\
  test_assert(strcmp((const char*)(ACTUAL), (const char*)(EXPECTED)) == 0, __FILE__, __LINE__, "TEST_ASSERT_STR_EQUAL(" #ACTUAL ", " #EXPECTED ")")

#define TEST_ASSERT_STR_NOT_EQUAL(ACTUAL, NOT_EXPECTED)\
  test_assert(strcmp((const char*)(ACTUAL), (const char*)(NOT_EXPECTED)) != 0, __FILE__, __LINE__, "TEST_ASSERT_STR_NOT_EQUAL(" #ACTUAL ", " #NOT_EXPECTED ")")

#define TEST_ASSERT_MEM_EQUAL(ACTUAL, EXPECTED, SIZE)\
  test_assert(memcmp((const void*)(ACTUAL), (const void*)(EXPECTED), (SIZE)) == 0, __FILE__, __LINE__, "TEST_ASSERT_MEM_EQUAL(" #ACTUAL ", " #EXPECTED, ", " #SIZE ")")

#define TEST_ASSERT_MEM_NOT_EQUAL(ACTUAL, NOT_EXPECTED, SIZE)\
  test_assert(memcmp((const void*)(ACTUAL), (const void*)(NOT_EXPECTED), (SIZE)) != 0, __FILE__, __LINE__, "TEST_ASSERT_MEM_NOT_EQUAL(" #ACTUAL ", " #NOT_EXPECTED, ", " #SIZE ")")

#define test_assert(COND, FILE, LINE, MSG)\
  do {\
    if (!(COND)) {\
      fprintf(stderr, TAU_ESC_FG_BRIGHT_BLACK "(%s:%d) " TAU_ESC_FG_RED "Assertion failed: %s" TAU_ESC_RESET "\n", FILE, LINE, MSG);\
      g_test_case_status = TEST_STATUS_FAILED;\
      return;\
    }\
  } while (0)

typedef enum test_status_t
{
  TEST_STATUS_PASSED,
  TEST_STATUS_IGNORED,
  TEST_STATUS_FAILED,
} test_status_t;

static test_status_t g_test_case_status = TEST_STATUS_PASSED;
static int g_test_main_result = 0;

#endif
