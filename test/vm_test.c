#include "test.h"

#include "vm.h"

test()

  vm_t vm;
  
  const uint8_t code[] = {
    0x01,
    0x02, 0x03,
    0x04, 0x05, 0x06, 0x07,
    0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
    0x00, 0x00, 0x00, 0x40,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x40
  };

  const uint8_t mem_readonly[] = {
    0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88,
    0x00, 0x00, 0x80, 0x40,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x14, 0x40
  };

  uint8_t mem[8];

  before()
    vm_init(&vm, code, sizeof(code));
  end()

  after()
    vm_free(&vm);
  end()

  describe("vm")
    describe("registers")
      it("should set and get unsigned 8-bit register values")
        vm_register_u8_set(&vm, REG_ALB, 0xAB);
        assert_equal(vm_register_u8_get(&vm, REG_ALB), 0xAB);
      end()

      it("should set and get unsigned 16-bit register values")
        vm_register_u16_set(&vm, REG_BLW, 0xABCD);
        assert_equal(vm_register_u16_get(&vm, REG_BLW), 0xABCD);
      end()

      it("should set and get unsigned 32-bit register values")
        vm_register_u32_set(&vm, REG_CLD, 0x12345678);
        assert_equal(vm_register_u32_get(&vm, REG_CLD), 0x12345678);
      end()

      it("should set and get unsigned 64-bit register values")
        vm_register_u64_set(&vm, REG_D, 0x1234567890ABCDEF);
        assert_equal(vm_register_u64_get(&vm, REG_D), 0x1234567890ABCDEF);
      end()

      it("should set and get 32-bit floating-point register values")
        vm_register_f32_set(&vm, REG_ELD, 3.14159f);
        assert_equal(vm_register_f32_get(&vm, REG_ELD), 3.14159f);
      end()

      it("should set and get 64-bit floating-point register values")
        vm_register_f64_set(&vm, REG_F, 3.141592653589793);
        assert_equal(vm_register_f64_get(&vm, REG_F), 3.141592653589793);
      end()
    end()

    describe("code")
      it("should retrieve the next unsigned 8-bit value from the code")
        uint8_t value = vm_code_next_u8(&vm);
        assert_equal(value, 0x01);
      end()

      it("should retrieve the next unsigned 16-bit value from the code")
        uint16_t value = vm_code_next_u16(&vm);
        assert_equal(value, 0x0302);
      end()

      it("should retrieve the next unsigned 32-bit value from the code")
        uint32_t value = vm_code_next_u32(&vm);
        assert_equal(value, 0x07060504);
      end()

      it("should retrieve the next unsigned 64-bit value from the code")
        uint64_t value = vm_code_next_u64(&vm);
        assert_equal(value, 0x0F0E0D0C0B0A0908);
      end()

      it("should retrieve the next 32-bit floating-point value from the code")
        float value = vm_code_next_f32(&vm);
        assert_equal(value, 2.0f);
      end()

      it("should retrieve the next 64-bit floating-point value from the code")
        double value = vm_code_next_f64(&vm);
        assert_equal(value, 3.0);
      end()
    end()

    describe("memory")
      it("should get unsigned 8-bit values from memory")
        uint8_t value = vm_mem_u8_get(&vm, &mem_readonly[0]);
        assert_equal(value, 0x11);
      end()

      it("should get unsigned 16-bit values from memory")
        uint16_t value = vm_mem_u16_get(&vm, &mem_readonly[0]);
        assert_equal(value, 0x2211);
      end()

      it("should get unsigned 32-bit values from memory")
        uint32_t value = vm_mem_u32_get(&vm, &mem_readonly[0]);
        assert_equal(value, 0x44332211);
      end()

      it("should get unsigned 64-bit values from memory")
        uint64_t value = vm_mem_u64_get(&vm, &mem_readonly[0]);
        assert_equal(value, 0x8877665544332211);
      end()

      it("should get 32-bit floating-point values from memory")
        float value = vm_mem_f32_get(&vm, &mem_readonly[8]);
        assert_equal(value, 4.0f);
      end()

      it("should get 64-bit floating-point values from memory")
        double value = vm_mem_f64_get(&vm, &mem_readonly[12]);
        assert_equal(value, 5.0);
      end()

      it("should set unsigned 8-bit values to memory")
        vm_mem_u8_set(&vm, &mem[0], 0xAA);
        assert_equal(mem[0], 0xAA);
      end()

      it("should set unsigned 16-bit values to memory")
        vm_mem_u16_set(&vm, &mem[0], 0xAABB);
        assert_equal(mem[0], 0xBB);
        assert_equal(mem[1], 0xAA);
      end()

      it("should set unsigned 32-bit values to memory")
        vm_mem_u32_set(&vm, &mem[0], 0xAABBCCDD);
        assert_equal(mem[0], 0xDD);
        assert_equal(mem[1], 0xCC);
        assert_equal(mem[2], 0xBB);
        assert_equal(mem[3], 0xAA);
      end()

      it("should set unsigned 64-bit values to memory")
        vm_mem_u64_set(&vm, &mem[0], 0xAABBCCDDEEFF0011);
        assert_equal(mem[0], 0x11);
        assert_equal(mem[1], 0x00);
        assert_equal(mem[2], 0xFF);
        assert_equal(mem[3], 0xEE);
        assert_equal(mem[4], 0xDD);
        assert_equal(mem[5], 0xCC);
        assert_equal(mem[6], 0xBB);
        assert_equal(mem[7], 0xAA);
      end()

      it("should set 32-bit floating-point values to memory")
        vm_mem_f32_set(&vm, &mem[0], 1.234f);
        assert_equal(mem[0], 0xB6);
        assert_equal(mem[1], 0xF3);
        assert_equal(mem[2], 0x9D);
        assert_equal(mem[3], 0x3F);
      end()

      it("should set 64-bit floating-point values to memory")
        vm_mem_f64_set(&vm, &mem[0], 2.345);
        assert_equal(mem[0], 0xC3);
        assert_equal(mem[1], 0xF5);
        assert_equal(mem[2], 0x28);
        assert_equal(mem[3], 0x5C);
        assert_equal(mem[4], 0x8F);
        assert_equal(mem[5], 0xC2);
        assert_equal(mem[6], 0x02);
        assert_equal(mem[7], 0x40);
      end()
    end()

    describe("addressing")
      it("should encode and decode ADDR_MODE_OFFSET")
        addr_mode_t mode = ADDR_MODE_OFFSET;
        int64_t offset = 123;

        uint8_t mem[16];
        size_t encoded_size = vm_mem_addr_encode(&vm, mem, mode, 0, 0, 0, offset);

        addr_mode_t decoded_mode;
        int64_t decoded_offset;

        size_t decoded_size = vm_mem_addr_decode(&vm, mem, &decoded_mode, NULL, NULL, NULL, &decoded_offset);

        assert_equal(encoded_size, decoded_size);
        assert_equal(decoded_mode, mode);
        assert_equal(decoded_offset, offset);
      end()

      it("should encode and decode ADDR_MODE_BASE")
        addr_mode_t mode = ADDR_MODE_BASE;
        register_t base = REG_A;

        uint8_t mem[16];
        size_t encoded_size = vm_mem_addr_encode(&vm, mem, mode, base, 0, 0, 0);

        addr_mode_t decoded_mode;
        register_t decoded_base;

        size_t decoded_size = vm_mem_addr_decode(&vm, mem, &decoded_mode, &decoded_base, NULL, NULL, NULL);

        assert_equal(encoded_size, decoded_size);
        assert_equal(decoded_mode, mode);
        assert_equal(decoded_base, base);
      end()

      it("should encode and decode ADDR_MODE_BASE_OFFSET")
        addr_mode_t mode = ADDR_MODE_BASE_OFFSET;
        register_t base = REG_B;
        int64_t offset = 456;

        uint8_t mem[16];
        size_t encoded_size = vm_mem_addr_encode(&vm, mem, mode, base, 0, 0, offset);

        addr_mode_t decoded_mode;
        register_t decoded_base;
        int64_t decoded_offset;

        size_t decoded_size = vm_mem_addr_decode(&vm, mem, &decoded_mode, &decoded_base, NULL, NULL, &decoded_offset);

        assert_equal(encoded_size, decoded_size);
        assert_equal(decoded_mode, mode);
        assert_equal(decoded_base, base);
        assert_equal(decoded_offset, offset);
      end()

      it("should encode and decode ADDR_MODE_BASE_INDEX")
        addr_mode_t mode = ADDR_MODE_BASE_INDEX;
        register_t base = REG_C;
        register_t index = REG_D;

        uint8_t mem[16];
        size_t encoded_size = vm_mem_addr_encode(&vm, mem, mode, base, index, 0, 0);

        addr_mode_t decoded_mode;
        register_t decoded_base;
        register_t decoded_index;

        size_t decoded_size = vm_mem_addr_decode(&vm, mem, &decoded_mode, &decoded_base, &decoded_index, NULL, NULL);

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

        uint8_t mem[16];
        size_t encoded_size = vm_mem_addr_encode(&vm, mem, mode, base, index, 0, offset);

        addr_mode_t decoded_mode;
        register_t decoded_base;
        register_t decoded_index;
        int64_t decoded_offset;

        size_t decoded_size = vm_mem_addr_decode(&vm, mem, &decoded_mode, &decoded_base, &decoded_index, NULL, &decoded_offset);

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

        uint8_t mem[16];
        size_t encoded_size = vm_mem_addr_encode(&vm, mem, mode, base, index, scale, 0);

        addr_mode_t decoded_mode;
        register_t decoded_base;
        register_t decoded_index;
        int32_t decoded_scale;

        size_t decoded_size = vm_mem_addr_decode(&vm, mem, &decoded_mode, &decoded_base, &decoded_index, &decoded_scale, NULL);

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

        uint8_t mem[16];
        size_t encoded_size = vm_mem_addr_encode(&vm, mem, mode, 0, index, scale, offset);

        addr_mode_t decoded_mode;
        register_t decoded_index;
        int32_t decoded_scale;
        int64_t decoded_offset;

        size_t decoded_size = vm_mem_addr_decode(&vm, mem, &decoded_mode, NULL, &decoded_index, &decoded_scale, &decoded_offset);

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

        uint8_t mem[16];
        size_t encoded_size = vm_mem_addr_encode(&vm, mem, mode, base, index, scale, offset);

        addr_mode_t decoded_mode;
        register_t decoded_base;
        register_t decoded_index;
        int32_t decoded_scale;
        int64_t decoded_offset;

        size_t decoded_size = vm_mem_addr_decode(&vm, mem, &decoded_mode, &decoded_base, &decoded_index, &decoded_scale, &decoded_offset);

        assert_equal(encoded_size, decoded_size);
        assert_equal(decoded_mode, mode);
        assert_equal(decoded_base, base);
        assert_equal(decoded_index, index);
        assert_equal(decoded_scale, scale);
        assert_equal(decoded_offset, offset);
      end()
    end()

    describe("stack")
      it("should push and pop unsigned 8-bit values to/from the stack")
        vm_stack_u8_push(&vm, 0x11);
        vm_stack_u8_push(&vm, 0x22);
        assert_equal(vm_stack_u8_pop(&vm), 0x22);
        assert_equal(vm_stack_u8_pop(&vm), 0x11);
      end()

      it("should push and pop unsigned 16-bit values to/from the stack")
        vm_stack_u16_push(&vm, 0x1122);
        vm_stack_u16_push(&vm, 0x3344);
        assert_equal(vm_stack_u16_pop(&vm), 0x3344);
        assert_equal(vm_stack_u16_pop(&vm), 0x1122);
      end()

      it("should push and pop unsigned 32-bit values to/from the stack")
        vm_stack_u32_push(&vm, 0x11223344);
        vm_stack_u32_push(&vm, 0x55667788);
        assert_equal(vm_stack_u32_pop(&vm), 0x55667788);
        assert_equal(vm_stack_u32_pop(&vm), 0x11223344);
      end()

      it("should push and pop unsigned 64-bit values to/from the stack")
        vm_stack_u64_push(&vm, 0x1122334455667788);
        vm_stack_u64_push(&vm, 0xAABBCCDDEEFF0011);
        assert_equal(vm_stack_u64_pop(&vm), 0xAABBCCDDEEFF0011);
        assert_equal(vm_stack_u64_pop(&vm), 0x1122334455667788);
      end()

      it("should push and pop 32-bit floating-point values to/from the stack")
        vm_stack_f32_push(&vm, 1.23f);
        vm_stack_f32_push(&vm, 4.56f);
        assert_equal(vm_stack_f32_pop(&vm), 4.56f);
        assert_equal(vm_stack_f32_pop(&vm), 1.23f);
      end()

      it("should push and pop 64-bit floating-point values to/from the stack")
        vm_stack_f64_push(&vm, 1.234);
        vm_stack_f64_push(&vm, 4.567);
        assert_equal(vm_stack_f64_pop(&vm), 4.567);
        assert_equal(vm_stack_f64_pop(&vm), 1.234);
      end()
    end()
  end()

end()
