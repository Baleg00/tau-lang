#include "test.h"

#include "register.h"

test()

  uint8_t encoded_reg;
  register_t decoded_reg;

  describe("register")
    describe("register_encode, register_decode")
      it("should encode and decode 8 bit registers")
        encoded_reg = register_encode(REG_ALB);
        decoded_reg = register_decode(encoded_reg, OPCODE_WIDTH_8BIT);
        assert_equal(decoded_reg, REG_ALB);

        encoded_reg = register_encode(REG_AHB);
        decoded_reg = register_decode(encoded_reg, OPCODE_WIDTH_8BIT);
        assert_equal(decoded_reg, REG_AHB);

        encoded_reg = register_encode(REG_BLB);
        decoded_reg = register_decode(encoded_reg, OPCODE_WIDTH_8BIT);
        assert_equal(decoded_reg, REG_BLB);

        encoded_reg = register_encode(REG_BHB);
        decoded_reg = register_decode(encoded_reg, OPCODE_WIDTH_8BIT);
        assert_equal(decoded_reg, REG_BHB);

        encoded_reg = register_encode(REG_CLB);
        decoded_reg = register_decode(encoded_reg, OPCODE_WIDTH_8BIT);
        assert_equal(decoded_reg, REG_CLB);

        encoded_reg = register_encode(REG_CHB);
        decoded_reg = register_decode(encoded_reg, OPCODE_WIDTH_8BIT);
        assert_equal(decoded_reg, REG_CHB);

        encoded_reg = register_encode(REG_DLB);
        decoded_reg = register_decode(encoded_reg, OPCODE_WIDTH_8BIT);
        assert_equal(decoded_reg, REG_DLB);

        encoded_reg = register_encode(REG_DHB);
        decoded_reg = register_decode(encoded_reg, OPCODE_WIDTH_8BIT);
        assert_equal(decoded_reg, REG_DHB);

        encoded_reg = register_encode(REG_ELB);
        decoded_reg = register_decode(encoded_reg, OPCODE_WIDTH_8BIT);
        assert_equal(decoded_reg, REG_ELB);

        encoded_reg = register_encode(REG_EHB);
        decoded_reg = register_decode(encoded_reg, OPCODE_WIDTH_8BIT);
        assert_equal(decoded_reg, REG_EHB);

        encoded_reg = register_encode(REG_FLB);
        decoded_reg = register_decode(encoded_reg, OPCODE_WIDTH_8BIT);
        assert_equal(decoded_reg, REG_FLB);

        encoded_reg = register_encode(REG_FHB);
        decoded_reg = register_decode(encoded_reg, OPCODE_WIDTH_8BIT);
        assert_equal(decoded_reg, REG_FHB);
      end()

      it("should encode and decode 16 bit registers")
        encoded_reg = register_encode(REG_ALW);
        decoded_reg = register_decode(encoded_reg, OPCODE_WIDTH_16BIT);
        assert_equal(decoded_reg, REG_ALW);

        encoded_reg = register_encode(REG_AHW);
        decoded_reg = register_decode(encoded_reg, OPCODE_WIDTH_16BIT);
        assert_equal(decoded_reg, REG_AHW);

        encoded_reg = register_encode(REG_BLW);
        decoded_reg = register_decode(encoded_reg, OPCODE_WIDTH_16BIT);
        assert_equal(decoded_reg, REG_BLW);

        encoded_reg = register_encode(REG_BHW);
        decoded_reg = register_decode(encoded_reg, OPCODE_WIDTH_16BIT);
        assert_equal(decoded_reg, REG_BHW);

        encoded_reg = register_encode(REG_CLW);
        decoded_reg = register_decode(encoded_reg, OPCODE_WIDTH_16BIT);
        assert_equal(decoded_reg, REG_CLW);

        encoded_reg = register_encode(REG_CHW);
        decoded_reg = register_decode(encoded_reg, OPCODE_WIDTH_16BIT);
        assert_equal(decoded_reg, REG_CHW);

        encoded_reg = register_encode(REG_DLW);
        decoded_reg = register_decode(encoded_reg, OPCODE_WIDTH_16BIT);
        assert_equal(decoded_reg, REG_DLW);

        encoded_reg = register_encode(REG_DHW);
        decoded_reg = register_decode(encoded_reg, OPCODE_WIDTH_16BIT);
        assert_equal(decoded_reg, REG_DHW);

        encoded_reg = register_encode(REG_ELW);
        decoded_reg = register_decode(encoded_reg, OPCODE_WIDTH_16BIT);
        assert_equal(decoded_reg, REG_ELW);

        encoded_reg = register_encode(REG_EHW);
        decoded_reg = register_decode(encoded_reg, OPCODE_WIDTH_16BIT);
        assert_equal(decoded_reg, REG_EHW);

        encoded_reg = register_encode(REG_FLW);
        decoded_reg = register_decode(encoded_reg, OPCODE_WIDTH_16BIT);
        assert_equal(decoded_reg, REG_FLW);

        encoded_reg = register_encode(REG_FHW);
        decoded_reg = register_decode(encoded_reg, OPCODE_WIDTH_16BIT);
        assert_equal(decoded_reg, REG_FHW);
      end()

      it("should encode and decode 32 bit registers")
        encoded_reg = register_encode(REG_ALD);
        decoded_reg = register_decode(encoded_reg, OPCODE_WIDTH_32BIT);
        assert_equal(decoded_reg, REG_ALD);

        encoded_reg = register_encode(REG_AHD);
        decoded_reg = register_decode(encoded_reg, OPCODE_WIDTH_32BIT);
        assert_equal(decoded_reg, REG_AHD);

        encoded_reg = register_encode(REG_BLD);
        decoded_reg = register_decode(encoded_reg, OPCODE_WIDTH_32BIT);
        assert_equal(decoded_reg, REG_BLD);

        encoded_reg = register_encode(REG_BHD);
        decoded_reg = register_decode(encoded_reg, OPCODE_WIDTH_32BIT);
        assert_equal(decoded_reg, REG_BHD);

        encoded_reg = register_encode(REG_CLD);
        decoded_reg = register_decode(encoded_reg, OPCODE_WIDTH_32BIT);
        assert_equal(decoded_reg, REG_CLD);

        encoded_reg = register_encode(REG_CHD);
        decoded_reg = register_decode(encoded_reg, OPCODE_WIDTH_32BIT);
        assert_equal(decoded_reg, REG_CHD);

        encoded_reg = register_encode(REG_DLD);
        decoded_reg = register_decode(encoded_reg, OPCODE_WIDTH_32BIT);
        assert_equal(decoded_reg, REG_DLD);

        encoded_reg = register_encode(REG_DHD);
        decoded_reg = register_decode(encoded_reg, OPCODE_WIDTH_32BIT);
        assert_equal(decoded_reg, REG_DHD);

        encoded_reg = register_encode(REG_ELD);
        decoded_reg = register_decode(encoded_reg, OPCODE_WIDTH_32BIT);
        assert_equal(decoded_reg, REG_ELD);

        encoded_reg = register_encode(REG_EHD);
        decoded_reg = register_decode(encoded_reg, OPCODE_WIDTH_32BIT);
        assert_equal(decoded_reg, REG_EHD);

        encoded_reg = register_encode(REG_FLD);
        decoded_reg = register_decode(encoded_reg, OPCODE_WIDTH_32BIT);
        assert_equal(decoded_reg, REG_FLD);

        encoded_reg = register_encode(REG_FHD);
        decoded_reg = register_decode(encoded_reg, OPCODE_WIDTH_32BIT);
        assert_equal(decoded_reg, REG_FHD);
      end()

      it("should encode and decode 64 bit registers")
        encoded_reg = register_encode(REG_A);
        decoded_reg = register_decode(encoded_reg, OPCODE_WIDTH_64BIT);
        assert_equal(decoded_reg, REG_A);

        encoded_reg = register_encode(REG_B);
        decoded_reg = register_decode(encoded_reg, OPCODE_WIDTH_64BIT);
        assert_equal(decoded_reg, REG_B);

        encoded_reg = register_encode(REG_C);
        decoded_reg = register_decode(encoded_reg, OPCODE_WIDTH_64BIT);
        assert_equal(decoded_reg, REG_C);

        encoded_reg = register_encode(REG_D);
        decoded_reg = register_decode(encoded_reg, OPCODE_WIDTH_64BIT);
        assert_equal(decoded_reg, REG_D);

        encoded_reg = register_encode(REG_E);
        decoded_reg = register_decode(encoded_reg, OPCODE_WIDTH_64BIT);
        assert_equal(decoded_reg, REG_E);

        encoded_reg = register_encode(REG_F);
        decoded_reg = register_decode(encoded_reg, OPCODE_WIDTH_64BIT);
        assert_equal(decoded_reg, REG_F);

        encoded_reg = register_encode(REG_SP);
        decoded_reg = register_decode(encoded_reg, OPCODE_WIDTH_64BIT);
        assert_equal(decoded_reg, REG_SP);

        encoded_reg = register_encode(REG_BP);
        decoded_reg = register_decode(encoded_reg, OPCODE_WIDTH_64BIT);
        assert_equal(decoded_reg, REG_BP);

        encoded_reg = register_encode(REG_IP);
        decoded_reg = register_decode(encoded_reg, OPCODE_WIDTH_64BIT);
        assert_equal(decoded_reg, REG_IP);
      end()
    end()
  end()

end()
