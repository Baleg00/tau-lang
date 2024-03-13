#include "test.h"

#include "utils/hash.h"

test()

  describe("hash")
    describe("hash_digest")
      it("should hash \"secret\" correctly")
        uint64_t digest = hash_digest("secret", 6);

        assert_equal(digest, 0xAB23F0EEC020C951ULL);
      end()

      it("should hash \"password\" correctly")
        uint64_t digest = hash_digest("password", 8);

        assert_equal(digest, 0x4B1A493507B3A318ULL);
      end()

      it("should hash \"The quick brown fox jumps over the lazy dog\" correctly")
        uint64_t digest = hash_digest("The quick brown fox jumps over the lazy dog", 43);

        assert_equal(digest, 0xF3F9B7F5E7E47110ULL);
      end()
    end()

    describe("hash_combine_with_data")
      it("should combine \"secret\" and \"password\" correctly")
        uint64_t seed = hash_digest("secret", 6);
        uint64_t combined = hash_combine_with_data(seed, "password", 8);

        assert_equal(combined, 0x95FC71CF9E056A34ULL);
      end()
    end()

    describe("hash_combine_with_hash")
      it("should combine \"secret\" and \"password\" correctly")
        uint64_t seed = hash_digest("secret", 6);
        uint64_t hash = hash_digest("password", 8);
        uint64_t combined = hash_combine_with_hash(seed, hash);

        assert_equal(combined, 0x95FC71CF9E056A34ULL);
      end()
    end()
  end()

end()
