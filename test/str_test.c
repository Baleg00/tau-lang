#include "test.h"

#include "utils/str.h"

TEST_CASE(tau_string_init)
{
  tau_string_t* str = tau_string_init();

  TEST_ASSERT_NOT_NULL(str);
  TEST_ASSERT_EQUAL(tau_string_length(str), 0);

  tau_string_free(str);
}

TEST_CASE(tau_string_init_with_cstr)
{
  tau_string_t* str = tau_string_init_with_cstr("foo");

  TEST_ASSERT_NOT_NULL(str);
  TEST_ASSERT_EQUAL(tau_string_length(str), 3);
  TEST_ASSERT_STR_EQUAL(tau_string_begin(str), "foo");

  tau_string_free(str);
}

TEST_CASE(tau_string_append_empty)
{
  tau_string_t* str1 = tau_string_init();
  tau_string_t* str2 = tau_string_init_with_cstr("");

  tau_string_append(str1, str2);

  TEST_ASSERT_EQUAL(tau_string_length(str1), 0);
  TEST_ASSERT_STR_EQUAL(tau_string_begin(str1), "");

  TEST_ASSERT_EQUAL(tau_string_length(str2), 0);
  TEST_ASSERT_STR_EQUAL(tau_string_begin(str2), "");

  tau_string_free(str2);
  tau_string_free(str1);
}

TEST_CASE(tau_string_append)
{
  tau_string_t* str1 = tau_string_init_with_cstr("foo");
  tau_string_t* str2 = tau_string_init_with_cstr("bar");

  tau_string_append(str1, str2);

  TEST_ASSERT_EQUAL(tau_string_length(str1), 6);
  TEST_ASSERT_STR_EQUAL(tau_string_begin(str1), "foobar");

  TEST_ASSERT_EQUAL(tau_string_length(str2), 3);
  TEST_ASSERT_STR_EQUAL(tau_string_begin(str2), "bar");

  tau_string_free(str2);
  tau_string_free(str1);
}

TEST_CASE(tau_string_append_cstr_empty)
{
  tau_string_t* str = tau_string_init();

  tau_string_append_cstr(str, "");

  TEST_ASSERT_EQUAL(tau_string_length(str), 0);
  TEST_ASSERT_STR_EQUAL(tau_string_begin(str), "");

  tau_string_free(str);
}

TEST_CASE(tau_string_append_cstr)
{
  tau_string_t* str = tau_string_init_with_cstr("foo");

  tau_string_append_cstr(str, "bar");

  TEST_ASSERT_EQUAL(tau_string_length(str), 6);
  TEST_ASSERT_STR_EQUAL(tau_string_begin(str), "foobar");

  tau_string_free(str);
}

TEST_CASE(tau_string_insert_empty)
{
  tau_string_t* str1 = tau_string_init_with_cstr("foo");
  tau_string_t* str2 = tau_string_init_with_cstr("");

  tau_string_insert(str1, 1, str2);

  TEST_ASSERT_TRUE(tau_string_compare_cstr(str1, "foo") == 0);

  tau_string_free(str2);
  tau_string_free(str1);
}

TEST_CASE(tau_string_insert_begin)
{
  tau_string_t* str1 = tau_string_init_with_cstr("foo");
  tau_string_t* str2 = tau_string_init_with_cstr("bar");

  tau_string_insert(str1, 0, str2);

  TEST_ASSERT_TRUE(tau_string_compare_cstr(str1, "barfoo") == 0);

  tau_string_free(str2);
  tau_string_free(str1);
}

TEST_CASE(tau_string_insert_end)
{
  tau_string_t* str1 = tau_string_init_with_cstr("foo");
  tau_string_t* str2 = tau_string_init_with_cstr("bar");

  tau_string_insert(str1, 3, str2);

  TEST_ASSERT_TRUE(tau_string_compare_cstr(str1, "foobar") == 0);

  tau_string_free(str2);
  tau_string_free(str1);
}

