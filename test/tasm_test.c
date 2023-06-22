#include "test.h"

#include "tasm.h"
#include "vm.h"

test()

  describe("tasm")
    describe("addressing")
      uint8_t mem[16];

      it("should encode and decode ADDR_MODE_OFFSET")
        addr_mode_t mode = ADDR_MODE_OFFSET;
        int64_t offset = 123;

        size_t encoded_size = tasm_addr_encode(mem, mode, 0, 0, 0, offset);

        addr_mode_t decoded_mode;
        int64_t decoded_offset;

        size_t decoded_size = tasm_addr_decode(mem, &decoded_mode, NULL, NULL, NULL, &decoded_offset);

        assert_equal(encoded_size, decoded_size);
        assert_equal(decoded_mode, mode);
        assert_equal(decoded_offset, offset);
      end()

      it("should encode and decode ADDR_MODE_BASE")
        addr_mode_t mode = ADDR_MODE_BASE;
        register_t base = REG_A;

        size_t encoded_size = tasm_addr_encode(mem, mode, base, 0, 0, 0);

        addr_mode_t decoded_mode;
        register_t decoded_base;

        size_t decoded_size = tasm_addr_decode(mem, &decoded_mode, &decoded_base, NULL, NULL, NULL);

        assert_equal(encoded_size, decoded_size);
        assert_equal(decoded_mode, mode);
        assert_equal(decoded_base, base);
      end()

      it("should encode and decode ADDR_MODE_BASE_OFFSET")
        addr_mode_t mode = ADDR_MODE_BASE_OFFSET;
        register_t base = REG_B;
        int64_t offset = 456;

        size_t encoded_size = tasm_addr_encode(mem, mode, base, 0, 0, offset);

        addr_mode_t decoded_mode;
        register_t decoded_base;
        int64_t decoded_offset;

        size_t decoded_size = tasm_addr_decode(mem, &decoded_mode, &decoded_base, NULL, NULL, &decoded_offset);

        assert_equal(encoded_size, decoded_size);
        assert_equal(decoded_mode, mode);
        assert_equal(decoded_base, base);
        assert_equal(decoded_offset, offset);
      end()

      it("should encode and decode ADDR_MODE_BASE_INDEX")
        addr_mode_t mode = ADDR_MODE_BASE_INDEX;
        register_t base = REG_C;
        register_t index = REG_D;

        size_t encoded_size = tasm_addr_encode(mem, mode, base, index, 0, 0);

        addr_mode_t decoded_mode;
        register_t decoded_base;
        register_t decoded_index;

        size_t decoded_size = tasm_addr_decode(mem, &decoded_mode, &decoded_base, &decoded_index, NULL, NULL);

        assert_equal(encoded_size, decoded_size);
        assert_equal(decoded_mode, mode);
        assert_equal(decoded_base, base);
        assert_equal(decoded_index, index);
      end()

      it("should encode and decode ADDR_MODE_BASE_INDEX_OFFSET")
        addr_mode_t mode = ADDR_MODE_BASE_INDEX_OFFSET;
        register_t base = REG_E;
        register_t index = REG_F;
        int64_t offset = 789;

        size_t encoded_size = tasm_addr_encode(mem, mode, base, index, 0, offset);

        addr_mode_t decoded_mode;
        register_t decoded_base;
        register_t decoded_index;
        int64_t decoded_offset;

        size_t decoded_size = tasm_addr_decode(mem, &decoded_mode, &decoded_base, &decoded_index, NULL, &decoded_offset);

        assert_equal(encoded_size, decoded_size);
        assert_equal(decoded_mode, mode);
        assert_equal(decoded_base, base);
        assert_equal(decoded_index, index);
        assert_equal(decoded_offset, offset);
      end()

      it("should encode and decode ADDR_MODE_BASE_INDEX_SCALE")
        addr_mode_t mode = ADDR_MODE_BASE_INDEX_SCALE;
        register_t base = REG_A;
        register_t index = REG_B;
        int32_t scale = 64;

        size_t encoded_size = tasm_addr_encode(mem, mode, base, index, scale, 0);

        addr_mode_t decoded_mode;
        register_t decoded_base;
        register_t decoded_index;
        int32_t decoded_scale;

        size_t decoded_size = tasm_addr_decode(mem, &decoded_mode, &decoded_base, &decoded_index, &decoded_scale, NULL);

        assert_equal(encoded_size, decoded_size);
        assert_equal(decoded_mode, mode);
        assert_equal(decoded_base, base);
        assert_equal(decoded_index, index);
        assert_equal(decoded_scale, scale);
      end()

      it("should encode and decode ADDR_MODE_INDEX_SCALE_OFFSET")
        addr_mode_t mode = ADDR_MODE_INDEX_SCALE_OFFSET;
        register_t index = REG_C;
        int32_t scale = -128;
        int64_t offset = -123;

        size_t encoded_size = tasm_addr_encode(mem, mode, 0, index, scale, offset);

        addr_mode_t decoded_mode;
        register_t decoded_index;
        int32_t decoded_scale;
        int64_t decoded_offset;

        size_t decoded_size = tasm_addr_decode(mem, &decoded_mode, NULL, &decoded_index, &decoded_scale, &decoded_offset);

        assert_equal(encoded_size, decoded_size);
        assert_equal(decoded_mode, mode);
        assert_equal(decoded_index, index);
        assert_equal(decoded_scale, scale);
        assert_equal(decoded_offset, offset);
      end()

      it("should encode and decode ADDR_MODE_BASE_INDEX_SCALE_OFFSET")
        addr_mode_t mode = ADDR_MODE_BASE_INDEX_SCALE_OFFSET;
        register_t base = REG_D;
        register_t index = REG_E;
        int32_t scale = 128;
        int64_t offset = -456;

        size_t encoded_size = tasm_addr_encode(mem, mode, base, index, scale, offset);

        addr_mode_t decoded_mode;
        register_t decoded_base;
        register_t decoded_index;
        int32_t decoded_scale;
        int64_t decoded_offset;

        size_t decoded_size = tasm_addr_decode(mem, &decoded_mode, &decoded_base, &decoded_index, &decoded_scale, &decoded_offset);

        assert_equal(encoded_size, decoded_size);
        assert_equal(decoded_mode, mode);
        assert_equal(decoded_base, base);
        assert_equal(decoded_index, index);
        assert_equal(decoded_scale, scale);
        assert_equal(decoded_offset, offset);
      end()
    end()
  end()

end()
