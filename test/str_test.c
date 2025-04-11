#include "test.h"

#include "utils/str.h"

TEST_CASE(string_init)
{
  string_t* str = string_init();

  TEST_ASSERT_NOT_NULL(str);
  TEST_ASSERT_EQUAL(string_length(str), 0);

  string_free(str);
}

TEST_CASE(string_init_with_cstr)
{
  string_t* str = string_init_with_cstr("foo");

  TEST_ASSERT_NOT_NULL(str);
  TEST_ASSERT_EQUAL(string_length(str), 3);
  TEST_ASSERT_STR_EQUAL(string_begin(str), "foo");

  string_free(str);
}

TEST_CASE(string_append_empty)
{
  string_t* str1 = string_init();
  string_t* str2 = string_init_with_cstr("");

  string_append(str1, str2);

  TEST_ASSERT_EQUAL(string_length(str1), 0);
  TEST_ASSERT_STR_EQUAL(string_begin(str1), "");

  TEST_ASSERT_EQUAL(string_length(str2), 0);
  TEST_ASSERT_STR_EQUAL(string_begin(str2), "");

  string_free(str2);
  string_free(str1);
}

TEST_CASE(string_append)
{
  string_t* str1 = string_init_with_cstr("foo");
  string_t* str2 = string_init_with_cstr("bar");

  string_append(str1, str2);

  TEST_ASSERT_EQUAL(string_length(str1), 6);
  TEST_ASSERT_STR_EQUAL(string_begin(str1), "foobar");

  TEST_ASSERT_EQUAL(string_length(str2), 3);
  TEST_ASSERT_STR_EQUAL(string_begin(str2), "bar");

  string_free(str2);
  string_free(str1);
}

TEST_CASE(string_append_cstr_empty)
{
  string_t* str = string_init();

  string_append_cstr(str, "");

  TEST_ASSERT_EQUAL(string_length(str), 0);
  TEST_ASSERT_STR_EQUAL(string_begin(str), "");

  string_free(str);
}

TEST_CASE(string_append_cstr)
{
  string_t* str = string_init_with_cstr("foo");

  string_append_cstr(str, "bar");

  TEST_ASSERT_EQUAL(string_length(str), 6);
  TEST_ASSERT_STR_EQUAL(string_begin(str), "foobar");

  string_free(str);
}

TEST_CASE(string_insert_empty)
{
  string_t* str1 = string_init_with_cstr("foo");
  string_t* str2 = string_init_with_cstr("");

  string_insert(str1, 1, str2);

  TEST_ASSERT_TRUE(string_compare_cstr(str1, "foo") == 0);

  string_free(str2);
  string_free(str1);
}

TEST_CASE(string_insert_begin)
{
  string_t* str1 = string_init_with_cstr("foo");
  string_t* str2 = string_init_with_cstr("bar");

  string_insert(str1, 0, str2);

  TEST_ASSERT_TRUE(string_compare_cstr(str1, "barfoo") == 0);

  string_free(str2);
  string_free(str1);
}

TEST_CASE(string_insert_end)
{
  string_t* str1 = string_init_with_cstr("foo");
  string_t* str2 = string_init_with_cstr("bar");

  string_insert(str1, 3, str2);

  TEST_ASSERT_TRUE(string_compare_cstr(str1, "foobar") == 0);

  string_free(str2);
  string_free(str1);
}

TEST_CASE(string_insert)
{
  string_t* str1 = string_init_with_cstr("foo");
  string_t* str2 = string_init_with_cstr("bar");

  string_insert(str1, 2, str2);

  TEST_ASSERT_TRUE(string_compare_cstr(str1, "fobaro") == 0);

  string_free(str2);
  string_free(str1);
}

TEST_CASE(string_insert_cstr_empty)
{
  string_t* str = string_init_with_cstr("foo");

  string_insert_cstr(str, 1, "");

  TEST_ASSERT_TRUE(string_compare_cstr(str, "foo") == 0);

  string_free(str);
}

TEST_CASE(string_insert_cstr_begin)
{
  string_t* str = string_init_with_cstr("foo");

  string_insert_cstr(str, 0, "bar");

  TEST_ASSERT_TRUE(string_compare_cstr(str, "barfoo") == 0);

  string_free(str);
}