TEST_CASE(tau_string_insert)
{
  tau_string_t* str1 = tau_string_init_with_cstr("foo");
  tau_string_t* str2 = tau_string_init_with_cstr("bar");

  tau_string_insert(str1, 2, str2);

  TEST_ASSERT_TRUE(tau_string_compare_cstr(str1, "fobaro") == 0);

  tau_string_free(str2);
  tau_string_free(str1);
}

TEST_CASE(tau_string_insert_cstr_empty)
{
  tau_string_t* str = tau_string_init_with_cstr("foo");

  tau_string_insert_cstr(str, 1, "");

  TEST_ASSERT_TRUE(tau_string_compare_cstr(str, "foo") == 0);

  tau_string_free(str);
}

TEST_CASE(tau_string_insert_cstr_begin)
{
  tau_string_t* str = tau_string_init_with_cstr("foo");

  tau_string_insert_cstr(str, 0, "bar");

  TEST_ASSERT_TRUE(tau_string_compare_cstr(str, "barfoo") == 0);

  tau_string_free(str);
}

TEST_CASE(tau_string_insert_cstr_end)
{
  tau_string_t* str = tau_string_init_with_cstr("foo");

  tau_string_insert_cstr(str, 3, "bar");

  TEST_ASSERT_TRUE(tau_string_compare_cstr(str, "foobar") == 0);

  tau_string_free(str);
}

TEST_CASE(tau_string_insert_cstr)
{
  tau_string_t* str = tau_string_init_with_cstr("foo");

  tau_string_insert_cstr(str, 2, "bar");

  TEST_ASSERT_TRUE(tau_string_compare_cstr(str, "fobaro") == 0);

  tau_string_free(str);
}

TEST_CASE(tau_string_erase_empty)
{
  tau_string_t* str = tau_string_init_with_cstr("foo");

  tau_string_erase(str, 0, 0);

  TEST_ASSERT_TRUE(tau_string_compare_cstr(str, "foo") == 0);

  tau_string_free(str);
}

TEST_CASE(tau_string_erase_begin)
{
  tau_string_t* str = tau_string_init_with_cstr("foobar");

  tau_string_erase(str, 0, 3);

  TEST_ASSERT_TRUE(tau_string_compare_cstr(str, "bar") == 0);

  tau_string_free(str);
}

TEST_CASE(tau_string_erase_end)
{
  tau_string_t* str = tau_string_init_with_cstr("foobar");

  tau_string_erase(str, 3, 3);

  TEST_ASSERT_TRUE(tau_string_compare_cstr(str, "foo") == 0);

  tau_string_free(str);
}

TEST_CASE(tau_string_erase)
{
  tau_string_t* str = tau_string_init_with_cstr("foobar");

  tau_string_erase(str, 2, 2);

  TEST_ASSERT_TRUE(tau_string_compare_cstr(str, "foar") == 0);

  tau_string_free(str);
}

TEST_CASE(tau_string_clear)
{
  tau_string_t* str = tau_string_init_with_cstr("foo");

  tau_string_clear(str);

  TEST_ASSERT_TRUE(tau_string_compare_cstr(str, "") == 0);

  tau_string_free(str);
}

TEST_CASE(tau_string_copy)
{
  tau_string_t* str = tau_string_init_with_cstr("foo");
  tau_string_t* copy = tau_string_copy(str);

  TEST_ASSERT_TRUE(tau_string_compare(str, copy) == 0);

  tau_string_free(copy);
  tau_string_free(str);
}

TEST_CASE(tau_string_substr_empty)
{
  tau_string_t* str1 = tau_string_init_with_cstr("foo");
  tau_string_t* str2 = tau_string_substr(str1, 0, 0);

  TEST_ASSERT_EQUAL(tau_string_length(str1), 3);
  TEST_ASSERT_STR_EQUAL(tau_string_begin(str1), "foo");

  TEST_ASSERT_EQUAL(tau_string_length(str2), 0);
  TEST_ASSERT_STR_EQUAL(tau_string_begin(str2), "");

  tau_string_free(str2);
  tau_string_free(str1);
}

