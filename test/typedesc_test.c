#include "test.h"

#include "stages/analysis/types/typedesc/typedesc.h"

TEST_CASE(typedesc_remove_mut)
{
  typedesc_mut_t* desc = typedesc_mut_init();
  typedesc_prim_t* base_desc = typedesc_prim_unit_init();

  desc->base_type = (typedesc_t*)base_desc;

  typedesc_t* removed = typedesc_remove_mut((typedesc_t*)desc);
  TEST_ASSERT_PTR_EQUAL(removed, base_desc);

  removed = typedesc_remove_mut((typedesc_t*)base_desc);
  TEST_ASSERT_PTR_EQUAL(removed, base_desc);

  typedesc_free((typedesc_t*)base_desc);
  typedesc_free((typedesc_t*)desc);
}

TEST_CASE(typedesc_remove_ptr)
{
  typedesc_ptr_t* desc = typedesc_ptr_init();
  typedesc_prim_t* base_desc = typedesc_prim_unit_init();

  desc->base_type = (typedesc_t*)base_desc;

  typedesc_t* removed = typedesc_remove_ptr((typedesc_t*)desc);
  TEST_ASSERT_PTR_EQUAL(removed, base_desc);

  removed = typedesc_remove_ptr((typedesc_t*)base_desc);
  TEST_ASSERT_PTR_EQUAL(removed, base_desc);

  typedesc_free((typedesc_t*)base_desc);
  typedesc_free((typedesc_t*)desc);
}

TEST_CASE(typedesc_remove_array)
{
  typedesc_array_t* desc = typedesc_array_init();
  typedesc_prim_t* base_desc = typedesc_prim_unit_init();

  desc->base_type = (typedesc_t*)base_desc;

  typedesc_t* removed = typedesc_remove_array((typedesc_t*)desc);
  TEST_ASSERT_PTR_EQUAL(removed, base_desc);

  removed = typedesc_remove_array((typedesc_t*)base_desc);
  TEST_ASSERT_PTR_EQUAL(removed, base_desc);

  typedesc_free((typedesc_t*)base_desc);
  typedesc_free((typedesc_t*)desc);
}

TEST_CASE(typedesc_remove_ref)
{
  typedesc_ref_t* desc = typedesc_ref_init();
  typedesc_prim_t* base_desc = typedesc_prim_unit_init();

  desc->base_type = (typedesc_t*)base_desc;

  typedesc_t* removed = typedesc_remove_ref((typedesc_t*)desc);
  TEST_ASSERT_PTR_EQUAL(removed, base_desc);

  removed = typedesc_remove_ref((typedesc_t*)base_desc);
  TEST_ASSERT_PTR_EQUAL(removed, base_desc);

  typedesc_free((typedesc_t*)base_desc);
  typedesc_free((typedesc_t*)desc);
}

TEST_CASE(typedesc_remove_opt)
{
  typedesc_opt_t* desc = typedesc_opt_init();
  typedesc_prim_t* base_desc = typedesc_prim_unit_init();

  desc->base_type = (typedesc_t*)base_desc;

  typedesc_t* removed = typedesc_remove_opt((typedesc_t*)desc);
  TEST_ASSERT_PTR_EQUAL(removed, base_desc);

  removed = typedesc_remove_opt((typedesc_t*)base_desc);
  TEST_ASSERT_PTR_EQUAL(removed, base_desc);

  typedesc_free((typedesc_t*)base_desc);
  typedesc_free((typedesc_t*)desc);
}

TEST_CASE(typedesc_remove_ref_mut)
{
  typedesc_ref_t* ref_desc = typedesc_ref_init();
  typedesc_mut_t* mut_desc = typedesc_mut_init();
  typedesc_prim_t* base_desc = typedesc_prim_unit_init();

  ref_desc->base_type = (typedesc_t*)mut_desc;
  mut_desc->base_type = (typedesc_t*)base_desc;

  typedesc_t* removed = typedesc_remove_ref_mut((typedesc_t*)ref_desc);
  TEST_ASSERT_PTR_EQUAL(removed, base_desc);

  removed = typedesc_remove_ref_mut((typedesc_t*)mut_desc);
  TEST_ASSERT_PTR_EQUAL(removed, base_desc);

  removed = typedesc_remove_ref_mut((typedesc_t*)base_desc);
  TEST_ASSERT_PTR_EQUAL(removed, base_desc);

  typedesc_free((typedesc_t*)base_desc);
  typedesc_free((typedesc_t*)mut_desc);
  typedesc_free((typedesc_t*)ref_desc);
}

