#include "test.h"

#include "stages/analysis/types/typedesc/typedesc.h"

test()

  describe("typedesc")
    describe("typedesc_remove")
      it("should remove mutable modifier")
        typedesc_mut_t* desc = typedesc_mut_init();
        typedesc_prim_t* base_desc = typedesc_prim_unit_init();

        desc->base_type = (typedesc_t*)base_desc;

        typedesc_t* removed = typedesc_remove_mut((typedesc_t*)desc);
        assert_ptr_equal(removed, base_desc);

        removed = typedesc_remove_mut((typedesc_t*)base_desc);
        assert_ptr_equal(removed, base_desc);

        typedesc_free((typedesc_t*)base_desc);
        typedesc_free((typedesc_t*)desc);
      end()

      it("should remove pointer modifier")
        typedesc_ptr_t* desc = typedesc_ptr_init();
        typedesc_prim_t* base_desc = typedesc_prim_unit_init();

        desc->base_type = (typedesc_t*)base_desc;

        typedesc_t* removed = typedesc_remove_ptr((typedesc_t*)desc);
        assert_ptr_equal(removed, base_desc);

        removed = typedesc_remove_ptr((typedesc_t*)base_desc);
        assert_ptr_equal(removed, base_desc);

        typedesc_free((typedesc_t*)base_desc);
        typedesc_free((typedesc_t*)desc);
      end()

      it("should remove array modifier")
        typedesc_array_t* desc = typedesc_array_init();
        typedesc_prim_t* base_desc = typedesc_prim_unit_init();

        desc->base_type = (typedesc_t*)base_desc;

        typedesc_t* removed = typedesc_remove_array((typedesc_t*)desc);
        assert_ptr_equal(removed, base_desc);

        removed = typedesc_remove_array((typedesc_t*)base_desc);
        assert_ptr_equal(removed, base_desc);

        typedesc_free((typedesc_t*)base_desc);
        typedesc_free((typedesc_t*)desc);
      end()

      it("should remove reference modifier")
        typedesc_ref_t* desc = typedesc_ref_init();
        typedesc_prim_t* base_desc = typedesc_prim_unit_init();

        desc->base_type = (typedesc_t*)base_desc;

        typedesc_t* removed = typedesc_remove_ref((typedesc_t*)desc);
        assert_ptr_equal(removed, base_desc);

        removed = typedesc_remove_ref((typedesc_t*)base_desc);
        assert_ptr_equal(removed, base_desc);

        typedesc_free((typedesc_t*)base_desc);
        typedesc_free((typedesc_t*)desc);
      end()

      it("should remove optional modifier")
        typedesc_opt_t* desc = typedesc_opt_init();
        typedesc_prim_t* base_desc = typedesc_prim_unit_init();

        desc->base_type = (typedesc_t*)base_desc;

        typedesc_t* removed = typedesc_remove_opt((typedesc_t*)desc);
        assert_ptr_equal(removed, base_desc);

        removed = typedesc_remove_opt((typedesc_t*)base_desc);
        assert_ptr_equal(removed, base_desc);

        typedesc_free((typedesc_t*)base_desc);
        typedesc_free((typedesc_t*)desc);
      end()

      it("should remove reference and mutable modifier")
        typedesc_ref_t* ref_desc = typedesc_ref_init();
        typedesc_mut_t* mut_desc = typedesc_mut_init();
        typedesc_prim_t* base_desc = typedesc_prim_unit_init();

        ref_desc->base_type = (typedesc_t*)mut_desc;
        mut_desc->base_type = (typedesc_t*)base_desc;

        typedesc_t* removed = typedesc_remove_ref_mut((typedesc_t*)ref_desc);
        assert_ptr_equal(removed, base_desc);

        removed = typedesc_remove_ref_mut((typedesc_t*)mut_desc);
        assert_ptr_equal(removed, base_desc);

        removed = typedesc_remove_ref_mut((typedesc_t*)base_desc);
        assert_ptr_equal(removed, base_desc);

        typedesc_free((typedesc_t*)base_desc);
        typedesc_free((typedesc_t*)mut_desc);
        typedesc_free((typedesc_t*)ref_desc);
      end()
    end()

    describe("typedesc_can_add")
      it("should be able to add mutable modifier")
        typedesc_ptr_t* ptr_desc = typedesc_ptr_init();
        typedesc_opt_t* opt_desc = typedesc_opt_init();
        typedesc_prim_t* unit_desc = typedesc_prim_unit_init();

        assert_true(typedesc_can_add_mut((typedesc_t*)ptr_desc));
        assert_true(typedesc_can_add_mut((typedesc_t*)opt_desc));
        assert_true(typedesc_can_add_mut((typedesc_t*)unit_desc));

        typedesc_free((typedesc_t*)unit_desc);
        typedesc_free((typedesc_t*)opt_desc);
        typedesc_free((typedesc_t*)ptr_desc);
      end()

      it("should not be able to add mutable modifier")
        typedesc_mut_t* mut_desc = typedesc_mut_init();
        typedesc_ref_t* ref_desc = typedesc_ref_init();

        assert_false(typedesc_can_add_mut((typedesc_t*)mut_desc));
        assert_false(typedesc_can_add_mut((typedesc_t*)ref_desc));

        typedesc_free((typedesc_t*)mut_desc);
        typedesc_free((typedesc_t*)ref_desc);
      end()
      
      it("should be able to add pointer modifier")
        typedesc_mut_t* mut_desc = typedesc_mut_init();
        typedesc_ptr_t* ptr_desc = typedesc_ptr_init();
        typedesc_opt_t* opt_desc = typedesc_opt_init();
        typedesc_prim_t* unit_desc = typedesc_prim_unit_init();

        mut_desc->base_type = (typedesc_t*)unit_desc;

        assert_true(typedesc_can_add_ptr((typedesc_t*)mut_desc));
        assert_true(typedesc_can_add_ptr((typedesc_t*)ptr_desc));
        assert_true(typedesc_can_add_ptr((typedesc_t*)opt_desc));
        assert_true(typedesc_can_add_ptr((typedesc_t*)unit_desc));

        typedesc_free((typedesc_t*)unit_desc);
        typedesc_free((typedesc_t*)opt_desc);
        typedesc_free((typedesc_t*)ptr_desc);
        typedesc_free((typedesc_t*)mut_desc);
      end()
      
      it("should not be able to add pointer modifier")
        typedesc_ref_t* ref_desc = typedesc_ref_init();

        assert_false(typedesc_can_add_ptr((typedesc_t*)ref_desc));

        typedesc_free((typedesc_t*)ref_desc);
      end()

      it("should be able to add array modifier")
        typedesc_mut_t* mut_desc = typedesc_mut_init();
        typedesc_ptr_t* ptr_desc = typedesc_ptr_init();
        typedesc_opt_t* opt_desc = typedesc_opt_init();
        typedesc_prim_t* unit_desc = typedesc_prim_unit_init();

        mut_desc->base_type = (typedesc_t*)unit_desc;

        assert_true(typedesc_can_add_array((typedesc_t*)mut_desc));
        assert_true(typedesc_can_add_array((typedesc_t*)ptr_desc));
        assert_true(typedesc_can_add_array((typedesc_t*)opt_desc));
        assert_true(typedesc_can_add_array((typedesc_t*)unit_desc));

        typedesc_free((typedesc_t*)unit_desc);
        typedesc_free((typedesc_t*)opt_desc);
        typedesc_free((typedesc_t*)ptr_desc);
        typedesc_free((typedesc_t*)mut_desc);
      end()
      
      it("should not be able to add array modifier")
        typedesc_ref_t* ref_desc = typedesc_ref_init();

        assert_false(typedesc_can_add_array((typedesc_t*)ref_desc));

        typedesc_free((typedesc_t*)ref_desc);
      end()

      it("should be able to add optional modifier")
        typedesc_ptr_t* ptr_desc = typedesc_ptr_init();
        typedesc_prim_t* unit_desc = typedesc_prim_unit_init();

        assert_true(typedesc_can_add_opt((typedesc_t*)ptr_desc));
        assert_true(typedesc_can_add_opt((typedesc_t*)unit_desc));

        typedesc_free((typedesc_t*)unit_desc);
        typedesc_free((typedesc_t*)ptr_desc);
      end()
      
      it("should not be able to add optional modifier")
        typedesc_mut_t* mut_desc = typedesc_mut_init();
        typedesc_ref_t* ref_desc = typedesc_ref_init();
        typedesc_opt_t* opt_desc = typedesc_opt_init();

        mut_desc->base_type = (typedesc_t*)ref_desc;
        assert_false(typedesc_can_add_opt((typedesc_t*)mut_desc));

        mut_desc->base_type = (typedesc_t*)opt_desc;
        assert_false(typedesc_can_add_opt((typedesc_t*)mut_desc));

        assert_false(typedesc_can_add_opt((typedesc_t*)ref_desc));

        typedesc_free((typedesc_t*)opt_desc);
        typedesc_free((typedesc_t*)ref_desc);
        typedesc_free((typedesc_t*)mut_desc);
      end()
    end()

    describe("typedesc_integer_bits")
      it("should correctly determine bit widths")
        typedesc_prim_t* i8_desc    = typedesc_prim_i8_init();
        typedesc_prim_t* i16_desc   = typedesc_prim_i16_init();
        typedesc_prim_t* i32_desc   = typedesc_prim_i32_init();
        typedesc_prim_t* i64_desc   = typedesc_prim_i64_init();
        typedesc_prim_t* isize_desc = typedesc_prim_isize_init();
        typedesc_prim_t* u8_desc    = typedesc_prim_u8_init();
        typedesc_prim_t* u16_desc   = typedesc_prim_u16_init();
        typedesc_prim_t* u32_desc   = typedesc_prim_u32_init();
        typedesc_prim_t* u64_desc   = typedesc_prim_u64_init();
        typedesc_prim_t* usize_desc = typedesc_prim_usize_init();

        assert_equal(typedesc_integer_bits((typedesc_t*)i8_desc   ), 8                );
        assert_equal(typedesc_integer_bits((typedesc_t*)i16_desc  ), 16               );
        assert_equal(typedesc_integer_bits((typedesc_t*)i32_desc  ), 32               );
        assert_equal(typedesc_integer_bits((typedesc_t*)i64_desc  ), 64               );
        assert_equal(typedesc_integer_bits((typedesc_t*)isize_desc), sizeof(void*) * 8);
        assert_equal(typedesc_integer_bits((typedesc_t*)u8_desc   ), 8                );
        assert_equal(typedesc_integer_bits((typedesc_t*)u16_desc  ), 16               );
        assert_equal(typedesc_integer_bits((typedesc_t*)u32_desc  ), 32               );
        assert_equal(typedesc_integer_bits((typedesc_t*)u64_desc  ), 64               );
        assert_equal(typedesc_integer_bits((typedesc_t*)usize_desc), sizeof(void*) * 8);

        typedesc_free((typedesc_t*)i8_desc   );
        typedesc_free((typedesc_t*)i16_desc  );
        typedesc_free((typedesc_t*)i32_desc  );
        typedesc_free((typedesc_t*)i64_desc  );
        typedesc_free((typedesc_t*)isize_desc);
        typedesc_free((typedesc_t*)u8_desc   );
        typedesc_free((typedesc_t*)u16_desc  );
        typedesc_free((typedesc_t*)u32_desc  );
        typedesc_free((typedesc_t*)u64_desc  );
        typedesc_free((typedesc_t*)usize_desc);
      end()
    end()

    describe("typedesc_is_implicitly_convertible")
      it("should convert primitive type to itself")
        typedesc_prim_t* i8_desc    = typedesc_prim_i8_init();
        typedesc_prim_t* i16_desc   = typedesc_prim_i16_init();
        typedesc_prim_t* i32_desc   = typedesc_prim_i32_init();
        typedesc_prim_t* i64_desc   = typedesc_prim_i64_init();
        typedesc_prim_t* isize_desc = typedesc_prim_isize_init();
        typedesc_prim_t* u8_desc    = typedesc_prim_u8_init();
        typedesc_prim_t* u16_desc   = typedesc_prim_u16_init();
        typedesc_prim_t* u32_desc   = typedesc_prim_u32_init();
        typedesc_prim_t* u64_desc   = typedesc_prim_u64_init();
        typedesc_prim_t* usize_desc = typedesc_prim_usize_init();

        assert_true(typedesc_is_implicitly_convertible((typedesc_t*)i8_desc   , (typedesc_t*)i8_desc   ));
        assert_true(typedesc_is_implicitly_convertible((typedesc_t*)i16_desc  , (typedesc_t*)i16_desc  ));
        assert_true(typedesc_is_implicitly_convertible((typedesc_t*)i32_desc  , (typedesc_t*)i32_desc  ));
        assert_true(typedesc_is_implicitly_convertible((typedesc_t*)i64_desc  , (typedesc_t*)i64_desc  ));
        assert_true(typedesc_is_implicitly_convertible((typedesc_t*)isize_desc, (typedesc_t*)isize_desc));
        assert_true(typedesc_is_implicitly_convertible((typedesc_t*)u8_desc   , (typedesc_t*)u8_desc   ));
        assert_true(typedesc_is_implicitly_convertible((typedesc_t*)u16_desc  , (typedesc_t*)u16_desc  ));
        assert_true(typedesc_is_implicitly_convertible((typedesc_t*)u32_desc  , (typedesc_t*)u32_desc  ));
        assert_true(typedesc_is_implicitly_convertible((typedesc_t*)u64_desc  , (typedesc_t*)u64_desc  ));
        assert_true(typedesc_is_implicitly_convertible((typedesc_t*)usize_desc, (typedesc_t*)usize_desc));

        typedesc_free((typedesc_t*)i8_desc   );
        typedesc_free((typedesc_t*)i16_desc  );
        typedesc_free((typedesc_t*)i32_desc  );
        typedesc_free((typedesc_t*)i64_desc  );
        typedesc_free((typedesc_t*)isize_desc);
        typedesc_free((typedesc_t*)u8_desc   );
        typedesc_free((typedesc_t*)u16_desc  );
        typedesc_free((typedesc_t*)u32_desc  );
        typedesc_free((typedesc_t*)u64_desc  );
        typedesc_free((typedesc_t*)usize_desc);
      end()

      it("should convert \"mut unit\" to \"unit\"")
        typedesc_prim_t* unit_desc = typedesc_prim_unit_init();
        typedesc_mut_t* mut_desc = typedesc_mut_init();

        mut_desc->base_type = (typedesc_t*)unit_desc;

        assert_true(typedesc_is_implicitly_convertible((typedesc_t*)mut_desc, (typedesc_t*)unit_desc));

        typedesc_free((typedesc_t*)mut_desc);
        typedesc_free((typedesc_t*)unit_desc);
      end()

      it("should convert \"unit\" to \"mut unit\"")
        typedesc_prim_t* unit_desc = typedesc_prim_unit_init();
        typedesc_mut_t* mut_desc = typedesc_mut_init();

        mut_desc->base_type = (typedesc_t*)unit_desc;

        assert_true(typedesc_is_implicitly_convertible((typedesc_t*)unit_desc, (typedesc_t*)mut_desc));

        typedesc_free((typedesc_t*)mut_desc);
        typedesc_free((typedesc_t*)unit_desc);
      end()

      it("should convert \"&mut unit\" to \"&unit\"")
        typedesc_prim_t* unit_desc = typedesc_prim_unit_init();
        typedesc_ref_t* ref_imm_desc = typedesc_ref_init();
        typedesc_mut_t* mut_desc = typedesc_mut_init();
        typedesc_ref_t* ref_mut_desc = typedesc_ref_init();

        ref_imm_desc->base_type = (typedesc_t*)unit_desc;
        mut_desc->base_type = (typedesc_t*)unit_desc;
        ref_mut_desc->base_type = (typedesc_t*)mut_desc;

        assert_true(typedesc_is_implicitly_convertible((typedesc_t*)ref_mut_desc, (typedesc_t*)ref_imm_desc));

        typedesc_free((typedesc_t*)ref_mut_desc);
        typedesc_free((typedesc_t*)mut_desc);
        typedesc_free((typedesc_t*)ref_imm_desc);
        typedesc_free((typedesc_t*)unit_desc);
      end()

      it("should not convert \"&unit\" to \"&mut unit\"")
        typedesc_prim_t* unit_desc = typedesc_prim_unit_init();
        typedesc_ref_t* ref_imm_desc = typedesc_ref_init();
        typedesc_mut_t* mut_desc = typedesc_mut_init();
        typedesc_ref_t* ref_mut_desc = typedesc_ref_init();

        ref_imm_desc->base_type = (typedesc_t*)unit_desc;
        mut_desc->base_type = (typedesc_t*)unit_desc;
        ref_mut_desc->base_type = (typedesc_t*)mut_desc;

        assert_false(typedesc_is_implicitly_convertible((typedesc_t*)ref_imm_desc, (typedesc_t*)ref_mut_desc));

        typedesc_free((typedesc_t*)ref_mut_desc);
        typedesc_free((typedesc_t*)mut_desc);
        typedesc_free((typedesc_t*)ref_imm_desc);
        typedesc_free((typedesc_t*)unit_desc);
      end()

      it("should convert \"*mut unit\" to \"*unit\"")
        typedesc_prim_t* unit_desc = typedesc_prim_unit_init();
        typedesc_ptr_t* ptr_imm_desc = typedesc_ptr_init();
        typedesc_mut_t* mut_desc = typedesc_mut_init();
        typedesc_ptr_t* ptr_mut_desc = typedesc_ptr_init();

        ptr_imm_desc->base_type = (typedesc_t*)unit_desc;
        mut_desc->base_type = (typedesc_t*)unit_desc;
        ptr_mut_desc->base_type = (typedesc_t*)mut_desc;

        assert_true(typedesc_is_implicitly_convertible((typedesc_t*)ptr_mut_desc, (typedesc_t*)ptr_imm_desc));

        typedesc_free((typedesc_t*)ptr_mut_desc);
        typedesc_free((typedesc_t*)mut_desc);
        typedesc_free((typedesc_t*)ptr_imm_desc);
        typedesc_free((typedesc_t*)unit_desc);
      end()

      it("should not convert \"*unit\" to \"*mut unit\"")
        typedesc_prim_t* unit_desc = typedesc_prim_unit_init();
        typedesc_ptr_t* ptr_imm_desc = typedesc_ptr_init();
        typedesc_mut_t* mut_desc = typedesc_mut_init();
        typedesc_ptr_t* ptr_mut_desc = typedesc_ptr_init();

        ptr_imm_desc->base_type = (typedesc_t*)unit_desc;
        mut_desc->base_type = (typedesc_t*)unit_desc;
        ptr_mut_desc->base_type = (typedesc_t*)mut_desc;

        assert_false(typedesc_is_implicitly_convertible((typedesc_t*)ptr_imm_desc, (typedesc_t*)ptr_mut_desc));

        typedesc_free((typedesc_t*)ptr_mut_desc);
        typedesc_free((typedesc_t*)mut_desc);
        typedesc_free((typedesc_t*)ptr_imm_desc);
        typedesc_free((typedesc_t*)unit_desc);
      end()

      it("should convert \"&mut *mut unit\" to \"&mut *unit\"")
        typedesc_prim_t* unit_desc = typedesc_prim_unit_init();
        typedesc_ptr_t* ptr_imm_desc = typedesc_ptr_init();
        typedesc_mut_t* mut_unit_desc = typedesc_mut_init();
        typedesc_mut_t* mut_ptr_desc = typedesc_mut_init();
        typedesc_ptr_t* ptr_mut_desc = typedesc_ptr_init();
        typedesc_ref_t* ref_ptr_mut_desc = typedesc_ref_init();
        typedesc_ref_t* ref_ptr_imm_desc = typedesc_ref_init();

        ptr_imm_desc->base_type = (typedesc_t*)unit_desc;
        ref_ptr_imm_desc->base_type = (typedesc_t*)ptr_imm_desc;

        mut_unit_desc->base_type = (typedesc_t*)unit_desc;
        ptr_mut_desc->base_type = (typedesc_t*)mut_unit_desc;
        mut_ptr_desc->base_type = (typedesc_t*)ptr_mut_desc;
        ref_ptr_mut_desc->base_type = (typedesc_t*)mut_ptr_desc;

        assert_true(typedesc_is_implicitly_convertible((typedesc_t*)ref_ptr_mut_desc, (typedesc_t*)ref_ptr_imm_desc));

        typedesc_free((typedesc_t*)ref_ptr_imm_desc);
        typedesc_free((typedesc_t*)ref_ptr_mut_desc);
        typedesc_free((typedesc_t*)ptr_mut_desc);
        typedesc_free((typedesc_t*)mut_ptr_desc);
        typedesc_free((typedesc_t*)mut_unit_desc);
        typedesc_free((typedesc_t*)ptr_imm_desc);
        typedesc_free((typedesc_t*)unit_desc);
      end()

      it("should not convert \"&mut *unit\" to \"&mut *mut unit\"")
        typedesc_prim_t* unit_desc = typedesc_prim_unit_init();
        typedesc_ptr_t* ptr_imm_desc = typedesc_ptr_init();
        typedesc_mut_t* mut_unit_desc = typedesc_mut_init();
        typedesc_mut_t* mut_ptr_desc = typedesc_mut_init();
        typedesc_ptr_t* ptr_mut_desc = typedesc_ptr_init();
        typedesc_ref_t* ref_ptr_mut_desc = typedesc_ref_init();
        typedesc_ref_t* ref_ptr_imm_desc = typedesc_ref_init();

        ptr_imm_desc->base_type = (typedesc_t*)unit_desc;
        ref_ptr_imm_desc->base_type = (typedesc_t*)ptr_imm_desc;

        mut_unit_desc->base_type = (typedesc_t*)unit_desc;
        ptr_mut_desc->base_type = (typedesc_t*)mut_unit_desc;
        mut_ptr_desc->base_type = (typedesc_t*)ptr_mut_desc;
        ref_ptr_mut_desc->base_type = (typedesc_t*)mut_ptr_desc;

        assert_false(typedesc_is_implicitly_convertible((typedesc_t*)ref_ptr_imm_desc, (typedesc_t*)ref_ptr_mut_desc));

        typedesc_free((typedesc_t*)ref_ptr_imm_desc);
        typedesc_free((typedesc_t*)ref_ptr_mut_desc);
        typedesc_free((typedesc_t*)ptr_mut_desc);
        typedesc_free((typedesc_t*)mut_ptr_desc);
        typedesc_free((typedesc_t*)mut_unit_desc);
        typedesc_free((typedesc_t*)ptr_imm_desc);
        typedesc_free((typedesc_t*)unit_desc);
      end()

      it("should convert \"&mut *mut unit\" to \"&*mut unit\"")
        typedesc_prim_t* unit_desc = typedesc_prim_unit_init();
        typedesc_ref_t* ref_imm_desc = typedesc_ref_init();
        typedesc_mut_t* mut_unit_desc = typedesc_mut_init();
        typedesc_mut_t* mut_ptr_desc = typedesc_mut_init();
        typedesc_ptr_t* ptr_mut_desc = typedesc_ptr_init();
        typedesc_ref_t* ref_mut_desc = typedesc_ref_init();

        ref_imm_desc->base_type = (typedesc_t*)ptr_mut_desc;

        mut_unit_desc->base_type = (typedesc_t*)unit_desc;
        ptr_mut_desc->base_type = (typedesc_t*)mut_unit_desc;
        mut_ptr_desc->base_type = (typedesc_t*)ptr_mut_desc;
        ref_mut_desc->base_type = (typedesc_t*)mut_ptr_desc;

        assert_true(typedesc_is_implicitly_convertible((typedesc_t*)ref_mut_desc, (typedesc_t*)ref_imm_desc));

        typedesc_free((typedesc_t*)ref_mut_desc);
        typedesc_free((typedesc_t*)ptr_mut_desc);
        typedesc_free((typedesc_t*)mut_ptr_desc);
        typedesc_free((typedesc_t*)mut_unit_desc);
        typedesc_free((typedesc_t*)ref_imm_desc);
        typedesc_free((typedesc_t*)unit_desc);
      end()

      it("should not convert \"&*mut unit\" to \"&mut *mut unit\"")
        typedesc_prim_t* unit_desc = typedesc_prim_unit_init();
        typedesc_ref_t* ref_imm_desc = typedesc_ref_init();
        typedesc_mut_t* mut_unit_desc = typedesc_mut_init();
        typedesc_mut_t* mut_ptr_desc = typedesc_mut_init();
        typedesc_ptr_t* ptr_mut_desc = typedesc_ptr_init();
        typedesc_ref_t* ref_mut_desc = typedesc_ref_init();

        ref_imm_desc->base_type = (typedesc_t*)ptr_mut_desc;

        mut_unit_desc->base_type = (typedesc_t*)unit_desc;
        ptr_mut_desc->base_type = (typedesc_t*)mut_unit_desc;
        mut_ptr_desc->base_type = (typedesc_t*)ptr_mut_desc;
        ref_mut_desc->base_type = (typedesc_t*)mut_ptr_desc;

        assert_false(typedesc_is_implicitly_convertible((typedesc_t*)ref_imm_desc, (typedesc_t*)ref_mut_desc));

        typedesc_free((typedesc_t*)ref_mut_desc);
        typedesc_free((typedesc_t*)ptr_mut_desc);
        typedesc_free((typedesc_t*)mut_ptr_desc);
        typedesc_free((typedesc_t*)mut_unit_desc);
        typedesc_free((typedesc_t*)ref_imm_desc);
        typedesc_free((typedesc_t*)unit_desc);
      end()

      it("should convert \"&mut *mut unit\" to \"&*unit\"")
        typedesc_prim_t* unit_desc = typedesc_prim_unit_init();
        typedesc_ptr_t* ptr_imm_desc = typedesc_ptr_init();
        typedesc_ref_t* ref_imm_desc = typedesc_ref_init();
        typedesc_mut_t* mut_unit_desc = typedesc_mut_init();
        typedesc_mut_t* mut_ptr_desc = typedesc_mut_init();
        typedesc_ptr_t* ptr_mut_desc = typedesc_ptr_init();
        typedesc_ref_t* ref_mut_desc = typedesc_ref_init();

        ptr_imm_desc->base_type = (typedesc_t*)unit_desc;
        ref_imm_desc->base_type = (typedesc_t*)ptr_imm_desc;

        mut_unit_desc->base_type = (typedesc_t*)unit_desc;
        ptr_mut_desc->base_type = (typedesc_t*)mut_unit_desc;
        mut_ptr_desc->base_type = (typedesc_t*)ptr_mut_desc;
        ref_mut_desc->base_type = (typedesc_t*)mut_ptr_desc;

        assert_true(typedesc_is_implicitly_convertible((typedesc_t*)ref_mut_desc, (typedesc_t*)ref_imm_desc));

        typedesc_free((typedesc_t*)ref_mut_desc);
        typedesc_free((typedesc_t*)ptr_mut_desc);
        typedesc_free((typedesc_t*)mut_ptr_desc);
        typedesc_free((typedesc_t*)mut_unit_desc);
        typedesc_free((typedesc_t*)ref_imm_desc);
        typedesc_free((typedesc_t*)ptr_imm_desc);
        typedesc_free((typedesc_t*)unit_desc);
      end()

      it("should not convert \"&*unit\" to \"&mut *mut unit\"")
        typedesc_prim_t* unit_desc = typedesc_prim_unit_init();
        typedesc_ptr_t* ptr_imm_desc = typedesc_ptr_init();
        typedesc_ref_t* ref_imm_desc = typedesc_ref_init();
        typedesc_mut_t* mut_unit_desc = typedesc_mut_init();
        typedesc_mut_t* mut_ptr_desc = typedesc_mut_init();
        typedesc_ptr_t* ptr_mut_desc = typedesc_ptr_init();
        typedesc_ref_t* ref_mut_desc = typedesc_ref_init();

        ptr_imm_desc->base_type = (typedesc_t*)unit_desc;
        ref_imm_desc->base_type = (typedesc_t*)ptr_imm_desc;

        mut_unit_desc->base_type = (typedesc_t*)unit_desc;
        ptr_mut_desc->base_type = (typedesc_t*)mut_unit_desc;
        mut_ptr_desc->base_type = (typedesc_t*)ptr_mut_desc;
        ref_mut_desc->base_type = (typedesc_t*)mut_ptr_desc;

        assert_false(typedesc_is_implicitly_convertible((typedesc_t*)ref_imm_desc, (typedesc_t*)ref_mut_desc));

        typedesc_free((typedesc_t*)ref_mut_desc);
        typedesc_free((typedesc_t*)ptr_mut_desc);
        typedesc_free((typedesc_t*)mut_ptr_desc);
        typedesc_free((typedesc_t*)mut_unit_desc);
        typedesc_free((typedesc_t*)ref_imm_desc);
        typedesc_free((typedesc_t*)ptr_imm_desc);
        typedesc_free((typedesc_t*)unit_desc);
      end()
    end()
  end()

end()