TEST_CASE(tau_string_substr)
{
  tau_string_t* str1 = tau_string_init_with_cstr("foobarfizz");
  tau_string_t* str2 = tau_string_substr(str1, 3, 3);

  TEST_ASSERT_EQUAL(tau_string_length(str1), 10);
  TEST_ASSERT_STR_EQUAL(tau_string_begin(str1), "foobarfizz");

  TEST_ASSERT_EQUAL(tau_string_length(str2), 3);
  TEST_ASSERT_STR_EQUAL(tau_string_begin(str2), "bar");

  tau_string_free(str2);
  tau_string_free(str1);
}

TEST_CASE(tau_string_substr_all)
{
  tau_string_t* str1 = tau_string_init_with_cstr("foobar");
  tau_string_t* str2 = tau_string_substr(str1, 0, 6);

  TEST_ASSERT_EQUAL(tau_string_length(str1), 6);
  TEST_ASSERT_STR_EQUAL(tau_string_begin(str1), "foobar");

  TEST_ASSERT_EQUAL(tau_string_length(str2), 6);
  TEST_ASSERT_STR_EQUAL(tau_string_begin(str2), "foobar");

  tau_string_free(str2);
  tau_string_free(str1);
}

TEST_CASE(tau_string_compare)
{
  tau_string_t* str1 = tau_string_init_with_cstr("foo");
  tau_string_t* str2 = tau_string_init_with_cstr("bar");

  TEST_ASSERT_TRUE(tau_string_compare(str2, str1) < 0);
  TEST_ASSERT_TRUE(tau_string_compare(str1, str2) > 0);
  TEST_ASSERT_TRUE(tau_string_compare(str1, str1) == 0);
  TEST_ASSERT_TRUE(tau_string_compare(str2, str2) == 0);

  tau_string_free(str2);
  tau_string_free(str1);
}

TEST_CASE(tau_string_compare_cstr)
{
  tau_string_t* str = tau_string_init_with_cstr("foo");

  TEST_ASSERT_TRUE(tau_string_compare_cstr(str, "xyz") < 0);
  TEST_ASSERT_TRUE(tau_string_compare_cstr(str, "bar") > 0);
  TEST_ASSERT_TRUE(tau_string_compare_cstr(str, "foo") == 0);

  tau_string_free(str);
}

TEST_CASE(tau_string_escape)
{
  tau_string_t* str = tau_string_init_with_cstr("a'b\"c\\d\ae\bf\fg\nh\ri\tj\vk");
  tau_string_t* expected = tau_string_init_with_cstr("a\\'b\\\"c\\\\d\\ae\\bf\\fg\\nh\\ri\\tj\\vk");
  tau_string_t* escaped = tau_string_escape(str);

  TEST_ASSERT_TRUE(tau_string_compare(escaped, expected) == 0);

  tau_string_free(escaped);
  tau_string_free(expected);
  tau_string_free(str);
}

TEST_CASE(tau_string_starts_with_empty)
{
  tau_string_t* str = tau_string_init_with_cstr("foo");
  tau_string_t* prefix = tau_string_init();

  TEST_ASSERT_TRUE(tau_string_starts_with(str, prefix));

  tau_string_free(prefix);
  tau_string_free(str);
}

TEST_CASE(tau_string_starts_with)
{
  tau_string_t* str = tau_string_init_with_cstr("foobar");
  tau_string_t* prefix1 = tau_string_init_with_cstr("fizz");
  tau_string_t* prefix2 = tau_string_init_with_cstr("foo");

  TEST_ASSERT_FALSE(tau_string_starts_with(str, prefix1));
  TEST_ASSERT_TRUE(tau_string_starts_with(str, prefix2));

  tau_string_free(prefix2);
  tau_string_free(prefix1);
  tau_string_free(str);
}

TEST_CASE(tau_string_starts_with_cstr_empty)
{
  tau_string_t* str = tau_string_init_with_cstr("foo");

  TEST_ASSERT_TRUE(tau_string_starts_with_cstr(str, ""));

  tau_string_free(str);
}

