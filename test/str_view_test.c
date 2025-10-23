#include "test.h"

#include "utils/str_view.h"

TEST_CASE(tau_string_view_init)
{
  tau_string_view_t str = tau_string_view_init("foo");

  TEST_ASSERT_TRUE(tau_string_view_compare_cstr(str, "foo") == 0);
}

TEST_CASE(tau_string_view_init_with_length)
{
  tau_string_view_t str = tau_string_view_init_with_length("foobar", 3);

  TEST_ASSERT_TRUE(tau_string_view_compare_cstr(str, "foo") == 0);
}

TEST_CASE(tau_string_view_substr_empty)
{
  tau_string_view_t str = tau_string_view_init("foo");
  tau_string_view_t sub = tau_string_view_substr(str, 0, 0);

  TEST_ASSERT_TRUE(tau_string_view_compare_cstr(sub, "") == 0);
}

TEST_CASE(tau_string_view_substr)
{
  tau_string_view_t str = tau_string_view_init("foobarfizz");
  tau_string_view_t sub = tau_string_view_substr(str, 3, 3);

  TEST_ASSERT_TRUE(tau_string_view_compare_cstr(sub, "bar") == 0);
}

TEST_CASE(tau_string_view_substr_all)
{
  tau_string_view_t str = tau_string_view_init("foobar");
  tau_string_view_t sub = tau_string_view_substr(str, 0, 6);

  TEST_ASSERT_TRUE(tau_string_view_compare_cstr(sub, "foobar") == 0);
}

TEST_CASE(tau_string_view_compare)
{
  tau_string_view_t str1 = tau_string_view_init("foo");
  tau_string_view_t str2 = tau_string_view_init("bar");

  TEST_ASSERT_TRUE(tau_string_view_compare(str2, str1) < 0);
  TEST_ASSERT_TRUE(tau_string_view_compare(str1, str2) > 0);
  TEST_ASSERT_TRUE(tau_string_view_compare(str1, str1) == 0);
  TEST_ASSERT_TRUE(tau_string_view_compare(str2, str2) == 0);
}

TEST_CASE(tau_string_view_compare_cstr)
{
  tau_string_view_t str = tau_string_view_init("foo");

  TEST_ASSERT_TRUE(tau_string_view_compare_cstr(str, "xyz") < 0);
  TEST_ASSERT_TRUE(tau_string_view_compare_cstr(str, "bar") > 0);
  TEST_ASSERT_TRUE(tau_string_view_compare_cstr(str, "foo") == 0);
}

TEST_CASE(tau_string_view_starts_with_empty)
{
  tau_string_view_t str = tau_string_view_init("foo");
  tau_string_view_t prefix = tau_string_view_init("");

  TEST_ASSERT_TRUE(tau_string_view_starts_with(str, prefix));
}

TEST_CASE(tau_string_view_starts_with)
{
  tau_string_view_t str = tau_string_view_init("foobar");
  tau_string_view_t prefix1 = tau_string_view_init("fizz");
  tau_string_view_t prefix2 = tau_string_view_init("foo");

  TEST_ASSERT_FALSE(tau_string_view_starts_with(str, prefix1));
  TEST_ASSERT_TRUE(tau_string_view_starts_with(str, prefix2));
}

TEST_CASE(tau_string_view_starts_with_cstr_empty)
{
  tau_string_view_t str = tau_string_view_init("foo");

  TEST_ASSERT_TRUE(tau_string_view_starts_with_cstr(str, ""));
}

TEST_CASE(tau_string_view_starts_with_cstr)
{
  tau_string_view_t str = tau_string_view_init("foobar");

  TEST_ASSERT_FALSE(tau_string_view_starts_with_cstr(str, "fizz"));
  TEST_ASSERT_TRUE(tau_string_view_starts_with_cstr(str, "foo"));
}

TEST_CASE(tau_string_view_ends_with_empty)
{
  tau_string_view_t str = tau_string_view_init("foo");
  tau_string_view_t suffix = tau_string_view_init("");

  TEST_ASSERT_TRUE(tau_string_view_ends_with(str, suffix));
}

TEST_CASE(tau_string_view_ends_with)
{
  tau_string_view_t str = tau_string_view_init("foobar");
  tau_string_view_t suffix1 = tau_string_view_init("fizz");
  tau_string_view_t suffix2 = tau_string_view_init("bar");

  TEST_ASSERT_FALSE(tau_string_view_ends_with(str, suffix1));
  TEST_ASSERT_TRUE(tau_string_view_ends_with(str, suffix2));
}

TEST_CASE(tau_string_view_ends_with_cstr_empty)
{
  tau_string_view_t str = tau_string_view_init("foo");

  TEST_ASSERT_TRUE(tau_string_view_ends_with_cstr(str, ""));
}

