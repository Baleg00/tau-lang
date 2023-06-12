#include "test.h"

#include "opcode.h"

test()

  uint16_t encoded_opcode;
  opcode_t decoded_opcode;
  opcode_param_t decoded_param;
  opcode_width_t decoded_width;

  describe("opcode")
    describe("opcode_encode, opcode_decode")
      it("should encode and decode opcode with no parameters")
        uint16_t encoded_opcode = opcode_encode(OPCODE_NOP, OPCODE_PARAM_NONE, OPCODE_WIDTH_NONE);
        opcode_decode(encoded_opcode, &decoded_opcode, &decoded_param, &decoded_width);

        assert_equal(decoded_opcode, OPCODE_NOP);
        assert_equal(decoded_param, OPCODE_PARAM_NONE);
        assert_equal(decoded_width, OPCODE_WIDTH_NONE);
      end()

      it("should encode and decode opcode with 1 parameter")
        uint16_t encoded_opcode = opcode_encode(OPCODE_INEG, OPCODE_PARAM_REG, OPCODE_WIDTH_64BIT);
        opcode_decode(encoded_opcode, &decoded_opcode, &decoded_param, &decoded_width);

        assert_equal(decoded_opcode, OPCODE_INEG);
        assert_equal(decoded_param, OPCODE_PARAM_REG);
        assert_equal(decoded_width, OPCODE_WIDTH_64BIT);
      end()

      it("should encode and decode opcode with 2 parameters")
        uint16_t encoded_opcode = opcode_encode(OPCODE_IADD, OPCODE_PARAM_REG_REG, OPCODE_WIDTH_64BIT);
        opcode_decode(encoded_opcode, &decoded_opcode, &decoded_param, &decoded_width);

        assert_equal(decoded_opcode, OPCODE_IADD);
        assert_equal(decoded_param, OPCODE_PARAM_REG_REG);
        assert_equal(decoded_width, OPCODE_WIDTH_64BIT);
      end()
    end()
  end()

end()