TEST_CASE(tau_string_starts_with_cstr)
{
  tau_string_t* str = tau_string_init_with_cstr("foobar");

  TEST_ASSERT_FALSE(tau_string_starts_with_cstr(str, "fizz"));
  TEST_ASSERT_TRUE(tau_string_starts_with_cstr(str, "foo"));

  tau_string_free(str);
}

TEST_CASE(tau_string_ends_with_empty)
{
  tau_string_t* str = tau_string_init_with_cstr("foo");
  tau_string_t* suffix = tau_string_init();

  TEST_ASSERT_TRUE(tau_string_ends_with(str, suffix));

  tau_string_free(suffix);
  tau_string_free(str);
}

TEST_CASE(tau_string_ends_with)
{
  tau_string_t* str = tau_string_init_with_cstr("foobar");
  tau_string_t* suffix1 = tau_string_init_with_cstr("fizz");
  tau_string_t* suffix2 = tau_string_init_with_cstr("bar");

  TEST_ASSERT_FALSE(tau_string_ends_with(str, suffix1));
  TEST_ASSERT_TRUE(tau_string_ends_with(str, suffix2));

  tau_string_free(suffix2);
  tau_string_free(suffix1);
  tau_string_free(str);
}

TEST_CASE(tau_string_ends_with_cstr_empty)
{
  tau_string_t* str = tau_string_init_with_cstr("foo");

  TEST_ASSERT_TRUE(tau_string_ends_with_cstr(str, ""));

  tau_string_free(str);
}

TEST_CASE(tau_string_ends_with_cstr)
{
  tau_string_t* str = tau_string_init_with_cstr("foobar");

  TEST_ASSERT_FALSE(tau_string_ends_with_cstr(str, "fizz"));
  TEST_ASSERT_TRUE(tau_string_ends_with_cstr(str, "bar"));

  tau_string_free(str);

}

TEST_CASE(tau_string_contains_empty)
{
  tau_string_t* str = tau_string_init_with_cstr("foo");
  tau_string_t* sub = tau_string_init();

  TEST_ASSERT_TRUE(tau_string_contains(str, sub));

  tau_string_free(sub);
  tau_string_free(str);
}

TEST_CASE(tau_string_contains)
{
  tau_string_t* str = tau_string_init_with_cstr("foobar");
  tau_string_t* sub1 = tau_string_init_with_cstr("fizz");
  tau_string_t* sub2 = tau_string_init_with_cstr("foo");
  tau_string_t* sub3 = tau_string_init_with_cstr("bar");
  tau_string_t* sub4 = tau_string_init_with_cstr("oob");

  TEST_ASSERT_FALSE(tau_string_contains(str, sub1));
  TEST_ASSERT_TRUE(tau_string_contains(str, sub2));
  TEST_ASSERT_TRUE(tau_string_contains(str, sub3));
  TEST_ASSERT_TRUE(tau_string_contains(str, sub4));

  tau_string_free(sub4);
  tau_string_free(sub3);
  tau_string_free(sub2);
  tau_string_free(sub1);
  tau_string_free(str);
}

TEST_CASE(tau_string_contains_cstr_empty)
{
  tau_string_t* str = tau_string_init_with_cstr("foo");

  TEST_ASSERT_TRUE(tau_string_contains_cstr(str, ""));

  tau_string_free(str);
}

TEST_CASE(tau_string_contains_cstr)
{
  tau_string_t* str = tau_string_init_with_cstr("foobar");

  TEST_ASSERT_FALSE(tau_string_contains_cstr(str, "fizz"));
  TEST_ASSERT_TRUE(tau_string_contains_cstr(str, "foo"));
  TEST_ASSERT_TRUE(tau_string_contains_cstr(str, "bar"));
  TEST_ASSERT_TRUE(tau_string_contains_cstr(str, "oob"));

  tau_string_free(str);
}

TEST_CASE(tau_string_replace_all)
{
  tau_string_t* str = tau_string_init_with_cstr("foo");
  tau_string_t* rep = tau_string_init_with_cstr("bar");

  tau_string_replace(str, 0, 3, rep);

  TEST_ASSERT_TRUE(tau_string_compare(str, rep) == 0);

  tau_string_free(rep);
  tau_string_free(str);
}