TEST_CASE(typedesc_can_add_mut)
{
  typedesc_ptr_t* ptr_desc = typedesc_ptr_init();
  typedesc_opt_t* opt_desc = typedesc_opt_init();
  typedesc_prim_t* unit_desc = typedesc_prim_unit_init();

  TEST_ASSERT_TRUE(typedesc_can_add_mut((typedesc_t*)ptr_desc));
  TEST_ASSERT_TRUE(typedesc_can_add_mut((typedesc_t*)opt_desc));
  TEST_ASSERT_TRUE(typedesc_can_add_mut((typedesc_t*)unit_desc));

  typedesc_free((typedesc_t*)unit_desc);
  typedesc_free((typedesc_t*)opt_desc);
  typedesc_free((typedesc_t*)ptr_desc);
}

TEST_CASE(typedesc_cannot_add_mut)
{
  typedesc_mut_t* mut_desc = typedesc_mut_init();
  typedesc_ref_t* ref_desc = typedesc_ref_init();

  TEST_ASSERT_FALSE(typedesc_can_add_mut((typedesc_t*)mut_desc));
  TEST_ASSERT_FALSE(typedesc_can_add_mut((typedesc_t*)ref_desc));

  typedesc_free((typedesc_t*)mut_desc);
  typedesc_free((typedesc_t*)ref_desc);
}

TEST_CASE(typedesc_can_add_ptr)
{
  typedesc_mut_t* mut_desc = typedesc_mut_init();
  typedesc_ptr_t* ptr_desc = typedesc_ptr_init();
  typedesc_opt_t* opt_desc = typedesc_opt_init();
  typedesc_prim_t* unit_desc = typedesc_prim_unit_init();

  mut_desc->base_type = (typedesc_t*)unit_desc;

  TEST_ASSERT_TRUE(typedesc_can_add_ptr((typedesc_t*)mut_desc));
  TEST_ASSERT_TRUE(typedesc_can_add_ptr((typedesc_t*)ptr_desc));
  TEST_ASSERT_TRUE(typedesc_can_add_ptr((typedesc_t*)opt_desc));
  TEST_ASSERT_TRUE(typedesc_can_add_ptr((typedesc_t*)unit_desc));

  typedesc_free((typedesc_t*)unit_desc);
  typedesc_free((typedesc_t*)opt_desc);
  typedesc_free((typedesc_t*)ptr_desc);
  typedesc_free((typedesc_t*)mut_desc);
}

TEST_CASE(typedesc_cannot_add_ptr)
{
  typedesc_ref_t* ref_desc = typedesc_ref_init();

  TEST_ASSERT_FALSE(typedesc_can_add_ptr((typedesc_t*)ref_desc));

  typedesc_free((typedesc_t*)ref_desc);
}

TEST_CASE(typedesc_can_add_array)
{
  typedesc_mut_t* mut_desc = typedesc_mut_init();
  typedesc_ptr_t* ptr_desc = typedesc_ptr_init();
  typedesc_opt_t* opt_desc = typedesc_opt_init();
  typedesc_prim_t* unit_desc = typedesc_prim_unit_init();

  mut_desc->base_type = (typedesc_t*)unit_desc;

  TEST_ASSERT_TRUE(typedesc_can_add_array((typedesc_t*)mut_desc));
  TEST_ASSERT_TRUE(typedesc_can_add_array((typedesc_t*)ptr_desc));
  TEST_ASSERT_TRUE(typedesc_can_add_array((typedesc_t*)opt_desc));
  TEST_ASSERT_TRUE(typedesc_can_add_array((typedesc_t*)unit_desc));

  typedesc_free((typedesc_t*)unit_desc);
  typedesc_free((typedesc_t*)opt_desc);
  typedesc_free((typedesc_t*)ptr_desc);
  typedesc_free((typedesc_t*)mut_desc);
}

TEST_CASE(typedesc_cannot_add_array)
{
  typedesc_ref_t* ref_desc = typedesc_ref_init();

  TEST_ASSERT_FALSE(typedesc_can_add_array((typedesc_t*)ref_desc));

  typedesc_free((typedesc_t*)ref_desc);
}