TEST_CASE(string_insert_cstr_end)
{
  string_t* str = string_init_with_cstr("foo");

  string_insert_cstr(str, 3, "bar");

  TEST_ASSERT_TRUE(string_compare_cstr(str, "foobar") == 0);

  string_free(str);
}

TEST_CASE(string_insert_cstr)
{
  string_t* str = string_init_with_cstr("foo");

  string_insert_cstr(str, 2, "bar");

  TEST_ASSERT_TRUE(string_compare_cstr(str, "fobaro") == 0);

  string_free(str);
}

TEST_CASE(string_erase_empty)
{
  string_t* str = string_init_with_cstr("foo");

  string_erase(str, 0, 0);

  TEST_ASSERT_TRUE(string_compare_cstr(str, "foo") == 0);

  string_free(str);
}

TEST_CASE(string_erase_begin)
{
  string_t* str = string_init_with_cstr("foobar");

  string_erase(str, 0, 3);

  TEST_ASSERT_TRUE(string_compare_cstr(str, "bar") == 0);

  string_free(str);
}

TEST_CASE(string_erase_end)
{
  string_t* str = string_init_with_cstr("foobar");

  string_erase(str, 3, 3);

  TEST_ASSERT_TRUE(string_compare_cstr(str, "foo") == 0);

  string_free(str);
}

TEST_CASE(string_erase)
{
  string_t* str = string_init_with_cstr("foobar");

  string_erase(str, 2, 2);

  TEST_ASSERT_TRUE(string_compare_cstr(str, "foar") == 0);

  string_free(str);
}

TEST_CASE(string_clear)
{
  string_t* str = string_init_with_cstr("foo");

  string_clear(str);

  TEST_ASSERT_TRUE(string_compare_cstr(str, "") == 0);

  string_free(str);
}

TEST_CASE(string_copy)
{
  string_t* str = string_init_with_cstr("foo");
  string_t* copy = string_copy(str);

  TEST_ASSERT_TRUE(string_compare(str, copy) == 0);

  string_free(copy);
  string_free(str);
}

TEST_CASE(string_substr_empty)
{
  string_t* str1 = string_init_with_cstr("foo");
  string_t* str2 = string_substr(str1, 0, 0);

  TEST_ASSERT_EQUAL(string_length(str1), 3);
  TEST_ASSERT_STR_EQUAL(string_begin(str1), "foo");

  TEST_ASSERT_EQUAL(string_length(str2), 0);
  TEST_ASSERT_STR_EQUAL(string_begin(str2), "");

  string_free(str2);
  string_free(str1);
}

TEST_CASE(string_substr)
{
  string_t* str1 = string_init_with_cstr("foobarfizz");
  string_t* str2 = string_substr(str1, 3, 3);

  TEST_ASSERT_EQUAL(string_length(str1), 10);
  TEST_ASSERT_STR_EQUAL(string_begin(str1), "foobarfizz");

  TEST_ASSERT_EQUAL(string_length(str2), 3);
  TEST_ASSERT_STR_EQUAL(string_begin(str2), "bar");

  string_free(str2);
  string_free(str1);
}

TEST_CASE(string_substr_all)
{
  string_t* str1 = string_init_with_cstr("foobar");
  string_t* str2 = string_substr(str1, 0, 6);

  TEST_ASSERT_EQUAL(string_length(str1), 6);
  TEST_ASSERT_STR_EQUAL(string_begin(str1), "foobar");

  TEST_ASSERT_EQUAL(string_length(str2), 6);
  TEST_ASSERT_STR_EQUAL(string_begin(str2), "foobar");

  string_free(str2);
  string_free(str1);
}

TEST_CASE(string_compare)
{
  string_t* str1 = string_init_with_cstr("foo");
  string_t* str2 = string_init_with_cstr("bar");

  TEST_ASSERT_TRUE(string_compare(str2, str1) < 0);
  TEST_ASSERT_TRUE(string_compare(str1, str2) > 0);
  TEST_ASSERT_TRUE(string_compare(str1, str1) == 0);
  TEST_ASSERT_TRUE(string_compare(str2, str2) == 0);

  string_free(str2);
  string_free(str1);
}