TEST_CASE(tau_string_replace_zero_len)
{
  tau_string_t* str = tau_string_init_with_cstr("foo");
  tau_string_t* rep = tau_string_init_with_cstr("bar");

  tau_string_replace(str, 0, 0, rep);

  TEST_ASSERT_TRUE(tau_string_compare_cstr(str, "barfoo") == 0);

  tau_string_replace(str, 6, 0, rep);

  TEST_ASSERT_TRUE(tau_string_compare_cstr(str, "barfoobar") == 0);

  tau_string_replace(str, 3, 0, rep);

  TEST_ASSERT_TRUE(tau_string_compare_cstr(str, "barbarfoobar") == 0);

  tau_string_free(rep);
  tau_string_free(str);
}

TEST_CASE(tau_string_replace_empty)
{
  tau_string_t* str = tau_string_init_with_cstr("foobar");
  tau_string_t* rep = tau_string_init_with_cstr("");

  tau_string_replace(str, 2, 2, rep);

  TEST_ASSERT_TRUE(tau_string_compare_cstr(str, "foar") == 0);

  tau_string_free(rep);
  tau_string_free(str);
}

TEST_CASE(tau_string_replace)
{
  tau_string_t* str = tau_string_init_with_cstr("foobar");
  tau_string_t* rep = tau_string_init_with_cstr("fizz");

  tau_string_replace(str, 0, 4, rep);

  TEST_ASSERT_TRUE(tau_string_compare_cstr(str, "fizzar") == 0);

  tau_string_replace(str, 2, 4, rep);

  TEST_ASSERT_TRUE(tau_string_compare_cstr(str, "fifizz") == 0);

  tau_string_replace(str, 3, 2, rep);

  TEST_ASSERT_TRUE(tau_string_compare_cstr(str, "fiffizzz") == 0);

  tau_string_free(rep);
  tau_string_free(str);
}

TEST_CASE(tau_string_replace_with_substr_all)
{
  tau_string_t* str = tau_string_init_with_cstr("foo");
  tau_string_t* rep = tau_string_init_with_cstr("fizzbar");

  tau_string_replace_with_substr(str, 0, 3, rep, 4);

  TEST_ASSERT_TRUE(tau_string_compare_cstr(str, "bar") == 0);

  tau_string_free(rep);
  tau_string_free(str);
}

TEST_CASE(tau_string_replace_with_substr_zero_len)
{
  tau_string_t* str = tau_string_init_with_cstr("foo");
  tau_string_t* rep = tau_string_init_with_cstr("bar");

  tau_string_replace_with_substr(str, 0, 0, rep, 1);

  TEST_ASSERT_TRUE(tau_string_compare_cstr(str, "arfoo") == 0);

  tau_string_replace_with_substr(str, 5, 0, rep, 1);

  TEST_ASSERT_TRUE(tau_string_compare_cstr(str, "arfooar") == 0);

  tau_string_replace_with_substr(str, 4, 0, rep, 1);

  TEST_ASSERT_TRUE(tau_string_compare_cstr(str, "arfoaroar") == 0);

  tau_string_free(rep);
  tau_string_free(str);
}

TEST_CASE(tau_string_replace_with_substr)
{
  tau_string_t* str = tau_string_init_with_cstr("foobar");
  tau_string_t* rep = tau_string_init_with_cstr("fizz");

  tau_string_replace_with_substr(str, 0, 3, rep, 2);

  TEST_ASSERT_TRUE(tau_string_compare_cstr(str, "zzbar") == 0);

  tau_string_replace_with_substr(str, 4, 1, rep, 1);

  TEST_ASSERT_TRUE(tau_string_compare_cstr(str, "zzbaizz") == 0);

  tau_string_replace_with_substr(str, 3, 2, rep, 1);

  TEST_ASSERT_TRUE(tau_string_compare_cstr(str, "zzbizzzz") == 0);

  tau_string_free(rep);
  tau_string_free(str);
}