TEST_CASE(typedesc_can_add_opt)
{
  typedesc_ptr_t* ptr_desc = typedesc_ptr_init();
  typedesc_prim_t* unit_desc = typedesc_prim_unit_init();

  TEST_ASSERT_TRUE(typedesc_can_add_opt((typedesc_t*)ptr_desc));
  TEST_ASSERT_TRUE(typedesc_can_add_opt((typedesc_t*)unit_desc));

  typedesc_free((typedesc_t*)unit_desc);
  typedesc_free((typedesc_t*)ptr_desc);
}

TEST_CASE(typedesc_cannot_add_opt)
{
  typedesc_mut_t* mut_desc = typedesc_mut_init();
  typedesc_ref_t* ref_desc = typedesc_ref_init();
  typedesc_opt_t* opt_desc = typedesc_opt_init();

  mut_desc->base_type = (typedesc_t*)ref_desc;
  TEST_ASSERT_FALSE(typedesc_can_add_opt((typedesc_t*)mut_desc));

  mut_desc->base_type = (typedesc_t*)opt_desc;
  TEST_ASSERT_FALSE(typedesc_can_add_opt((typedesc_t*)mut_desc));

  TEST_ASSERT_FALSE(typedesc_can_add_opt((typedesc_t*)ref_desc));

  typedesc_free((typedesc_t*)opt_desc);
  typedesc_free((typedesc_t*)ref_desc);
  typedesc_free((typedesc_t*)mut_desc);
}

TEST_CASE(typedesc_integer_bits)
{
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

        TEST_ASSERT_EQUAL(typedesc_integer_bits((typedesc_t*)i8_desc   ), 8                );
        TEST_ASSERT_EQUAL(typedesc_integer_bits((typedesc_t*)i16_desc  ), 16               );
        TEST_ASSERT_EQUAL(typedesc_integer_bits((typedesc_t*)i32_desc  ), 32               );
        TEST_ASSERT_EQUAL(typedesc_integer_bits((typedesc_t*)i64_desc  ), 64               );
        TEST_ASSERT_EQUAL(typedesc_integer_bits((typedesc_t*)isize_desc), sizeof(void*) * 8);
        TEST_ASSERT_EQUAL(typedesc_integer_bits((typedesc_t*)u8_desc   ), 8                );
        TEST_ASSERT_EQUAL(typedesc_integer_bits((typedesc_t*)u16_desc  ), 16               );
        TEST_ASSERT_EQUAL(typedesc_integer_bits((typedesc_t*)u32_desc  ), 32               );
        TEST_ASSERT_EQUAL(typedesc_integer_bits((typedesc_t*)u64_desc  ), 64               );
        TEST_ASSERT_EQUAL(typedesc_integer_bits((typedesc_t*)usize_desc), sizeof(void*) * 8);

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
}

TEST_CASE(typedesc_implicitly_direct_convert_prim_to_self)
{
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

  TEST_ASSERT_TRUE(typedesc_is_implicitly_direct_convertible((typedesc_t*)i8_desc,    (typedesc_t*)i8_desc   ));
  TEST_ASSERT_TRUE(typedesc_is_implicitly_direct_convertible((typedesc_t*)i16_desc,   (typedesc_t*)i16_desc  ));
  TEST_ASSERT_TRUE(typedesc_is_implicitly_direct_convertible((typedesc_t*)i32_desc,   (typedesc_t*)i32_desc  ));
  TEST_ASSERT_TRUE(typedesc_is_implicitly_direct_convertible((typedesc_t*)i64_desc,   (typedesc_t*)i64_desc  ));
  TEST_ASSERT_TRUE(typedesc_is_implicitly_direct_convertible((typedesc_t*)isize_desc, (typedesc_t*)isize_desc));
  TEST_ASSERT_TRUE(typedesc_is_implicitly_direct_convertible((typedesc_t*)u8_desc,    (typedesc_t*)u8_desc   ));
  TEST_ASSERT_TRUE(typedesc_is_implicitly_direct_convertible((typedesc_t*)u16_desc,   (typedesc_t*)u16_desc  ));
  TEST_ASSERT_TRUE(typedesc_is_implicitly_direct_convertible((typedesc_t*)u32_desc,   (typedesc_t*)u32_desc  ));
  TEST_ASSERT_TRUE(typedesc_is_implicitly_direct_convertible((typedesc_t*)u64_desc,   (typedesc_t*)u64_desc  ));
  TEST_ASSERT_TRUE(typedesc_is_implicitly_direct_convertible((typedesc_t*)usize_desc, (typedesc_t*)usize_desc));

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
}

