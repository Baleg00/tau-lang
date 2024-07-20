#include "test.h"

#include "stages/lexer/lexer.h"
#include "stages/lexer/token/registry.h"
#include "utils/common.h"

test()
  describe("lexer")
    lexer_t* lex = NULL;
    vector_t* toks = NULL;

    before()
      lex = lexer_init();
    end()

    after()
      lexer_free(lex);
      token_registry_free();
    end()

    after_each()
      vector_free(toks);
    end()

    it("should lex empty source code")
      const char* src = "";

      toks = lexer_lex(lex, "", src);

      assert_false(vector_empty(toks));
      assert_equal(vector_size(toks), 1);
      assert_equal(((token_t*)vector_get(toks, 0))->kind, TOK_EOF);
    end()

    it("should lex identifiers")
      const char* src = "foo BaR _f_i_z_z __BUZZ__";

      toks = lexer_lex(lex, "", src);

      assert_false(vector_empty(toks));
      assert_equal(vector_size(toks), 5);
      assert_equal(((token_t*)vector_get(toks, 0))->kind, TOK_ID);
      assert_equal(((token_t*)vector_get(toks, 1))->kind, TOK_ID);
      assert_equal(((token_t*)vector_get(toks, 2))->kind, TOK_ID);
      assert_equal(((token_t*)vector_get(toks, 3))->kind, TOK_ID);
      assert_equal(((token_t*)vector_get(toks, 4))->kind, TOK_EOF);
    end()

    it("should lex keywords")
      const token_kind_t keyword_kinds[] = {
        TOK_KW_IS,
        TOK_KW_AS,
        TOK_KW_SIZEOF,
        TOK_KW_ALIGNOF,
        TOK_KW_USE,
        TOK_KW_IN,
        TOK_KW_PUB,
        TOK_KW_EXTERN,
        TOK_KW_FUN,
        TOK_KW_STRUCT,
        TOK_KW_UNION,
        TOK_KW_ENUM,
        TOK_KW_MOD,
        TOK_KW_IF,
        TOK_KW_THEN,
        TOK_KW_ELSE,
        TOK_KW_FOR,
        TOK_KW_WHILE,
        TOK_KW_DO,
        TOK_KW_BREAK,
        TOK_KW_CONTINUE,
        TOK_KW_RETURN,
        TOK_KW_DEFER,
        TOK_KW_MUT,
        TOK_KW_I8,
        TOK_KW_I16,
        TOK_KW_I32,
        TOK_KW_I64,
        TOK_KW_ISIZE,
        TOK_KW_U8,
        TOK_KW_U16,
        TOK_KW_U32,
        TOK_KW_U64,
        TOK_KW_USIZE,
        TOK_KW_F32,
        TOK_KW_F64,
        TOK_KW_CHAR,
        TOK_KW_BOOL,
        TOK_KW_UNIT,
      };

      const char* src = "is as sizeof alignof use in pub extern fun struct union enum mod if then else for while do break continue return defer mut i8 i16 i32 i64 isize u8 u16 u32 u64 usize f32 f64 char bool unit";

      toks = lexer_lex(lex, "", src);

      assert_false(vector_empty(toks));
      assert_equal(vector_size(toks), COUNTOF(keyword_kinds) + 1);

      for (size_t i = 0; i < COUNTOF(keyword_kinds); i++)
      {
        assert_equal(((token_t*)vector_get(toks, i))->kind, keyword_kinds[i]);
      }

      assert_equal(((token_t*)vector_back(toks))->kind, TOK_EOF);
    end()

    it("should lex integer literals")
      const char* src = "0 1 123 0x1234567890abcdefABCDEF 0X1234567890abcdefABCDEF 0o12345670 0O12345670 0b10 0B10";

      toks = lexer_lex(lex, "", src);

      assert_equal(vector_size(toks), 10);

      for (size_t i = 0; i < vector_size(toks) - 1; i++)
      {
        assert_equal(((token_t*)vector_get(toks, i))->kind, TOK_LIT_INT);
      }

      assert_equal(((token_t*)vector_back(toks))->kind, TOK_EOF);
    end()
  end()
end()