TEST_CASE(string_compare_cstr)
{
  string_t* str = string_init_with_cstr("foo");

  TEST_ASSERT_TRUE(string_compare_cstr(str, "xyz") < 0);
  TEST_ASSERT_TRUE(string_compare_cstr(str, "bar") > 0);
  TEST_ASSERT_TRUE(string_compare_cstr(str, "foo") == 0);

  string_free(str);
}

TEST_CASE(string_escape)
{
  string_t* str = string_init_with_cstr("a'b\"c\\d\ae\bf\fg\nh\ri\tj\vk");
  string_t* expected = string_init_with_cstr("a\\'b\\\"c\\\\d\\ae\\bf\\fg\\nh\\ri\\tj\\vk");
  string_t* escaped = string_escape(str);

  TEST_ASSERT_TRUE(string_compare(escaped, expected) == 0);

  string_free(escaped);
  string_free(expected);
  string_free(str);
}

TEST_CASE(string_starts_with_empty)
{
  string_t* str = string_init_with_cstr("foo");
  string_t* prefix = string_init();

  TEST_ASSERT_TRUE(string_starts_with(str, prefix));

  string_free(prefix);
  string_free(str);
}

TEST_CASE(string_starts_with)
{
  string_t* str = string_init_with_cstr("foobar");
  string_t* prefix1 = string_init_with_cstr("fizz");
  string_t* prefix2 = string_init_with_cstr("foo");

  TEST_ASSERT_FALSE(string_starts_with(str, prefix1));
  TEST_ASSERT_TRUE(string_starts_with(str, prefix2));

  string_free(prefix2);
  string_free(prefix1);
  string_free(str);
}

TEST_CASE(string_starts_with_cstr_empty)
{
  string_t* str = string_init_with_cstr("foo");

  TEST_ASSERT_TRUE(string_starts_with_cstr(str, ""));

  string_free(str);
}

TEST_CASE(string_starts_with_cstr)
{
  string_t* str = string_init_with_cstr("foobar");

  TEST_ASSERT_FALSE(string_starts_with_cstr(str, "fizz"));
  TEST_ASSERT_TRUE(string_starts_with_cstr(str, "foo"));

  string_free(str);
}

TEST_CASE(string_ends_with_empty)
{
  string_t* str = string_init_with_cstr("foo");
  string_t* suffix = string_init();

  TEST_ASSERT_TRUE(string_ends_with(str, suffix));

  string_free(suffix);
  string_free(str);
}

TEST_CASE(string_ends_with)
{
  string_t* str = string_init_with_cstr("foobar");
  string_t* suffix1 = string_init_with_cstr("fizz");
  string_t* suffix2 = string_init_with_cstr("bar");

  TEST_ASSERT_FALSE(string_ends_with(str, suffix1));
  TEST_ASSERT_TRUE(string_ends_with(str, suffix2));

  string_free(suffix2);
  string_free(suffix1);
  string_free(str);
}

TEST_CASE(string_ends_with_cstr_empty)
{
  string_t* str = string_init_with_cstr("foo");

  TEST_ASSERT_TRUE(string_ends_with_cstr(str, ""));

  string_free(str);
}

TEST_CASE(string_ends_with_cstr)
{
  string_t* str = string_init_with_cstr("foobar");

  TEST_ASSERT_FALSE(string_ends_with_cstr(str, "fizz"));
  TEST_ASSERT_TRUE(string_ends_with_cstr(str, "bar"));

  string_free(str);

}

TEST_CASE(string_contains_empty)
{
  string_t* str = string_init_with_cstr("foo");
  string_t* sub = string_init();

  TEST_ASSERT_TRUE(string_contains(str, sub));

  string_free(sub);
  string_free(str);
}