TEST_CASE(typedesc_implicitly_direct_convert_mut_to_immut)
{
  typedesc_prim_t* unit_desc = typedesc_prim_unit_init();
  typedesc_mut_t* mut_desc = typedesc_mut_init();

  mut_desc->base_type = (typedesc_t*)unit_desc;

  TEST_ASSERT_TRUE(typedesc_is_implicitly_direct_convertible((typedesc_t*)mut_desc, (typedesc_t*)unit_desc));

  typedesc_free((typedesc_t*)mut_desc);
  typedesc_free((typedesc_t*)unit_desc);
}

TEST_CASE(typedesc_implicitly_direct_convert_immut_to_mut)
{
  typedesc_prim_t* unit_desc = typedesc_prim_unit_init();
  typedesc_mut_t* mut_desc = typedesc_mut_init();

  mut_desc->base_type = (typedesc_t*)unit_desc;

  TEST_ASSERT_TRUE(typedesc_is_implicitly_direct_convertible((typedesc_t*)unit_desc, (typedesc_t*)mut_desc));

  typedesc_free((typedesc_t*)mut_desc);
  typedesc_free((typedesc_t*)unit_desc);
}

TEST_CASE(typedesc_implicitly_direct_convert_ref_mut_to_ref_immut)
{
  typedesc_prim_t* unit_desc = typedesc_prim_unit_init();
  typedesc_ref_t* ref_imm_desc = typedesc_ref_init();
  typedesc_mut_t* mut_desc = typedesc_mut_init();
  typedesc_ref_t* ref_mut_desc = typedesc_ref_init();

  ref_imm_desc->base_type = (typedesc_t*)unit_desc;
  mut_desc->base_type = (typedesc_t*)unit_desc;
  ref_mut_desc->base_type = (typedesc_t*)mut_desc;

  TEST_ASSERT_TRUE(typedesc_is_implicitly_direct_convertible((typedesc_t*)ref_mut_desc, (typedesc_t*)ref_imm_desc));

  typedesc_free((typedesc_t*)ref_mut_desc);
  typedesc_free((typedesc_t*)mut_desc);
  typedesc_free((typedesc_t*)ref_imm_desc);
  typedesc_free((typedesc_t*)unit_desc);
}

TEST_CASE(typedesc_implicitly_direct_convert_ref_immut_to_ref_mut)
{
  typedesc_prim_t* unit_desc = typedesc_prim_unit_init();
  typedesc_ref_t* ref_imm_desc = typedesc_ref_init();
  typedesc_mut_t* mut_desc = typedesc_mut_init();
  typedesc_ref_t* ref_mut_desc = typedesc_ref_init();

  ref_imm_desc->base_type = (typedesc_t*)unit_desc;
  mut_desc->base_type = (typedesc_t*)unit_desc;
  ref_mut_desc->base_type = (typedesc_t*)mut_desc;

  TEST_ASSERT_FALSE(typedesc_is_implicitly_direct_convertible((typedesc_t*)ref_imm_desc, (typedesc_t*)ref_mut_desc));

  typedesc_free((typedesc_t*)ref_mut_desc);
  typedesc_free((typedesc_t*)mut_desc);
  typedesc_free((typedesc_t*)ref_imm_desc);
  typedesc_free((typedesc_t*)unit_desc);
}

TEST_CASE(typedesc_implicitly_direct_convert_ptr_mut_to_ptr_immut)
{
  typedesc_prim_t* unit_desc = typedesc_prim_unit_init();
  typedesc_ptr_t* ptr_imm_desc = typedesc_ptr_init();
  typedesc_mut_t* mut_desc = typedesc_mut_init();
  typedesc_ptr_t* ptr_mut_desc = typedesc_ptr_init();

  ptr_imm_desc->base_type = (typedesc_t*)unit_desc;
  mut_desc->base_type = (typedesc_t*)unit_desc;
  ptr_mut_desc->base_type = (typedesc_t*)mut_desc;

  TEST_ASSERT_TRUE(typedesc_is_implicitly_direct_convertible((typedesc_t*)ptr_mut_desc, (typedesc_t*)ptr_imm_desc));

  typedesc_free((typedesc_t*)ptr_mut_desc);
  typedesc_free((typedesc_t*)mut_desc);
  typedesc_free((typedesc_t*)ptr_imm_desc);
  typedesc_free((typedesc_t*)unit_desc);
}

