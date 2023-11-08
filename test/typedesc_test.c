#include "test.h"

#include "typedesc.h"

test()

  describe("typedesc")
    describe("typedesc_remove")
      it("should remove mutable modifier")
        typedesc_mut_t* desc = (typedesc_mut_t*)typedesc_init(TYPEDESC_MUT);
        typedesc_t* base_desc = typedesc_init(TYPEDESC_UNIT);

        desc->base_type = base_desc;

        typedesc_t* removed = typedesc_remove_mut((typedesc_t*)desc);
        assert_ptr_equal(removed, base_desc);

        removed = typedesc_remove_mut(base_desc);
        assert_ptr_equal(removed, base_desc);

        typedesc_free(base_desc);
        typedesc_free((typedesc_t*)desc);
      end()

      it("should remove constant modifier")
        typedesc_const_t* desc = (typedesc_const_t*)typedesc_init(TYPEDESC_CONST);
        typedesc_t* base_desc = typedesc_init(TYPEDESC_UNIT);

        desc->base_type = base_desc;

        typedesc_t* removed = typedesc_remove_const((typedesc_t*)desc);
        assert_ptr_equal(removed, base_desc);

        removed = typedesc_remove_const(base_desc);
        assert_ptr_equal(removed, base_desc);

        typedesc_free(base_desc);
        typedesc_free((typedesc_t*)desc);
      end()

      it("should remove pointer modifier")
        typedesc_ptr_t* desc = (typedesc_ptr_t*)typedesc_init(TYPEDESC_PTR);
        typedesc_t* base_desc = typedesc_init(TYPEDESC_UNIT);

        desc->base_type = base_desc;

        typedesc_t* removed = typedesc_remove_ptr((typedesc_t*)desc);
        assert_ptr_equal(removed, base_desc);

        removed = typedesc_remove_ptr(base_desc);
        assert_ptr_equal(removed, base_desc);

        typedesc_free(base_desc);
        typedesc_free((typedesc_t*)desc);
      end()

      it("should remove array modifier")
        typedesc_array_t* desc = (typedesc_array_t*)typedesc_init(TYPEDESC_ARRAY);
        typedesc_t* base_desc = typedesc_init(TYPEDESC_UNIT);

        desc->base_type = base_desc;

        typedesc_t* removed = typedesc_remove_array((typedesc_t*)desc);
        assert_ptr_equal(removed, base_desc);

        removed = typedesc_remove_array(base_desc);
        assert_ptr_equal(removed, base_desc);

        typedesc_free(base_desc);
        typedesc_free((typedesc_t*)desc);
      end()

      it("should remove reference modifier")
        typedesc_ref_t* desc = (typedesc_ref_t*)typedesc_init(TYPEDESC_REF);
        typedesc_t* base_desc = typedesc_init(TYPEDESC_UNIT);

        desc->base_type = base_desc;

        typedesc_t* removed = typedesc_remove_ref((typedesc_t*)desc);
        assert_ptr_equal(removed, base_desc);

        removed = typedesc_remove_ref(base_desc);
        assert_ptr_equal(removed, base_desc);

        typedesc_free(base_desc);
        typedesc_free((typedesc_t*)desc);
      end()

      it("should remove optional modifier")
        typedesc_opt_t* desc = (typedesc_opt_t*)typedesc_init(TYPEDESC_OPT);
        typedesc_t* base_desc = typedesc_init(TYPEDESC_UNIT);

        desc->base_type = base_desc;

        typedesc_t* removed = typedesc_remove_opt((typedesc_t*)desc);
        assert_ptr_equal(removed, base_desc);

        removed = typedesc_remove_opt(base_desc);
        assert_ptr_equal(removed, base_desc);

        typedesc_free(base_desc);
        typedesc_free((typedesc_t*)desc);
      end()

      it("should remove constant and mutable modifier")
        typedesc_const_t* const_desc = (typedesc_const_t*)typedesc_init(TYPEDESC_CONST);
        typedesc_mut_t* mut_desc = (typedesc_mut_t*)typedesc_init(TYPEDESC_MUT);
        typedesc_t* base_desc = typedesc_init(TYPEDESC_UNIT);

        const_desc->base_type = (typedesc_t*)mut_desc;
        mut_desc->base_type = base_desc;

        typedesc_t* removed = typedesc_remove_const_mut((typedesc_t*)const_desc);
        assert_ptr_equal(removed, base_desc);

        removed = typedesc_remove_const_mut((typedesc_t*)mut_desc);
        assert_ptr_equal(removed, base_desc);

        removed = typedesc_remove_const_mut(base_desc);
        assert_ptr_equal(removed, base_desc);

        typedesc_free(base_desc);
        typedesc_free((typedesc_t*)mut_desc);
        typedesc_free((typedesc_t*)const_desc);
      end()

      it("should remove constant, mutable and reference modifier")
        typedesc_const_t* const_desc = (typedesc_const_t*)typedesc_init(TYPEDESC_CONST);
        typedesc_mut_t* mut_desc = (typedesc_mut_t*)typedesc_init(TYPEDESC_MUT);
        typedesc_ref_t* ref_desc = (typedesc_ref_t*)typedesc_init(TYPEDESC_REF);
        typedesc_t* base_desc = typedesc_init(TYPEDESC_UNIT);

        const_desc->base_type = (typedesc_t*)mut_desc;
        mut_desc->base_type = (typedesc_t*)ref_desc;
        ref_desc->base_type = base_desc;

        typedesc_t* removed = typedesc_remove_const_mut_ref((typedesc_t*)const_desc);
        assert_ptr_equal(removed, base_desc);

        removed = typedesc_remove_const_mut_ref((typedesc_t*)mut_desc);
        assert_ptr_equal(removed, base_desc);

        removed = typedesc_remove_const_mut_ref((typedesc_t*)ref_desc);
        assert_ptr_equal(removed, base_desc);

        removed = typedesc_remove_const_mut_ref(base_desc);
        assert_ptr_equal(removed, base_desc);

        typedesc_free(base_desc);
        typedesc_free((typedesc_t*)ref_desc);
        typedesc_free((typedesc_t*)mut_desc);
        typedesc_free((typedesc_t*)const_desc);
      end()
    end()

    describe("typedesc_check_can_add")
      it("should be able to add mutable modifier")
        typedesc_ref_t* ref_desc = (typedesc_ref_t*)typedesc_init(TYPEDESC_REF);
        typedesc_ptr_t* ptr_desc = (typedesc_ptr_t*)typedesc_init(TYPEDESC_PTR);
        typedesc_opt_t* opt_desc = (typedesc_opt_t*)typedesc_init(TYPEDESC_OPT);
        typedesc_t* unit_desc = typedesc_init(TYPEDESC_UNIT);

        assert_true(typedesc_check_can_add_mut((typedesc_t*)ref_desc));
        assert_true(typedesc_check_can_add_mut((typedesc_t*)ptr_desc));
        assert_true(typedesc_check_can_add_mut((typedesc_t*)opt_desc));
        assert_true(typedesc_check_can_add_mut(unit_desc));

        typedesc_free(unit_desc);
        typedesc_free((typedesc_t*)opt_desc);
        typedesc_free((typedesc_t*)ptr_desc);
        typedesc_free((typedesc_t*)ref_desc);
      end()

      it("should not be able to add mutable modifier")
        typedesc_mut_t* mut_desc = (typedesc_mut_t*)typedesc_init(TYPEDESC_MUT);
        typedesc_const_t* const_desc = (typedesc_const_t*)typedesc_init(TYPEDESC_CONST);

        assert_false(typedesc_check_can_add_mut((typedesc_t*)mut_desc));
        assert_false(typedesc_check_can_add_mut((typedesc_t*)const_desc));

        typedesc_free((typedesc_t*)mut_desc);
        typedesc_free((typedesc_t*)const_desc);
      end()

      it("should be able to add constant modifier")
        typedesc_mut_t* mut_desc = (typedesc_mut_t*)typedesc_init(TYPEDESC_MUT);
        typedesc_ref_t* ref_desc = (typedesc_ref_t*)typedesc_init(TYPEDESC_REF);
        typedesc_ptr_t* ptr_desc = (typedesc_ptr_t*)typedesc_init(TYPEDESC_PTR);
        typedesc_opt_t* opt_desc = (typedesc_opt_t*)typedesc_init(TYPEDESC_OPT);
        typedesc_t* unit_desc = typedesc_init(TYPEDESC_UNIT);

        assert_true(typedesc_check_can_add_const((typedesc_t*)mut_desc));
        assert_true(typedesc_check_can_add_const((typedesc_t*)ref_desc));
        assert_true(typedesc_check_can_add_const((typedesc_t*)ptr_desc));
        assert_true(typedesc_check_can_add_const((typedesc_t*)opt_desc));
        assert_true(typedesc_check_can_add_const(unit_desc));

        typedesc_free(unit_desc);
        typedesc_free((typedesc_t*)opt_desc);
        typedesc_free((typedesc_t*)ptr_desc);
        typedesc_free((typedesc_t*)ref_desc);
        typedesc_free((typedesc_t*)mut_desc);
      end()
      
      it("should not be able to add constant modifier")
        typedesc_const_t* const_desc = (typedesc_const_t*)typedesc_init(TYPEDESC_CONST);

        assert_false(typedesc_check_can_add_const((typedesc_t*)const_desc));

        typedesc_free((typedesc_t*)const_desc);
      end()

      it("should be able to add pointer modifier")
        typedesc_mut_t* mut_desc = (typedesc_mut_t*)typedesc_init(TYPEDESC_MUT);
        typedesc_ptr_t* ptr_desc = (typedesc_ptr_t*)typedesc_init(TYPEDESC_PTR);
        typedesc_opt_t* opt_desc = (typedesc_opt_t*)typedesc_init(TYPEDESC_OPT);
        typedesc_t* unit_desc = typedesc_init(TYPEDESC_UNIT);

        mut_desc->base_type = unit_desc;

        assert_true(typedesc_check_can_add_ptr((typedesc_t*)mut_desc));
        assert_true(typedesc_check_can_add_ptr((typedesc_t*)ptr_desc));
        assert_true(typedesc_check_can_add_ptr((typedesc_t*)opt_desc));
        assert_true(typedesc_check_can_add_ptr(unit_desc));

        typedesc_free(unit_desc);
        typedesc_free((typedesc_t*)opt_desc);
        typedesc_free((typedesc_t*)ptr_desc);
        typedesc_free((typedesc_t*)mut_desc);
      end()
      
      it("should not be able to add pointer modifier")
        typedesc_const_t* const_desc = (typedesc_const_t*)typedesc_init(TYPEDESC_CONST);
        typedesc_mut_t* mut_desc = (typedesc_mut_t*)typedesc_init(TYPEDESC_MUT);
        typedesc_ref_t* ref_desc = (typedesc_ref_t*)typedesc_init(TYPEDESC_REF);

        mut_desc->base_type = (typedesc_t*)ref_desc;

        assert_false(typedesc_check_can_add_ptr((typedesc_t*)const_desc));
        assert_false(typedesc_check_can_add_ptr((typedesc_t*)mut_desc));
        assert_false(typedesc_check_can_add_ptr((typedesc_t*)ref_desc));

        typedesc_free((typedesc_t*)ref_desc);
        typedesc_free((typedesc_t*)mut_desc);
        typedesc_free((typedesc_t*)const_desc);
      end()

      it("should be able to add array modifier")
        typedesc_mut_t* mut_desc = (typedesc_mut_t*)typedesc_init(TYPEDESC_MUT);
        typedesc_ptr_t* ptr_desc = (typedesc_ptr_t*)typedesc_init(TYPEDESC_PTR);
        typedesc_opt_t* opt_desc = (typedesc_opt_t*)typedesc_init(TYPEDESC_OPT);
        typedesc_t* unit_desc = typedesc_init(TYPEDESC_UNIT);

        mut_desc->base_type = unit_desc;

        assert_true(typedesc_check_can_add_array((typedesc_t*)mut_desc));
        assert_true(typedesc_check_can_add_array((typedesc_t*)ptr_desc));
        assert_true(typedesc_check_can_add_array((typedesc_t*)opt_desc));
        assert_true(typedesc_check_can_add_array(unit_desc));

        typedesc_free(unit_desc);
        typedesc_free((typedesc_t*)opt_desc);
        typedesc_free((typedesc_t*)ptr_desc);
        typedesc_free((typedesc_t*)mut_desc);
      end()
      
      it("should not be able to add array modifier")
        typedesc_const_t* const_desc = (typedesc_const_t*)typedesc_init(TYPEDESC_CONST);
        typedesc_mut_t* mut_desc = (typedesc_mut_t*)typedesc_init(TYPEDESC_MUT);
        typedesc_ref_t* ref_desc = (typedesc_ref_t*)typedesc_init(TYPEDESC_REF);

        mut_desc->base_type = (typedesc_t*)ref_desc;

        assert_false(typedesc_check_can_add_array((typedesc_t*)const_desc));
        assert_false(typedesc_check_can_add_array((typedesc_t*)mut_desc));
        assert_false(typedesc_check_can_add_array((typedesc_t*)ref_desc));

        typedesc_free((typedesc_t*)ref_desc);
        typedesc_free((typedesc_t*)mut_desc);
        typedesc_free((typedesc_t*)const_desc);
      end()

      it("should be able to add optional modifier")
        typedesc_mut_t* mut_desc = (typedesc_mut_t*)typedesc_init(TYPEDESC_MUT);
        typedesc_ptr_t* ptr_desc = (typedesc_ptr_t*)typedesc_init(TYPEDESC_PTR);
        typedesc_t* unit_desc = typedesc_init(TYPEDESC_UNIT);

        mut_desc->base_type = unit_desc;

        assert_true(typedesc_check_can_add_opt((typedesc_t*)mut_desc));
        assert_true(typedesc_check_can_add_opt((typedesc_t*)ptr_desc));
        assert_true(typedesc_check_can_add_opt(unit_desc));

        typedesc_free(unit_desc);
        typedesc_free((typedesc_t*)ptr_desc);
        typedesc_free((typedesc_t*)mut_desc);
      end()
      
      it("should not be able to add optional modifier")
        typedesc_const_t* const_desc = (typedesc_const_t*)typedesc_init(TYPEDESC_CONST);
        typedesc_mut_t* mut_desc = (typedesc_mut_t*)typedesc_init(TYPEDESC_MUT);
        typedesc_ref_t* ref_desc = (typedesc_ref_t*)typedesc_init(TYPEDESC_REF);
        typedesc_opt_t* opt_desc = (typedesc_opt_t*)typedesc_init(TYPEDESC_OPT);

        mut_desc->base_type = (typedesc_t*)ref_desc;
        assert_false(typedesc_check_can_add_opt((typedesc_t*)mut_desc));

        mut_desc->base_type = (typedesc_t*)opt_desc;
        assert_false(typedesc_check_can_add_opt((typedesc_t*)mut_desc));

        assert_false(typedesc_check_can_add_opt((typedesc_t*)const_desc));
        assert_false(typedesc_check_can_add_opt((typedesc_t*)ref_desc));

        typedesc_free((typedesc_t*)opt_desc);
        typedesc_free((typedesc_t*)ref_desc);
        typedesc_free((typedesc_t*)mut_desc);
        typedesc_free((typedesc_t*)const_desc);
      end()
    end()

    describe("typedesc_integer_bits")
      it("should correctly determine bit widths")
        typedesc_t* i8_desc    = typedesc_init(TYPEDESC_I8);
        typedesc_t* i16_desc   = typedesc_init(TYPEDESC_I16);
        typedesc_t* i32_desc   = typedesc_init(TYPEDESC_I32);
        typedesc_t* i64_desc   = typedesc_init(TYPEDESC_I64);
        typedesc_t* isize_desc = typedesc_init(TYPEDESC_ISIZE);
        typedesc_t* u8_desc    = typedesc_init(TYPEDESC_U8);
        typedesc_t* u16_desc   = typedesc_init(TYPEDESC_U16);
        typedesc_t* u32_desc   = typedesc_init(TYPEDESC_U32);
        typedesc_t* u64_desc   = typedesc_init(TYPEDESC_U64);
        typedesc_t* usize_desc = typedesc_init(TYPEDESC_USIZE);

        assert_equal(typedesc_integer_bits(i8_desc   ), 8);
        assert_equal(typedesc_integer_bits(i16_desc  ), 16);
        assert_equal(typedesc_integer_bits(i32_desc  ), 32);
        assert_equal(typedesc_integer_bits(i64_desc  ), 64);
        assert_equal(typedesc_integer_bits(isize_desc), sizeof(void*) * 8);
        assert_equal(typedesc_integer_bits(u8_desc   ), 8);
        assert_equal(typedesc_integer_bits(u16_desc  ), 16);
        assert_equal(typedesc_integer_bits(u32_desc  ), 32);
        assert_equal(typedesc_integer_bits(u64_desc  ), 64);
        assert_equal(typedesc_integer_bits(usize_desc), sizeof(void*) * 8);

        typedesc_free(i8_desc);
        typedesc_free(i16_desc);
        typedesc_free(i32_desc);
        typedesc_free(i64_desc);
        typedesc_free(isize_desc);
        typedesc_free(u8_desc);
        typedesc_free(u16_desc);
        typedesc_free(u32_desc);
        typedesc_free(u64_desc);
        typedesc_free(usize_desc);
      end()
    end()
  end()

end()
