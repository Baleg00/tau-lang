#include "test.h"

#include "opcode.h"
#include "register.h"
#include "vm.h"
#include "tasm.h"

test()

  vm_t vm;

  describe("vm")
    describe("registers")
      before()
        vm_init(&vm, NULL, 0);
      end()

      after()
        vm_free(&vm);
      end()

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
      const uint8_t code[] = {
        0x01,
        0x02, 0x03,
        0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
        0x00, 0x00, 0x00, 0x40,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x40
      };

      before()
        vm_init(&vm, code, sizeof(code));
      end()

      after()
        vm_free(&vm);
      end()

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
      const uint8_t mem_readonly[] = {
        0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88,
        0x00, 0x00, 0x80, 0x40,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x14, 0x40
      };

      uint8_t mem[8];

      before()
        vm_init(&vm, NULL, 0);
      end()

      after()
        vm_free(&vm);
      end()

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

    describe("stack")
      before()
        vm_init(&vm, NULL, 0);
      end()

      after()
        vm_free(&vm);
      end()

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

    describe("tasm")      
      it("should calculate fibonacci numbers")
        uint8_t code[128];
        uint8_t* ptr = code;

        uint64_t label_loop;
        uint64_t label_finish;
        uint8_t* reference_finish;

        // MOV qword rf, 5
        ptr += tasm_write_opcode(ptr, OPCODE_MOV, OPCODE_PARAM_REG_IMM, OPCODE_WIDTH_64BIT);
        ptr += tasm_write_register(ptr, REG_F);
        ptr += tasm_write_u64(ptr, 5);

        // MOV qword ra, 0
        ptr += tasm_write_opcode(ptr, OPCODE_MOV, OPCODE_PARAM_REG_IMM, OPCODE_WIDTH_64BIT);
        ptr += tasm_write_register(ptr, REG_A);
        ptr += tasm_write_u64(ptr, 0);

        // MOV qword rb, 1
        ptr += tasm_write_opcode(ptr, OPCODE_MOV, OPCODE_PARAM_REG_IMM, OPCODE_WIDTH_64BIT);
        ptr += tasm_write_register(ptr, REG_B);
        ptr += tasm_write_u64(ptr, 1);

        // :loop
        label_loop = (uint64_t)(ptr - code);

        // CMP qword rf, 0
        ptr += tasm_write_opcode(ptr, OPCODE_CMP, OPCODE_PARAM_REG_IMM, OPCODE_WIDTH_64BIT);
        ptr += tasm_write_register(ptr, REG_F);
        ptr += tasm_write_u64(ptr, 0);

        // JZ :finish
        ptr += tasm_write_opcode(ptr, OPCODE_JE, OPCODE_PARAM_LABEL, OPCODE_WIDTH_64BIT);
        reference_finish = ptr;
        ptr += tasm_write_u64(ptr, 0 /* :finish */);

        // DEC qword rf
        ptr += tasm_write_opcode(ptr, OPCODE_DEC, OPCODE_PARAM_REG, OPCODE_WIDTH_64BIT);
        ptr += tasm_write_register(ptr, REG_F);

        // ADD qword ra, rb
        ptr += tasm_write_opcode(ptr, OPCODE_ADD, OPCODE_PARAM_REG_REG, OPCODE_WIDTH_64BIT);
        ptr += tasm_write_registers(ptr, REG_A, REG_B);

        // MOV qword rc, ra
        ptr += tasm_write_opcode(ptr, OPCODE_MOV, OPCODE_PARAM_REG_REG, OPCODE_WIDTH_64BIT);
        ptr += tasm_write_registers(ptr, REG_C, REG_A);

        // MOV qword ra, rb
        ptr += tasm_write_opcode(ptr, OPCODE_MOV, OPCODE_PARAM_REG_REG, OPCODE_WIDTH_64BIT);
        ptr += tasm_write_registers(ptr, REG_A, REG_B);

        // MOV qword rb, rc
        ptr += tasm_write_opcode(ptr, OPCODE_MOV, OPCODE_PARAM_REG_REG, OPCODE_WIDTH_64BIT);
        ptr += tasm_write_registers(ptr, REG_B, REG_C);

        // JMP :loop
        ptr += tasm_write_opcode(ptr, OPCODE_JMP, OPCODE_PARAM_LABEL, OPCODE_WIDTH_64BIT);
        ptr += tasm_write_u64(ptr, label_loop /* :loop */);

        // :finish
        label_finish = (uint64_t)(ptr - code);
        memcpy(reference_finish, &label_finish, sizeof(uint64_t));

        // HLT
        ptr += tasm_write_opcode(ptr, OPCODE_HLT, OPCODE_PARAM_NONE, OPCODE_WIDTH_NONE);

        vm_init(&vm, code, sizeof(code));

        vm_run(&vm);

        assert_equal(vm_register_u64_get(&vm, REG_B), 8);

        vm_free(&vm);
      end()

      it("should determine if a number is prime")
        uint8_t code[128];
        uint8_t* ptr = code;

        uint64_t label_loop;
        uint64_t label_prime;
        uint64_t label_finish;

        uint8_t* reference_prime;
        uint8_t* reference_finish;

        // MOV dword ral, 19  ; number is 19
        ptr += tasm_write_opcode(ptr, OPCODE_MOV, OPCODE_PARAM_REG_IMM, OPCODE_WIDTH_32BIT);
        ptr += tasm_write_register(ptr, REG_ALD);
        ptr += tasm_write_u32(ptr, 19);
        
        // MOV dword rbl, 2   ; divisor is 2
        ptr += tasm_write_opcode(ptr, OPCODE_MOV, OPCODE_PARAM_REG_IMM, OPCODE_WIDTH_32BIT);
        ptr += tasm_write_register(ptr, REG_BLD);
        ptr += tasm_write_u32(ptr, 2);

        // :loop
        label_loop = (uint64_t)(ptr - code);

        // MOV dword rcl, rbl ; move divisor into rcl
        ptr += tasm_write_opcode(ptr, OPCODE_MOV, OPCODE_PARAM_REG_REG, OPCODE_WIDTH_32BIT);
        ptr += tasm_write_registers(ptr, REG_CLD, REG_BLD);

        // MUL dword rcl, rcl ; square divisor
        ptr += tasm_write_opcode(ptr, OPCODE_MUL, OPCODE_PARAM_REG_REG, OPCODE_WIDTH_32BIT);
        ptr += tasm_write_registers(ptr, REG_CLD, REG_CLD);
        
        // CMP dword ral, rcl ; compare number and square of divisor
        ptr += tasm_write_opcode(ptr, OPCODE_CMP, OPCODE_PARAM_REG_REG, OPCODE_WIDTH_32BIT);
        ptr += tasm_write_registers(ptr, REG_ALD, REG_CLD);
        
        // JC :prime          ; if number is less than square of divisor then
        //                    ; number is prime
        ptr += tasm_write_opcode(ptr, OPCODE_JC, OPCODE_PARAM_LABEL, OPCODE_WIDTH_64BIT);
        reference_prime = ptr;
        ptr += tasm_write_u64(ptr, 0 /* :prime */);

        // MOV dword rcl, ral ; move number into rcl
        ptr += tasm_write_opcode(ptr, OPCODE_MOV, OPCODE_PARAM_REG_REG, OPCODE_WIDTH_32BIT);
        ptr += tasm_write_registers(ptr, REG_CLD, REG_ALD);

        // MOD dword rcl, rbl ; modulo number and divisor
        ptr += tasm_write_opcode(ptr, OPCODE_MOD, OPCODE_PARAM_REG_REG, OPCODE_WIDTH_32BIT);
        ptr += tasm_write_registers(ptr, REG_CLD, REG_BLD);

        // JZ :finish         ; if remainder is zero then number is not prime,
        //                    ; jump to finish
        ptr += tasm_write_opcode(ptr, OPCODE_JZ, OPCODE_PARAM_LABEL, OPCODE_WIDTH_64BIT);
        reference_finish = ptr;
        ptr += tasm_write_u64(ptr, 0 /* :finish */);
        
        // INC dword rbl      ; increment divisor
        ptr += tasm_write_opcode(ptr, OPCODE_INC, OPCODE_PARAM_REG, OPCODE_WIDTH_32BIT);
        ptr += tasm_write_register(ptr, REG_BLD);

        // JMP :loop          ; jump back to loop
        ptr += tasm_write_opcode(ptr, OPCODE_JMP, OPCODE_PARAM_LABEL, OPCODE_WIDTH_32BIT);
        ptr += tasm_write_u64(ptr, label_loop);

        // :prime
        label_prime = (uint64_t)(ptr - code);
        memcpy(reference_prime, &label_prime, sizeof(uint64_t));

        // MOV dword rfl, 1   ; move 1 into rfl indicating that number is prime
        ptr += tasm_write_opcode(ptr, OPCODE_MOV, OPCODE_PARAM_REG_IMM, OPCODE_WIDTH_32BIT);
        ptr += tasm_write_register(ptr, REG_FLD);
        ptr += tasm_write_u32(ptr, 1);

        // :finish
        label_finish = (uint64_t)(ptr - code);
        memcpy(reference_finish, &label_finish, sizeof(uint64_t));

        // HLT                ; halt
        ptr += tasm_write_opcode(ptr, OPCODE_HLT, OPCODE_PARAM_NONE, OPCODE_WIDTH_NONE);

        vm_init(&vm, code, sizeof(code));

        vm_run(&vm);

        assert_equal(vm_register_u32_get(&vm, REG_FLD), 1);

        vm_free(&vm);
      end()
    end()
  end()

end()