TEST_CASE(typedesc_implicitly_direct_convert_ptr_immut_to_ptr_mut)
{
  typedesc_prim_t* unit_desc = typedesc_prim_unit_init();
  typedesc_ptr_t* ptr_imm_desc = typedesc_ptr_init();
  typedesc_mut_t* mut_desc = typedesc_mut_init();
  typedesc_ptr_t* ptr_mut_desc = typedesc_ptr_init();

  ptr_imm_desc->base_type = (typedesc_t*)unit_desc;
  mut_desc->base_type = (typedesc_t*)unit_desc;
  ptr_mut_desc->base_type = (typedesc_t*)mut_desc;

  TEST_ASSERT_FALSE(typedesc_is_implicitly_direct_convertible((typedesc_t*)ptr_imm_desc, (typedesc_t*)ptr_mut_desc));

  typedesc_free((typedesc_t*)ptr_mut_desc);
  typedesc_free((typedesc_t*)mut_desc);
  typedesc_free((typedesc_t*)ptr_imm_desc);
  typedesc_free((typedesc_t*)unit_desc);
}

TEST_CASE(typedesc_implicitly_direct_convert_ref_mut_ptr_mut_to_ref_mut_ptr_immut)
{
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

  TEST_ASSERT_TRUE(typedesc_is_implicitly_direct_convertible((typedesc_t*)ref_ptr_mut_desc, (typedesc_t*)ref_ptr_imm_desc));

  typedesc_free((typedesc_t*)ref_ptr_imm_desc);
  typedesc_free((typedesc_t*)ref_ptr_mut_desc);
  typedesc_free((typedesc_t*)ptr_mut_desc);
  typedesc_free((typedesc_t*)mut_ptr_desc);
  typedesc_free((typedesc_t*)mut_unit_desc);
  typedesc_free((typedesc_t*)ptr_imm_desc);
  typedesc_free((typedesc_t*)unit_desc);
}

TEST_CASE(typedesc_implicitly_direct_convert_ref_mut_ptr_immut_to_ref_mut_ptr_mut)
{
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

  TEST_ASSERT_FALSE(typedesc_is_implicitly_direct_convertible((typedesc_t*)ref_ptr_imm_desc, (typedesc_t*)ref_ptr_mut_desc));

  typedesc_free((typedesc_t*)ref_ptr_imm_desc);
  typedesc_free((typedesc_t*)ref_ptr_mut_desc);
  typedesc_free((typedesc_t*)ptr_mut_desc);
  typedesc_free((typedesc_t*)mut_ptr_desc);
  typedesc_free((typedesc_t*)mut_unit_desc);
  typedesc_free((typedesc_t*)ptr_imm_desc);
  typedesc_free((typedesc_t*)unit_desc);
}

TEST_CASE(typedesc_implicitly_direct_convert_ref_mut_ptr_mut_to_ref_immut_ptr_mut)
{
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

  TEST_ASSERT_TRUE(typedesc_is_implicitly_direct_convertible((typedesc_t*)ref_mut_desc, (typedesc_t*)ref_imm_desc));

  typedesc_free((typedesc_t*)ref_mut_desc);
  typedesc_free((typedesc_t*)ptr_mut_desc);
  typedesc_free((typedesc_t*)mut_ptr_desc);
  typedesc_free((typedesc_t*)mut_unit_desc);
  typedesc_free((typedesc_t*)ref_imm_desc);
  typedesc_free((typedesc_t*)unit_desc);
}