TEST_CASE(tau_string_view_ends_with_cstr)
{
  tau_string_view_t str = tau_string_view_init("foobar");

  TEST_ASSERT_FALSE(tau_string_view_ends_with_cstr(str, "fizz"));
  TEST_ASSERT_TRUE(tau_string_view_ends_with_cstr(str, "bar"));
}

TEST_CASE(tau_string_view_contains_empty)
{
  tau_string_view_t str = tau_string_view_init("foo");
  tau_string_view_t sub = tau_string_view_init("");

  TEST_ASSERT_TRUE(tau_string_view_contains(str, sub));
}

TEST_CASE(tau_string_view_contains)
{
  tau_string_view_t str = tau_string_view_init("foobar");
  tau_string_view_t sub1 = tau_string_view_init("fizz");
  tau_string_view_t sub2 = tau_string_view_init("foo");
  tau_string_view_t sub3 = tau_string_view_init("bar");
  tau_string_view_t sub4 = tau_string_view_init("oob");

  TEST_ASSERT_FALSE(tau_string_view_contains(str, sub1));
  TEST_ASSERT_TRUE(tau_string_view_contains(str, sub2));
  TEST_ASSERT_TRUE(tau_string_view_contains(str, sub3));
  TEST_ASSERT_TRUE(tau_string_view_contains(str, sub4));
}

TEST_CASE(tau_string_view_contains_cstr_empty)
{
  tau_string_view_t str = tau_string_view_init("foo");

  TEST_ASSERT_TRUE(tau_string_view_contains_cstr(str, ""));
}

TEST_CASE(tau_string_view_contains_cstr)
{
  tau_string_view_t str = tau_string_view_init("foobar");

  TEST_ASSERT_FALSE(tau_string_view_contains_cstr(str, "fizz"));
  TEST_ASSERT_TRUE(tau_string_view_contains_cstr(str, "foo"));
  TEST_ASSERT_TRUE(tau_string_view_contains_cstr(str, "bar"));
  TEST_ASSERT_TRUE(tau_string_view_contains_cstr(str, "oob"));
}

TEST_CASE(tau_string_view_find)
{
  tau_string_view_t str = tau_string_view_init("foobar");
  tau_string_view_t sub1 = tau_string_view_init("fizz");
  tau_string_view_t sub2 = tau_string_view_init("foo");
  tau_string_view_t sub3 = tau_string_view_init("bar");
  tau_string_view_t sub4 = tau_string_view_init("oob");

  TEST_ASSERT_EQUAL(tau_string_view_find(str, sub1), 6);
  TEST_ASSERT_EQUAL(tau_string_view_find(str, sub2), 0);
  TEST_ASSERT_EQUAL(tau_string_view_find(str, sub3), 3);
  TEST_ASSERT_EQUAL(tau_string_view_find(str, sub4), 1);
}

TEST_CASE(tau_string_view_find_cstr)
{
  tau_string_view_t str = tau_string_view_init("foobar");

  TEST_ASSERT_EQUAL(tau_string_view_find_cstr(str, "fizz"), 6);
  TEST_ASSERT_EQUAL(tau_string_view_find_cstr(str, "foo"), 0);
  TEST_ASSERT_EQUAL(tau_string_view_find_cstr(str, "bar"), 3);
  TEST_ASSERT_EQUAL(tau_string_view_find_cstr(str, "oob"), 1);
}

TEST_MAIN()
{
  TEST_RUN(tau_string_view_init);
  TEST_RUN(tau_string_view_init_with_length);
  TEST_RUN(tau_string_view_substr_empty);
  TEST_RUN(tau_string_view_substr);
  TEST_RUN(tau_string_view_substr_all);
  TEST_RUN(tau_string_view_compare);
  TEST_RUN(tau_string_view_compare_cstr);
  TEST_RUN(tau_string_view_starts_with_empty);
  TEST_RUN(tau_string_view_starts_with);
  TEST_RUN(tau_string_view_starts_with_cstr_empty);
  TEST_RUN(tau_string_view_starts_with_cstr);
  TEST_RUN(tau_string_view_ends_with_empty);
  TEST_RUN(tau_string_view_ends_with);
  TEST_RUN(tau_string_view_ends_with_cstr_empty);
  TEST_RUN(tau_string_view_ends_with_cstr);
  TEST_RUN(tau_string_view_contains_empty);
  TEST_RUN(tau_string_view_contains);
  TEST_RUN(tau_string_view_contains_cstr_empty);
  TEST_RUN(tau_string_view_contains_cstr);
  TEST_RUN(tau_string_view_find);
  TEST_RUN(tau_string_view_find_cstr);
}
