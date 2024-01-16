#include "test.h"

#include "utils/str.h"

test()

  describe("string")
    describe("string_init")
      it("should initialize new empty string")
        string_t* str = string_init();

        assert_not_null(str);
        assert_equal(string_length(str), 0);

        string_free(str);
      end()

      it("should initialize new string from c-string")
        string_t* str = string_init_with_cstr("foo");

        assert_not_null(str);
        assert_equal(string_length(str), 3);
        assert_str_equal(string_begin(str), "foo");

        string_free(str);
      end()
    end()

    describe("string_append")
      it("should append nothing for empty string")
        string_t* str1 = string_init();
        string_t* str2 = string_init_with_cstr("");
        
        string_append(str1, str2);

        assert_equal(string_length(str1), 0);
        assert_str_equal(string_begin(str1), "");

        assert_equal(string_length(str2), 0);
        assert_str_equal(string_begin(str2), "");
        
        string_free(str2);
        string_free(str1);
      end()

      it("should append for non-empty string")
        string_t* str1 = string_init_with_cstr("foo");
        string_t* str2 = string_init_with_cstr("bar");
        
        string_append(str1, str2);

        assert_equal(string_length(str1), 6);
        assert_str_equal(string_begin(str1), "foobar");

        assert_equal(string_length(str2), 3);
        assert_str_equal(string_begin(str2), "bar");
        
        string_free(str2);
        string_free(str1);
      end()
    end()

    describe("string_append_cstr")
      it("should append nothing for empty string")
        string_t* str = string_init();
        
        string_append_cstr(str, "");

        assert_equal(string_length(str), 0);
        assert_str_equal(string_begin(str), "");
        
        string_free(str);
      end()

      it("should append for non-empty string")
        string_t* str = string_init_with_cstr("foo");
        
        string_append_cstr(str, "bar");

        assert_equal(string_length(str), 6);
        assert_str_equal(string_begin(str), "foobar");

        string_free(str);
      end()
    end()

    describe("string_insert")
      it("should insert empty string")
        string_t* str1 = string_init_with_cstr("foo");
        string_t* str2 = string_init_with_cstr("");

        string_insert(str1, 1, str2);

        assert_true(string_compare_cstr(str1, "foo") == 0);

        string_free(str2);
        string_free(str1);
      end()

      it("should insert non-empty string at start")
        string_t* str1 = string_init_with_cstr("foo");
        string_t* str2 = string_init_with_cstr("bar");

        string_insert(str1, 0, str2);

        assert_true(string_compare_cstr(str1, "barfoo") == 0);

        string_free(str2);
        string_free(str1);
      end()

      it("should insert non-empty string at end")
        string_t* str1 = string_init_with_cstr("foo");
        string_t* str2 = string_init_with_cstr("bar");

        string_insert(str1, 3, str2);

        assert_true(string_compare_cstr(str1, "foobar") == 0);

        string_free(str2);
        string_free(str1);
      end()

      it("should insert non-empty string in middle")
        string_t* str1 = string_init_with_cstr("foo");
        string_t* str2 = string_init_with_cstr("bar");

        string_insert(str1, 2, str2);

        assert_true(string_compare_cstr(str1, "fobaro") == 0);

        string_free(str2);
        string_free(str1);
      end()
    end()

    describe("string_insert_cstr")
      it("should insert empty string")
        string_t* str = string_init_with_cstr("foo");

        string_insert_cstr(str, 1, "");

        assert_true(string_compare_cstr(str, "foo") == 0);

        string_free(str);
      end()

      it("should insert non-empty string at start")
        string_t* str = string_init_with_cstr("foo");

        string_insert_cstr(str, 0, "bar");

        assert_true(string_compare_cstr(str, "barfoo") == 0);

        string_free(str);
      end()

      it("should insert non-empty string at end")
        string_t* str = string_init_with_cstr("foo");

        string_insert_cstr(str, 3, "bar");

        assert_true(string_compare_cstr(str, "foobar") == 0);

        string_free(str);
      end()

      it("should insert non-empty string in middle")
        string_t* str = string_init_with_cstr("foo");

        string_insert_cstr(str, 2, "bar");

        assert_true(string_compare_cstr(str, "fobaro") == 0);

        string_free(str);
      end()
    end()

    describe("string_erase")
      it("should erase nothing")
        string_t* str = string_init_with_cstr("foo");

        string_erase(str, 0, 0);

        assert_true(string_compare_cstr(str, "foo") == 0);

        string_free(str);
      end()

      it("should erase start")
        string_t* str = string_init_with_cstr("foobar");

        string_erase(str, 0, 3);

        assert_true(string_compare_cstr(str, "bar") == 0);

        string_free(str);
      end()

      it("should erase end")
        string_t* str = string_init_with_cstr("foobar");

        string_erase(str, 3, 3);

        assert_true(string_compare_cstr(str, "foo") == 0);

        string_free(str);
      end()

      it("should erase middle")
        string_t* str = string_init_with_cstr("foobar");

        string_erase(str, 2, 2);

        assert_true(string_compare_cstr(str, "foar") == 0);

        string_free(str);
      end()
    end()

    describe("string_clear")
      it("should clear string")
        string_t* str = string_init_with_cstr("foo");

        string_clear(str);

        assert_true(string_compare_cstr(str, "") == 0);

        string_free(str);
      end()
    end()

    describe("string_copy")
      it("should copy string")
        string_t* str = string_init_with_cstr("foo");
        string_t* copy = string_copy(str);

        assert_true(string_compare(str, copy) == 0);

        string_free(copy);
        string_free(str);
      end()
    end()

    describe("string_substr")
      it("should create empty substring")
        string_t* str1 = string_init_with_cstr("foo");
        string_t* str2 = string_substr(str1, 0, 0);

        assert_equal(string_length(str1), 3);
        assert_str_equal(string_begin(str1), "foo");

        assert_equal(string_length(str2), 0);
        assert_str_equal(string_begin(str2), "");

        string_free(str2);
        string_free(str1);
      end()

      it("should create non-empty substring")
        string_t* str1 = string_init_with_cstr("foobarfizz");
        string_t* str2 = string_substr(str1, 3, 3);

        assert_equal(string_length(str1), 10);
        assert_str_equal(string_begin(str1), "foobarfizz");

        assert_equal(string_length(str2), 3);
        assert_str_equal(string_begin(str2), "bar");

        string_free(str2);
        string_free(str1);
      end()

      it("should copy the whole string")
        string_t* str1 = string_init_with_cstr("foobar");
        string_t* str2 = string_substr(str1, 0, 6);

        assert_equal(string_length(str1), 6);
        assert_str_equal(string_begin(str1), "foobar");

        assert_equal(string_length(str2), 6);
        assert_str_equal(string_begin(str2), "foobar");

        string_free(str2);
        string_free(str1);
      end()
    end()

    describe("string_compare")
      it("should compare strings")
        string_t* str1 = string_init_with_cstr("foo");
        string_t* str2 = string_init_with_cstr("bar");

        assert_true(string_compare(str2, str1) < 0);
        assert_true(string_compare(str1, str2) > 0);
        assert_true(string_compare(str1, str1) == 0);
        assert_true(string_compare(str2, str2) == 0);

        string_free(str2);
        string_free(str1);
      end()
    end()

    describe("string_compare_cstr")
      it("should compare strings")
        string_t* str = string_init_with_cstr("foo");

        assert_true(string_compare_cstr(str, "xyz") < 0);
        assert_true(string_compare_cstr(str, "bar") > 0);
        assert_true(string_compare_cstr(str, "foo") == 0);

        string_free(str);
      end()
    end()

    describe("string_escape")
      it("should escape special characters")
        string_t* str = string_init_with_cstr("a'b\"c\\d\ae\bf\fg\nh\ri\tj\vk");
        string_t* expected = string_init_with_cstr("a\\'b\\\"c\\\\d\\ae\\bf\\fg\\nh\\ri\\tj\\vk");
        string_t* escaped = string_escape(str);

        assert_true(string_compare(escaped, expected) == 0);

        string_free(escaped);
        string_free(expected);
        string_free(str);
      end()
    end()

    describe("string_starts_with")
      it("should start with empty string")
        string_t* str = string_init_with_cstr("foo");
        string_t* prefix = string_init();

        assert_true(string_starts_with(str, prefix));

        string_free(prefix);
        string_free(str);
      end()

      it("should not start with non-empty string")
        string_t* str = string_init_with_cstr("foobar");
        string_t* prefix = string_init_with_cstr("fizz");

        assert_false(string_starts_with(str, prefix));

        string_free(prefix);
        string_free(str);
      end()

      it("should start with non-empty string")
        string_t* str = string_init_with_cstr("foobar");
        string_t* prefix = string_init_with_cstr("foo");

        assert_true(string_starts_with(str, prefix));

        string_free(prefix);
        string_free(str);
      end()
    end()

    describe("string_starts_with_cstr")
      it("should start with empty string")
        string_t* str = string_init_with_cstr("foo");

        assert_true(string_starts_with_cstr(str, ""));

        string_free(str);
      end()

      it("should not start with non-empty string")
        string_t* str = string_init_with_cstr("foobar");

        assert_false(string_starts_with_cstr(str, "fizz"));

        string_free(str);
      end()

      it("should start with non-empty string")
        string_t* str = string_init_with_cstr("foobar");

        assert_true(string_starts_with_cstr(str, "foo"));

        string_free(str);
      end()
    end()

    describe("string_ends_with")
      it("should end with empty string")
        string_t* str = string_init_with_cstr("foo");
        string_t* suffix = string_init();

        assert_true(string_ends_with(str, suffix));

        string_free(suffix);
        string_free(str);
      end()

      it("should not end with non-empty string")
        string_t* str = string_init_with_cstr("foobar");
        string_t* suffix = string_init_with_cstr("fizz");

        assert_false(string_ends_with(str, suffix));

        string_free(suffix);
        string_free(str);
      end()

      it("should end with non-empty string")
        string_t* str = string_init_with_cstr("foobar");
        string_t* suffix = string_init_with_cstr("bar");

        assert_true(string_ends_with(str, suffix));

        string_free(suffix);
        string_free(str);
      end()
    end()

    describe("string_ends_with_cstr")
      it("should end with empty string")
        string_t* str = string_init_with_cstr("foo");

        assert_true(string_ends_with_cstr(str, ""));

        string_free(str);
      end()

      it("should not end with non-empty string")
        string_t* str = string_init_with_cstr("foobar");

        assert_false(string_ends_with_cstr(str, "fizz"));

        string_free(str);
      end()

      it("should end with non-empty string")
        string_t* str = string_init_with_cstr("foobar");

        assert_true(string_ends_with_cstr(str, "bar"));

        string_free(str);
      end()
    end()

    describe("string_contains")
      it("should contain empty string")
        string_t* str = string_init_with_cstr("foo");
        string_t* sub = string_init();

        assert_true(string_contains(str, sub));

        string_free(sub);
        string_free(str);
      end()

      it("should not contain non-empty string")
        string_t* str = string_init_with_cstr("foo");
        string_t* sub = string_init_with_cstr("bar");

        assert_false(string_contains(str, sub));

        string_free(sub);
        string_free(str);
      end()

      it("should contain non-empty string at start")
        string_t* str = string_init_with_cstr("foobar");
        string_t* sub = string_init_with_cstr("foo");

        assert_true(string_contains(str, sub));

        string_free(sub);
        string_free(str);
      end()

      it("should contain non-empty string at end")
        string_t* str = string_init_with_cstr("foobar");
        string_t* sub = string_init_with_cstr("bar");

        assert_true(string_contains(str, sub));

        string_free(sub);
        string_free(str);
      end()

      it("should contain non-empty string in middle")
        string_t* str = string_init_with_cstr("foobar");
        string_t* sub = string_init_with_cstr("oba");

        assert_true(string_contains(str, sub));

        string_free(sub);
        string_free(str);
      end()
    end()

    describe("string_contains_cstr")
      it("should contain empty string")
        string_t* str = string_init_with_cstr("foo");

        assert_true(string_contains_cstr(str, ""));

        string_free(str);
      end()

      it("should not contain non-empty string")
        string_t* str = string_init_with_cstr("foo");

        assert_false(string_contains_cstr(str, "bar"));

        string_free(str);
      end()

      it("should contain non-empty string at start")
        string_t* str = string_init_with_cstr("foobar");

        assert_true(string_contains_cstr(str, "foo"));

        string_free(str);
      end()

      it("should contain non-empty string at end")
        string_t* str = string_init_with_cstr("foobar");

        assert_true(string_contains_cstr(str, "bar"));

        string_free(str);
      end()

      it("should contain non-empty string in middle")
        string_t* str = string_init_with_cstr("foobar");

        assert_true(string_contains_cstr(str, "oba"));

        string_free(str);
      end()
    end()

    describe("string_replace")
      it("should replace whole string")
        string_t* str = string_init_with_cstr("foo");
        string_t* rep = string_init_with_cstr("bar");

        string_replace(str, 0, 3, rep);

        assert_true(string_compare(str, rep) == 0);

        string_free(rep);
        string_free(str);
      end()

      it("should replace 0 length at start")
        string_t* str = string_init_with_cstr("foo");
        string_t* rep = string_init_with_cstr("bar");

        string_replace(str, 0, 0, rep);

        assert_true(string_compare_cstr(str, "barfoo") == 0);

        string_free(rep);
        string_free(str);
      end()

      it("should replace 0 length in middle")
        string_t* str = string_init_with_cstr("foo");
        string_t* rep = string_init_with_cstr("bar");

        string_replace(str, 1, 0, rep);

        assert_true(string_compare_cstr(str, "fbaroo") == 0);

        string_free(rep);
        string_free(str);
      end()

      it("should replace start")
        string_t* str = string_init_with_cstr("foobar");
        string_t* rep = string_init_with_cstr("fizz");

        string_replace(str, 0, 3, rep);

        assert_true(string_compare_cstr(str, "fizzbar") == 0);

        string_free(rep);
        string_free(str);
      end()

      it("should replace end")
        string_t* str = string_init_with_cstr("foobar");
        string_t* rep = string_init_with_cstr("fizz");

        string_replace(str, 3, 3, rep);

        assert_true(string_compare_cstr(str, "foofizz") == 0);

        string_free(rep);
        string_free(str);
      end()

      it("should replace middle")
        string_t* str = string_init_with_cstr("foobar");
        string_t* rep = string_init_with_cstr("fizz");

        string_replace(str, 2, 2, rep);

        assert_true(string_compare_cstr(str, "fofizzar") == 0);

        string_free(rep);
        string_free(str);
      end()

      it("should replace with empty string")
        string_t* str = string_init_with_cstr("foobar");
        string_t* rep = string_init_with_cstr("");

        string_replace(str, 2, 2, rep);

        assert_true(string_compare_cstr(str, "foar") == 0);

        string_free(rep);
        string_free(str);
      end()
    end()

    describe("string_replace_with_substr")
      it("should replace whole string")
        string_t* str = string_init_with_cstr("foo");
        string_t* rep = string_init_with_cstr("fizzbar");

        string_replace_with_substr(str, 0, 3, rep, 4);

        assert_true(string_compare_cstr(str, "bar") == 0);

        string_free(rep);
        string_free(str);
      end()

      it("should replace 0 length at start")
        string_t* str = string_init_with_cstr("foo");
        string_t* rep = string_init_with_cstr("fizzbar");

        string_replace_with_substr(str, 0, 0, rep, 4);

        assert_true(string_compare_cstr(str, "barfoo") == 0);

        string_free(rep);
        string_free(str);
      end()

      it("should replace 0 length in middle")
        string_t* str = string_init_with_cstr("foo");
        string_t* rep = string_init_with_cstr("fizzbar");

        string_replace_with_substr(str, 1, 0, rep, 4);

        assert_true(string_compare_cstr(str, "fbaroo") == 0);

        string_free(rep);
        string_free(str);
      end()

      it("should replace start")
        string_t* str = string_init_with_cstr("foobar");
        string_t* rep = string_init_with_cstr("fuzzfizz");

        string_replace_with_substr(str, 0, 3, rep, 4);

        assert_true(string_compare_cstr(str, "fizzbar") == 0);

        string_free(rep);
        string_free(str);
      end()

      it("should replace end")
        string_t* str = string_init_with_cstr("foobar");
        string_t* rep = string_init_with_cstr("fuzzfizz");

        string_replace_with_substr(str, 3, 3, rep, 4);

        assert_true(string_compare_cstr(str, "foofizz") == 0);

        string_free(rep);
        string_free(str);
      end()

      it("should replace middle")
        string_t* str = string_init_with_cstr("foobar");
        string_t* rep = string_init_with_cstr("fuzzfizz");

        string_replace_with_substr(str, 2, 2, rep, 4);

        assert_true(string_compare_cstr(str, "fofizzar") == 0);

        string_free(rep);
        string_free(str);
      end()

      it("should replace with empty string")
        string_t* str = string_init_with_cstr("foobar");
        string_t* rep = string_init_with_cstr("");

        string_replace_with_substr(str, 2, 2, rep, 0);

        assert_true(string_compare_cstr(str, "foar") == 0);

        string_free(rep);
        string_free(str);
      end()
    end()

    describe("string_find")
      it("should not find sub-string")
        string_t* str = string_init_with_cstr("foo");
        string_t* sub = string_init_with_cstr("bar");

        size_t pos = string_find(str, sub);

        assert_equal(pos, 3);

        string_free(sub);
        string_free(str);
      end()

      it("should find sub-string at start")
        string_t* str = string_init_with_cstr("foobar");
        string_t* sub = string_init_with_cstr("foo");

        size_t pos = string_find(str, sub);

        assert_equal(pos, 0);

        string_free(sub);
        string_free(str);
      end()

      it("should find sub-string at end")
        string_t* str = string_init_with_cstr("foobar");
        string_t* sub = string_init_with_cstr("bar");

        size_t pos = string_find(str, sub);

        assert_equal(pos, 3);

        string_free(sub);
        string_free(str);
      end()

      it("should find sub-string in middle")
        string_t* str = string_init_with_cstr("foobar");
        string_t* sub = string_init_with_cstr("oba");

        size_t pos = string_find(str, sub);

        assert_equal(pos, 2);

        string_free(sub);
        string_free(str);
      end()
    end()

    describe("string_find_cstr")
      it("should not find sub-string")
        string_t* str = string_init_with_cstr("foo");

        size_t pos = string_find_cstr(str, "bar");

        assert_equal(pos, 3);

        string_free(str);
      end()

      it("should find sub-string at start")
        string_t* str = string_init_with_cstr("foobar");

        size_t pos = string_find_cstr(str, "foo");

        assert_equal(pos, 0);

        string_free(str);
      end()

      it("should find sub-string at end")
        string_t* str = string_init_with_cstr("foobar");

        size_t pos = string_find_cstr(str, "bar");

        assert_equal(pos, 3);

        string_free(str);
      end()

      it("should find sub-string in middle")
        string_t* str = string_init_with_cstr("foobar");

        size_t pos = string_find_cstr(str, "oba");

        assert_equal(pos, 2);

        string_free(str);
      end()
    end()
  end()

end()
