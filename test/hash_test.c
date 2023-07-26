#include "test.h"

#include "hash.h"

test()

  describe("hash")
    describe("hash_digest")
      it("should hash \"secret\" correctly")
        hash_t digest = hash_digest("secret", 6);

        assert_equal(digest, 0xAB23F0EEC020C951ULL);
      end()

      it("should hash \"password\" correctly")
        hash_t digest = hash_digest("password", 8);

        assert_equal(digest, 0x4B1A493507B3A318ULL);
      end()

      it("should hash \"The quick brown fox jumps over the lazy dog\" correctly")
        hash_t digest = hash_digest("The quick brown fox jumps over the lazy dog", 43);

        assert_equal(digest, 0xF3F9B7F5E7E47110ULL);
      end()
    end()
  end()

end()