TEST_CASE(typedesc_implicitly_direct_convert_ref_immut_ptr_mut_to_ref_mut_ptr_mut)
{
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

  TEST_ASSERT_FALSE(typedesc_is_implicitly_direct_convertible((typedesc_t*)ref_imm_desc, (typedesc_t*)ref_mut_desc));

  typedesc_free((typedesc_t*)ref_mut_desc);
  typedesc_free((typedesc_t*)ptr_mut_desc);
  typedesc_free((typedesc_t*)mut_ptr_desc);
  typedesc_free((typedesc_t*)mut_unit_desc);
  typedesc_free((typedesc_t*)ref_imm_desc);
  typedesc_free((typedesc_t*)unit_desc);
}

TEST_CASE(typedesc_implicitly_direct_convert_ref_mut_ptr_mut_to_ref_immut_ptr_immut)
{
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

  TEST_ASSERT_TRUE(typedesc_is_implicitly_direct_convertible((typedesc_t*)ref_mut_desc, (typedesc_t*)ref_imm_desc));

  typedesc_free((typedesc_t*)ref_mut_desc);
  typedesc_free((typedesc_t*)ptr_mut_desc);
  typedesc_free((typedesc_t*)mut_ptr_desc);
  typedesc_free((typedesc_t*)mut_unit_desc);
  typedesc_free((typedesc_t*)ref_imm_desc);
  typedesc_free((typedesc_t*)ptr_imm_desc);
  typedesc_free((typedesc_t*)unit_desc);
}

TEST_CASE(typedesc_implicitly_direct_convert_ref_immut_ptr_immut_to_ref_mut_ptr_mut)
{
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

  TEST_ASSERT_FALSE(typedesc_is_implicitly_direct_convertible((typedesc_t*)ref_imm_desc, (typedesc_t*)ref_mut_desc));

  typedesc_free((typedesc_t*)ref_mut_desc);
  typedesc_free((typedesc_t*)ptr_mut_desc);
  typedesc_free((typedesc_t*)mut_ptr_desc);
  typedesc_free((typedesc_t*)mut_unit_desc);
  typedesc_free((typedesc_t*)ref_imm_desc);
  typedesc_free((typedesc_t*)ptr_imm_desc);
  typedesc_free((typedesc_t*)unit_desc);
}

TEST_MAIN()
{
  TEST_RUN(typedesc_remove_mut);
  TEST_RUN(typedesc_remove_ptr);
  TEST_RUN(typedesc_remove_array);
  TEST_RUN(typedesc_remove_ref);
  TEST_RUN(typedesc_remove_opt);
  TEST_RUN(typedesc_remove_ref_mut);
  TEST_RUN(typedesc_can_add_mut);
  TEST_RUN(typedesc_cannot_add_mut);
  TEST_RUN(typedesc_can_add_ptr);
  TEST_RUN(typedesc_cannot_add_ptr);
  TEST_RUN(typedesc_can_add_array);
  TEST_RUN(typedesc_cannot_add_array);
  TEST_RUN(typedesc_can_add_opt);
  TEST_RUN(typedesc_cannot_add_opt);
  TEST_RUN(typedesc_integer_bits);
  TEST_RUN(typedesc_implicitly_direct_convert_prim_to_self);
  TEST_RUN(typedesc_implicitly_direct_convert_mut_to_immut);
  TEST_RUN(typedesc_implicitly_direct_convert_immut_to_mut);
  TEST_RUN(typedesc_implicitly_direct_convert_ref_mut_to_ref_immut);
  TEST_RUN(typedesc_implicitly_direct_convert_ref_immut_to_ref_mut);
  TEST_RUN(typedesc_implicitly_direct_convert_ptr_mut_to_ptr_immut);
  TEST_RUN(typedesc_implicitly_direct_convert_ptr_immut_to_ptr_mut);
  TEST_RUN(typedesc_implicitly_direct_convert_ref_mut_ptr_mut_to_ref_mut_ptr_immut);
  TEST_RUN(typedesc_implicitly_direct_convert_ref_mut_ptr_immut_to_ref_mut_ptr_mut);
  TEST_RUN(typedesc_implicitly_direct_convert_ref_mut_ptr_mut_to_ref_immut_ptr_mut);
  TEST_RUN(typedesc_implicitly_direct_convert_ref_immut_ptr_mut_to_ref_mut_ptr_mut);
  TEST_RUN(typedesc_implicitly_direct_convert_ref_mut_ptr_mut_to_ref_immut_ptr_immut);
  TEST_RUN(typedesc_implicitly_direct_convert_ref_immut_ptr_immut_to_ref_mut_ptr_mut);
}
