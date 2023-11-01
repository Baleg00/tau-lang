#include "test.h"

#include "str_view.h"

test()

  describe("string_view")
    describe("string_view_init")
      it("should initialize string view")
        string_view_t str = string_view_init("foo");

        assert_true(string_view_compare_cstr(str, "foo") == 0);
      end()

      it("should initialize string view with length")
        string_view_t str = string_view_init_with_length("foobar", 3);

        assert_true(string_view_compare_cstr(str, "foo") == 0);
      end()
    end()

    describe("string_view_substr")
      it("should create empty substring")
        string_view_t str = string_view_init("foo");
        string_view_t sub = string_view_substr(str, 0, 0);

        assert_true(string_view_compare_cstr(sub, "") == 0);
      end()

      it("should create non-empty substring")
        string_view_t str = string_view_init("foobarfizz");
        string_view_t sub = string_view_substr(str, 3, 3);

        assert_true(string_view_compare_cstr(sub, "bar") == 0);
      end()

      it("should copy the whole string")
        string_view_t str = string_view_init("foobar");
        string_view_t sub = string_view_substr(str, 0, 6);

        assert_true(string_view_compare_cstr(sub, "foobar") == 0);
      end()
    end()

    describe("string_view_compare")
      it("should compare strings")
        string_view_t str1 = string_view_init("foo");
        string_view_t str2 = string_view_init("bar");

        assert_true(string_view_compare(str2, str1) < 0);
        assert_true(string_view_compare(str1, str2) > 0);
        assert_true(string_view_compare(str1, str1) == 0);
        assert_true(string_view_compare(str2, str2) == 0);
      end()
    end()

    describe("string_view_compare_cstr")
      it("should compare strings")
        string_view_t str = string_view_init("foo");

        assert_true(string_view_compare_cstr(str, "xyz") < 0);
        assert_true(string_view_compare_cstr(str, "bar") > 0);
        assert_true(string_view_compare_cstr(str, "foo") == 0);
      end()
    end()

    describe("string_view_starts_with")
      it("should start with empty string")
        string_view_t str = string_view_init("foo");
        string_view_t prefix = string_view_init("");

        assert_true(string_view_starts_with(str, prefix));
      end()

      it("should not start with non-empty string")
        string_view_t str = string_view_init("foobar");
        string_view_t prefix = string_view_init("fizz");

        assert_false(string_view_starts_with(str, prefix));
      end()

      it("should start with non-empty string")
        string_view_t str = string_view_init("foobar");
        string_view_t prefix = string_view_init("foo");

        assert_true(string_view_starts_with(str, prefix));
      end()
    end()

    describe("string_view_starts_with_cstr")
      it("should start with empty string")
        string_view_t str = string_view_init("foo");

        assert_true(string_view_starts_with_cstr(str, ""));
      end()

      it("should not start with non-empty string")
        string_view_t str = string_view_init("foobar");

        assert_false(string_view_starts_with_cstr(str, "fizz"));
      end()

      it("should start with non-empty string")
        string_view_t str = string_view_init("foobar");

        assert_true(string_view_starts_with_cstr(str, "foo"));
      end()
    end()

    describe("string_view_ends_with")
      it("should end with empty string")
        string_view_t str = string_view_init("foo");
        string_view_t suffix = string_view_init("");

        assert_true(string_view_ends_with(str, suffix));
      end()

      it("should not end with non-empty string")
        string_view_t str = string_view_init("foobar");
        string_view_t suffix = string_view_init("fizz");

        assert_false(string_view_ends_with(str, suffix));
      end()

      it("should end with non-empty string")
        string_view_t str = string_view_init("foobar");
        string_view_t suffix = string_view_init("bar");

        assert_true(string_view_ends_with(str, suffix));
      end()
    end()

    describe("string_view_ends_with_cstr")
      it("should end with empty string")
        string_view_t str = string_view_init("foo");

        assert_true(string_view_ends_with_cstr(str, ""));
      end()

      it("should not end with non-empty string")
        string_view_t str = string_view_init("foobar");

        assert_false(string_view_ends_with_cstr(str, "fizz"));
      end()

      it("should end with non-empty string")
        string_view_t str = string_view_init("foobar");

        assert_true(string_view_ends_with_cstr(str, "bar"));
      end()
    end()

    describe("string_view_contains")
      it("should contain empty string")
        string_view_t str = string_view_init("foo");
        string_view_t sub = string_view_init("");

        assert_true(string_view_contains(str, sub));
      end()

      it("should not contain non-empty string")
        string_view_t str = string_view_init("foo");
        string_view_t sub = string_view_init("bar");

        assert_false(string_view_contains(str, sub));
      end()

      it("should contain non-empty string at start")
        string_view_t str = string_view_init("foobar");
        string_view_t sub = string_view_init("foo");

        assert_true(string_view_contains(str, sub));
      end()

      it("should contain non-empty string at end")
        string_view_t str = string_view_init("foobar");
        string_view_t sub = string_view_init("bar");

        assert_true(string_view_contains(str, sub));
      end()

      it("should contain non-empty string in middle")
        string_view_t str = string_view_init("foobar");
        string_view_t sub = string_view_init("oba");

        assert_true(string_view_contains(str, sub));
      end()
    end()

    describe("string_view_contains_cstr")
      it("should contain empty string")
        string_view_t str = string_view_init("foo");

        assert_true(string_view_contains_cstr(str, ""));
      end()

      it("should not contain non-empty string")
        string_view_t str = string_view_init("foo");

        assert_false(string_view_contains_cstr(str, "bar"));
      end()

      it("should contain non-empty string at start")
        string_view_t str = string_view_init("foobar");

        assert_true(string_view_contains_cstr(str, "foo"));
      end()

      it("should contain non-empty string at end")
        string_view_t str = string_view_init("foobar");

        assert_true(string_view_contains_cstr(str, "bar"));
      end()

      it("should contain non-empty string in middle")
        string_view_t str = string_view_init("foobar");

        assert_true(string_view_contains_cstr(str, "oba"));
      end()
    end()

    describe("string_view_find")
      it("should not find sub-string")
        string_view_t str = string_view_init("foo");
        string_view_t sub = string_view_init("bar");

        size_t pos = string_view_find(str, sub);

        assert_equal(pos, 3);
      end()

      it("should find sub-string at start")
        string_view_t str = string_view_init("foobar");
        string_view_t sub = string_view_init("foo");

        size_t pos = string_view_find(str, sub);

        assert_equal(pos, 0);
      end()

      it("should find sub-string at end")
        string_view_t str = string_view_init("foobar");
        string_view_t sub = string_view_init("bar");

        size_t pos = string_view_find(str, sub);

        assert_equal(pos, 3);
      end()

      it("should find sub-string in middle")
        string_view_t str = string_view_init("foobar");
        string_view_t sub = string_view_init("oba");

        size_t pos = string_view_find(str, sub);

        assert_equal(pos, 2);
      end()
    end()

    describe("string_view_find_cstr")
      it("should not find sub-string")
        string_view_t str = string_view_init("foo");

        size_t pos = string_view_find_cstr(str, "bar");

        assert_equal(pos, 3);
      end()

      it("should find sub-string at start")
        string_view_t str = string_view_init("foobar");

        size_t pos = string_view_find_cstr(str, "foo");

        assert_equal(pos, 0);
      end()

      it("should find sub-string at end")
        string_view_t str = string_view_init("foobar");

        size_t pos = string_view_find_cstr(str, "bar");

        assert_equal(pos, 3);
      end()

      it("should find sub-string in middle")
        string_view_t str = string_view_init("foobar");

        size_t pos = string_view_find_cstr(str, "oba");

        assert_equal(pos, 2);
      end()
    end()
  end()

end()