TEST_CASE(tau_string_find)
{
  tau_string_t* str = tau_string_init_with_cstr("foobar");
  tau_string_t* sub1 = tau_string_init_with_cstr("fizz");
  tau_string_t* sub2 = tau_string_init_with_cstr("foo");
  tau_string_t* sub3 = tau_string_init_with_cstr("bar");
  tau_string_t* sub4 = tau_string_init_with_cstr("oob");

  TEST_ASSERT_EQUAL(tau_string_find(str, sub1), 6);
  TEST_ASSERT_EQUAL(tau_string_find(str, sub2), 0);
  TEST_ASSERT_EQUAL(tau_string_find(str, sub3), 3);
  TEST_ASSERT_EQUAL(tau_string_find(str, sub4), 1);

  tau_string_free(sub4);
  tau_string_free(sub3);
  tau_string_free(sub2);
  tau_string_free(sub1);
  tau_string_free(str);
}

TEST_CASE(tau_string_find_cstr)
{
  tau_string_t* str = tau_string_init_with_cstr("foobar");

  TEST_ASSERT_EQUAL(tau_string_find_cstr(str, "fizz"), 6);
  TEST_ASSERT_EQUAL(tau_string_find_cstr(str, "foo"), 0);
  TEST_ASSERT_EQUAL(tau_string_find_cstr(str, "bar"), 3);
  TEST_ASSERT_EQUAL(tau_string_find_cstr(str, "oob"), 1);

  tau_string_free(str);
}

TEST_MAIN()
{
  TEST_RUN(tau_string_init);
  TEST_RUN(tau_string_init_with_cstr);
  TEST_RUN(tau_string_append_empty);
  TEST_RUN(tau_string_append);
  TEST_RUN(tau_string_append_cstr_empty);
  TEST_RUN(tau_string_append_cstr);
  TEST_RUN(tau_string_insert_empty);
  TEST_RUN(tau_string_insert_begin);
  TEST_RUN(tau_string_insert_end);
  TEST_RUN(tau_string_insert);
  TEST_RUN(tau_string_insert_cstr_empty);
  TEST_RUN(tau_string_insert_cstr_begin);
  TEST_RUN(tau_string_insert_cstr_end);
  TEST_RUN(tau_string_insert_cstr);
  TEST_RUN(tau_string_erase_empty);
  TEST_RUN(tau_string_erase_begin);
  TEST_RUN(tau_string_erase_end);
  TEST_RUN(tau_string_erase);
  TEST_RUN(tau_string_clear);
  TEST_RUN(tau_string_copy);
  TEST_RUN(tau_string_substr_empty);
  TEST_RUN(tau_string_substr);
  TEST_RUN(tau_string_substr_all);
  TEST_RUN(tau_string_compare);
  TEST_RUN(tau_string_compare_cstr);
  TEST_RUN(tau_string_escape);
  TEST_RUN(tau_string_starts_with_empty);
  TEST_RUN(tau_string_starts_with);
  TEST_RUN(tau_string_starts_with_cstr_empty);
  TEST_RUN(tau_string_starts_with_cstr);
  TEST_RUN(tau_string_ends_with_empty);
  TEST_RUN(tau_string_ends_with);
  TEST_RUN(tau_string_ends_with_cstr_empty);
  TEST_RUN(tau_string_ends_with_cstr);
  TEST_RUN(tau_string_contains_empty);
  TEST_RUN(tau_string_contains);
  TEST_RUN(tau_string_contains_cstr_empty);
  TEST_RUN(tau_string_contains_cstr);
  TEST_RUN(tau_string_replace_all);
  TEST_RUN(tau_string_replace_zero_len);
  TEST_RUN(tau_string_replace_empty);
  TEST_RUN(tau_string_replace);
  TEST_RUN(tau_string_replace_with_substr_all);
  TEST_RUN(tau_string_replace_with_substr_zero_len);
  TEST_RUN(tau_string_replace_with_substr);
  TEST_RUN(tau_string_find);
  TEST_RUN(tau_string_find_cstr);
}