TEST_CASE(string_contains)
{
  string_t* str = string_init_with_cstr("foobar");
  string_t* sub1 = string_init_with_cstr("fizz");
  string_t* sub2 = string_init_with_cstr("foo");
  string_t* sub3 = string_init_with_cstr("bar");
  string_t* sub4 = string_init_with_cstr("oob");

  TEST_ASSERT_FALSE(string_contains(str, sub1));
  TEST_ASSERT_TRUE(string_contains(str, sub2));
  TEST_ASSERT_TRUE(string_contains(str, sub3));
  TEST_ASSERT_TRUE(string_contains(str, sub4));

  string_free(sub4);
  string_free(sub3);
  string_free(sub2);
  string_free(sub1);
  string_free(str);
}

TEST_CASE(string_contains_cstr_empty)
{
  string_t* str = string_init_with_cstr("foo");

  TEST_ASSERT_TRUE(string_contains_cstr(str, ""));

  string_free(str);
}

TEST_CASE(string_contains_cstr)
{
  string_t* str = string_init_with_cstr("foobar");

  TEST_ASSERT_FALSE(string_contains_cstr(str, "fizz"));
  TEST_ASSERT_TRUE(string_contains_cstr(str, "foo"));
  TEST_ASSERT_TRUE(string_contains_cstr(str, "bar"));
  TEST_ASSERT_TRUE(string_contains_cstr(str, "oob"));

  string_free(str);
}

TEST_CASE(string_replace_all)
{
  string_t* str = string_init_with_cstr("foo");
  string_t* rep = string_init_with_cstr("bar");

  string_replace(str, 0, 3, rep);

  TEST_ASSERT_TRUE(string_compare(str, rep) == 0);

  string_free(rep);
  string_free(str);
}

TEST_CASE(string_replace_zero_len)
{
  string_t* str = string_init_with_cstr("foo");
  string_t* rep = string_init_with_cstr("bar");

  string_replace(str, 0, 0, rep);

  TEST_ASSERT_TRUE(string_compare_cstr(str, "barfoo") == 0);

  string_replace(str, 6, 0, rep);

  TEST_ASSERT_TRUE(string_compare_cstr(str, "barfoobar") == 0);

  string_replace(str, 3, 0, rep);

  TEST_ASSERT_TRUE(string_compare_cstr(str, "barbarfoobar") == 0);

  string_free(rep);
  string_free(str);
}

TEST_CASE(string_replace_empty)
{
  string_t* str = string_init_with_cstr("foobar");
  string_t* rep = string_init_with_cstr("");

  string_replace(str, 2, 2, rep);

  TEST_ASSERT_TRUE(string_compare_cstr(str, "foar") == 0);

  string_free(rep);
  string_free(str);
}

TEST_CASE(string_replace)
{
  string_t* str = string_init_with_cstr("foobar");
  string_t* rep = string_init_with_cstr("fizz");

  string_replace(str, 0, 4, rep);

  TEST_ASSERT_TRUE(string_compare_cstr(str, "fizzar") == 0);

  string_replace(str, 2, 4, rep);

  TEST_ASSERT_TRUE(string_compare_cstr(str, "fifizz") == 0);

  string_replace(str, 3, 2, rep);

  TEST_ASSERT_TRUE(string_compare_cstr(str, "fiffizzz") == 0);

  string_free(rep);
  string_free(str);
}

TEST_CASE(string_replace_with_substr_all)
{
  string_t* str = string_init_with_cstr("foo");
  string_t* rep = string_init_with_cstr("fizzbar");

  string_replace_with_substr(str, 0, 3, rep, 4);

  TEST_ASSERT_TRUE(string_compare_cstr(str, "bar") == 0);

  string_free(rep);
  string_free(str);
}

TEST_CASE(string_replace_with_substr_zero_len)
{
  string_t* str = string_init_with_cstr("foo");
  string_t* rep = string_init_with_cstr("bar");

  string_replace_with_substr(str, 0, 0, rep, 1);

  TEST_ASSERT_TRUE(string_compare_cstr(str, "arfoo") == 0);

  string_replace_with_substr(str, 5, 0, rep, 1);

  TEST_ASSERT_TRUE(string_compare_cstr(str, "arfooar") == 0);

  string_replace_with_substr(str, 4, 0, rep, 1);

  TEST_ASSERT_TRUE(string_compare_cstr(str, "arfoaroar") == 0);

  string_free(rep);
  string_free(str);
}

