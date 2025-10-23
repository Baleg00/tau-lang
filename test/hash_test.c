#include "test.h"

#include "utils/hash.h"

TEST_CASE(tau_hash_secret)
{
  uint64_t digest = tau_hash_digest("secret", 6);

  TEST_ASSERT_EQUAL(digest, 0xAB23F0EEC020C951ULL);
}

TEST_CASE(tau_hash_password)
{
  uint64_t digest = tau_hash_digest("password", 8);

  TEST_ASSERT_EQUAL(digest, 0x4B1A493507B3A318ULL);
}

TEST_CASE(tau_hash_quick_brown_fox)
{
  uint64_t digest = tau_hash_digest("The quick brown fox jumps over the lazy dog", 43);

  TEST_ASSERT_EQUAL(digest, 0xF3F9B7F5E7E47110ULL);
}

TEST_CASE(tau_hash_combine_with_data)
{
  uint64_t seed = tau_hash_digest("secret", 6);
  uint64_t combined = tau_hash_combine_with_data(seed, "password", 8);

  TEST_ASSERT_EQUAL(combined, 0x95FC71CF9E056A34ULL);
}

TEST_CASE(tau_hash_combine_with_hash)
{
  uint64_t seed = tau_hash_digest("secret", 6);
  uint64_t hash = tau_hash_digest("password", 8);
  uint64_t combined = tau_hash_combine_with_hash(seed, hash);

  TEST_ASSERT_EQUAL(combined, 0x95FC71CF9E056A34ULL);
}

TEST_MAIN()
{
  TEST_RUN(tau_hash_secret);
  TEST_RUN(tau_hash_password);
  TEST_RUN(tau_hash_quick_brown_fox);
  TEST_RUN(tau_hash_combine_with_data);
  TEST_RUN(tau_hash_combine_with_hash);
}