TEST_CASE(string_replace_with_substr)
{
  string_t* str = string_init_with_cstr("foobar");
  string_t* rep = string_init_with_cstr("fizz");

  string_replace_with_substr(str, 0, 3, rep, 2);

  TEST_ASSERT_TRUE(string_compare_cstr(str, "zzbar") == 0);

  string_replace_with_substr(str, 4, 1, rep, 1);

  TEST_ASSERT_TRUE(string_compare_cstr(str, "zzbaizz") == 0);

  string_replace_with_substr(str, 3, 2, rep, 1);

  TEST_ASSERT_TRUE(string_compare_cstr(str, "zzbizzzz") == 0);

  string_free(rep);
  string_free(str);
}

TEST_CASE(string_find)
{
  string_t* str = string_init_with_cstr("foobar");
  string_t* sub1 = string_init_with_cstr("fizz");
  string_t* sub2 = string_init_with_cstr("foo");
  string_t* sub3 = string_init_with_cstr("bar");
  string_t* sub4 = string_init_with_cstr("oob");

  TEST_ASSERT_EQUAL(string_find(str, sub1), 6);
  TEST_ASSERT_EQUAL(string_find(str, sub2), 0);
  TEST_ASSERT_EQUAL(string_find(str, sub3), 3);
  TEST_ASSERT_EQUAL(string_find(str, sub4), 1);

  string_free(sub4);
  string_free(sub3);
  string_free(sub2);
  string_free(sub1);
  string_free(str);
}

TEST_CASE(string_find_cstr)
{
  string_t* str = string_init_with_cstr("foobar");

  TEST_ASSERT_EQUAL(string_find_cstr(str, "fizz"), 6);
  TEST_ASSERT_EQUAL(string_find_cstr(str, "foo"), 0);
  TEST_ASSERT_EQUAL(string_find_cstr(str, "bar"), 3);
  TEST_ASSERT_EQUAL(string_find_cstr(str, "oob"), 1);

  string_free(str);
}

TEST_MAIN()
{
  TEST_RUN(string_init);
  TEST_RUN(string_init_with_cstr);
  TEST_RUN(string_append_empty);
  TEST_RUN(string_append);
  TEST_RUN(string_append_cstr_empty);
  TEST_RUN(string_append_cstr);
  TEST_RUN(string_insert_empty);
  TEST_RUN(string_insert_begin);
  TEST_RUN(string_insert_end);
  TEST_RUN(string_insert);
  TEST_RUN(string_insert_cstr_empty);
  TEST_RUN(string_insert_cstr_begin);
  TEST_RUN(string_insert_cstr_end);
  TEST_RUN(string_insert_cstr);
  TEST_RUN(string_erase_empty);
  TEST_RUN(string_erase_begin);
  TEST_RUN(string_erase_end);
  TEST_RUN(string_erase);
  TEST_RUN(string_clear);
  TEST_RUN(string_copy);
  TEST_RUN(string_substr_empty);
  TEST_RUN(string_substr);
  TEST_RUN(string_substr_all);
  TEST_RUN(string_compare);
  TEST_RUN(string_compare_cstr);
  TEST_RUN(string_escape);
  TEST_RUN(string_starts_with_empty);
  TEST_RUN(string_starts_with);
  TEST_RUN(string_starts_with_cstr_empty);
  TEST_RUN(string_starts_with_cstr);
  TEST_RUN(string_ends_with_empty);
  TEST_RUN(string_ends_with);
  TEST_RUN(string_ends_with_cstr_empty);
  TEST_RUN(string_ends_with_cstr);
  TEST_RUN(string_contains_empty);
  TEST_RUN(string_contains);
  TEST_RUN(string_contains_cstr_empty);
  TEST_RUN(string_contains_cstr);
  TEST_RUN(string_replace_all);
  TEST_RUN(string_replace_zero_len);
  TEST_RUN(string_replace_empty);
  TEST_RUN(string_replace);
  TEST_RUN(string_replace_with_substr_all);
  TEST_RUN(string_replace_with_substr_zero_len);
  TEST_RUN(string_replace_with_substr);
  TEST_RUN(string_find);
  TEST_RUN(string_find_cstr);
}
