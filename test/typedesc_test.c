#include "test.h"

#include "stages/analysis/types/typedesc/typedesc.h"

TEST_CASE(tau_typedesc_remove_mut)
{
  tau_typedesc_mut_t* desc = tau_typedesc_mut_init();
  tau_typedesc_prim_t* base_desc = tau_typedesc_prim_unit_init();

  desc->base_type = (tau_typedesc_t*)base_desc;

  tau_typedesc_t* removed = tau_typedesc_remove_mut((tau_typedesc_t*)desc);
  TEST_ASSERT_PTR_EQUAL(removed, base_desc);

  removed = tau_typedesc_remove_mut((tau_typedesc_t*)base_desc);
  TEST_ASSERT_PTR_EQUAL(removed, base_desc);

  tau_typedesc_free((tau_typedesc_t*)base_desc);
  tau_typedesc_free((tau_typedesc_t*)desc);
}

TEST_CASE(tau_typedesc_remove_ptr)
{
  tau_typedesc_ptr_t* desc = tau_typedesc_ptr_init();
  tau_typedesc_prim_t* base_desc = tau_typedesc_prim_unit_init();

  desc->base_type = (tau_typedesc_t*)base_desc;

  tau_typedesc_t* removed = tau_typedesc_remove_ptr((tau_typedesc_t*)desc);
  TEST_ASSERT_PTR_EQUAL(removed, base_desc);

  removed = tau_typedesc_remove_ptr((tau_typedesc_t*)base_desc);
  TEST_ASSERT_PTR_EQUAL(removed, base_desc);

  tau_typedesc_free((tau_typedesc_t*)base_desc);
  tau_typedesc_free((tau_typedesc_t*)desc);
}

TEST_CASE(tau_typedesc_remove_array)
{
  tau_typedesc_array_t* desc = tau_typedesc_array_init();
  tau_typedesc_prim_t* base_desc = tau_typedesc_prim_unit_init();

  desc->base_type = (tau_typedesc_t*)base_desc;

  tau_typedesc_t* removed = tau_typedesc_remove_array((tau_typedesc_t*)desc);
  TEST_ASSERT_PTR_EQUAL(removed, base_desc);

  removed = tau_typedesc_remove_array((tau_typedesc_t*)base_desc);
  TEST_ASSERT_PTR_EQUAL(removed, base_desc);

  tau_typedesc_free((tau_typedesc_t*)base_desc);
  tau_typedesc_free((tau_typedesc_t*)desc);
}

TEST_CASE(tau_typedesc_remove_ref)
{
  tau_typedesc_ref_t* desc = tau_typedesc_ref_init();
  tau_typedesc_prim_t* base_desc = tau_typedesc_prim_unit_init();

  desc->base_type = (tau_typedesc_t*)base_desc;

  tau_typedesc_t* removed = tau_typedesc_remove_ref((tau_typedesc_t*)desc);
  TEST_ASSERT_PTR_EQUAL(removed, base_desc);

  removed = tau_typedesc_remove_ref((tau_typedesc_t*)base_desc);
  TEST_ASSERT_PTR_EQUAL(removed, base_desc);

  tau_typedesc_free((tau_typedesc_t*)base_desc);
  tau_typedesc_free((tau_typedesc_t*)desc);
}

TEST_CASE(tau_typedesc_remove_opt)
{
  tau_typedesc_opt_t* desc = tau_typedesc_opt_init();
  tau_typedesc_prim_t* base_desc = tau_typedesc_prim_unit_init();

  desc->base_type = (tau_typedesc_t*)base_desc;

  tau_typedesc_t* removed = tau_typedesc_remove_opt((tau_typedesc_t*)desc);
  TEST_ASSERT_PTR_EQUAL(removed, base_desc);

  removed = tau_typedesc_remove_opt((tau_typedesc_t*)base_desc);
  TEST_ASSERT_PTR_EQUAL(removed, base_desc);

  tau_typedesc_free((tau_typedesc_t*)base_desc);
  tau_typedesc_free((tau_typedesc_t*)desc);
}

TEST_CASE(tau_typedesc_remove_ref_mut)
{
  tau_typedesc_ref_t* ref_desc = tau_typedesc_ref_init();
  tau_typedesc_mut_t* mut_desc = tau_typedesc_mut_init();
  tau_typedesc_prim_t* base_desc = tau_typedesc_prim_unit_init();

  ref_desc->base_type = (tau_typedesc_t*)mut_desc;
  mut_desc->base_type = (tau_typedesc_t*)base_desc;

  tau_typedesc_t* removed = tau_typedesc_remove_ref_mut((tau_typedesc_t*)ref_desc);
  TEST_ASSERT_PTR_EQUAL(removed, base_desc);

  removed = tau_typedesc_remove_ref_mut((tau_typedesc_t*)mut_desc);
  TEST_ASSERT_PTR_EQUAL(removed, base_desc);

  removed = tau_typedesc_remove_ref_mut((tau_typedesc_t*)base_desc);
  TEST_ASSERT_PTR_EQUAL(removed, base_desc);

  tau_typedesc_free((tau_typedesc_t*)base_desc);
  tau_typedesc_free((tau_typedesc_t*)mut_desc);
  tau_typedesc_free((tau_typedesc_t*)ref_desc);
}

TEST_CASE(tau_typedesc_can_add_mut)
{
  tau_typedesc_ptr_t* ptr_desc = tau_typedesc_ptr_init();
  tau_typedesc_opt_t* opt_desc = tau_typedesc_opt_init();
  tau_typedesc_prim_t* unit_desc = tau_typedesc_prim_unit_init();

  TEST_ASSERT_TRUE(tau_typedesc_can_add_mut((tau_typedesc_t*)ptr_desc));
  TEST_ASSERT_TRUE(tau_typedesc_can_add_mut((tau_typedesc_t*)opt_desc));
  TEST_ASSERT_TRUE(tau_typedesc_can_add_mut((tau_typedesc_t*)unit_desc));

  tau_typedesc_free((tau_typedesc_t*)unit_desc);
  tau_typedesc_free((tau_typedesc_t*)opt_desc);
  tau_typedesc_free((tau_typedesc_t*)ptr_desc);
}

TEST_CASE(tau_typedesc_cannot_add_mut)
{
  tau_typedesc_mut_t* mut_desc = tau_typedesc_mut_init();
  tau_typedesc_ref_t* ref_desc = tau_typedesc_ref_init();

  TEST_ASSERT_FALSE(tau_typedesc_can_add_mut((tau_typedesc_t*)mut_desc));
  TEST_ASSERT_FALSE(tau_typedesc_can_add_mut((tau_typedesc_t*)ref_desc));

  tau_typedesc_free((tau_typedesc_t*)mut_desc);
  tau_typedesc_free((tau_typedesc_t*)ref_desc);
}

TEST_CASE(tau_typedesc_can_add_ptr)
{
  tau_typedesc_mut_t* mut_desc = tau_typedesc_mut_init();
  tau_typedesc_ptr_t* ptr_desc = tau_typedesc_ptr_init();
  tau_typedesc_opt_t* opt_desc = tau_typedesc_opt_init();
  tau_typedesc_prim_t* unit_desc = tau_typedesc_prim_unit_init();

  mut_desc->base_type = (tau_typedesc_t*)unit_desc;

  TEST_ASSERT_TRUE(tau_typedesc_can_add_ptr((tau_typedesc_t*)mut_desc));
  TEST_ASSERT_TRUE(tau_typedesc_can_add_ptr((tau_typedesc_t*)ptr_desc));
  TEST_ASSERT_TRUE(tau_typedesc_can_add_ptr((tau_typedesc_t*)opt_desc));
  TEST_ASSERT_TRUE(tau_typedesc_can_add_ptr((tau_typedesc_t*)unit_desc));

  tau_typedesc_free((tau_typedesc_t*)unit_desc);
  tau_typedesc_free((tau_typedesc_t*)opt_desc);
  tau_typedesc_free((tau_typedesc_t*)ptr_desc);
  tau_typedesc_free((tau_typedesc_t*)mut_desc);
}

TEST_CASE(tau_typedesc_cannot_add_ptr)
{
  tau_typedesc_ref_t* ref_desc = tau_typedesc_ref_init();

  TEST_ASSERT_FALSE(tau_typedesc_can_add_ptr((tau_typedesc_t*)ref_desc));

  tau_typedesc_free((tau_typedesc_t*)ref_desc);
}

TEST_CASE(tau_typedesc_can_add_array)
{
  tau_typedesc_mut_t* mut_desc = tau_typedesc_mut_init();
  tau_typedesc_ptr_t* ptr_desc = tau_typedesc_ptr_init();
  tau_typedesc_opt_t* opt_desc = tau_typedesc_opt_init();
  tau_typedesc_prim_t* unit_desc = tau_typedesc_prim_unit_init();

  mut_desc->base_type = (tau_typedesc_t*)unit_desc;

  TEST_ASSERT_TRUE(tau_typedesc_can_add_array((tau_typedesc_t*)mut_desc));
  TEST_ASSERT_TRUE(tau_typedesc_can_add_array((tau_typedesc_t*)ptr_desc));
  TEST_ASSERT_TRUE(tau_typedesc_can_add_array((tau_typedesc_t*)opt_desc));
  TEST_ASSERT_TRUE(tau_typedesc_can_add_array((tau_typedesc_t*)unit_desc));

  tau_typedesc_free((tau_typedesc_t*)unit_desc);
  tau_typedesc_free((tau_typedesc_t*)opt_desc);
  tau_typedesc_free((tau_typedesc_t*)ptr_desc);
  tau_typedesc_free((tau_typedesc_t*)mut_desc);
}

TEST_CASE(tau_typedesc_cannot_add_array)
{
  tau_typedesc_ref_t* ref_desc = tau_typedesc_ref_init();

  TEST_ASSERT_FALSE(tau_typedesc_can_add_array((tau_typedesc_t*)ref_desc));

  tau_typedesc_free((tau_typedesc_t*)ref_desc);
}

TEST_CASE(tau_typedesc_can_add_opt)
{
  tau_typedesc_ptr_t* ptr_desc = tau_typedesc_ptr_init();
  tau_typedesc_prim_t* unit_desc = tau_typedesc_prim_unit_init();

  TEST_ASSERT_TRUE(tau_typedesc_can_add_opt((tau_typedesc_t*)ptr_desc));
  TEST_ASSERT_TRUE(tau_typedesc_can_add_opt((tau_typedesc_t*)unit_desc));

  tau_typedesc_free((tau_typedesc_t*)unit_desc);
  tau_typedesc_free((tau_typedesc_t*)ptr_desc);
}

TEST_CASE(tau_typedesc_cannot_add_opt)
{
  tau_typedesc_mut_t* mut_desc = tau_typedesc_mut_init();
  tau_typedesc_ref_t* ref_desc = tau_typedesc_ref_init();
  tau_typedesc_opt_t* opt_desc = tau_typedesc_opt_init();

  mut_desc->base_type = (tau_typedesc_t*)ref_desc;
  TEST_ASSERT_FALSE(tau_typedesc_can_add_opt((tau_typedesc_t*)mut_desc));

  mut_desc->base_type = (tau_typedesc_t*)opt_desc;
  TEST_ASSERT_FALSE(tau_typedesc_can_add_opt((tau_typedesc_t*)mut_desc));

  TEST_ASSERT_FALSE(tau_typedesc_can_add_opt((tau_typedesc_t*)ref_desc));

  tau_typedesc_free((tau_typedesc_t*)opt_desc);
  tau_typedesc_free((tau_typedesc_t*)ref_desc);
  tau_typedesc_free((tau_typedesc_t*)mut_desc);
}

TEST_CASE(tau_typedesc_integer_bits)
{
  tau_typedesc_prim_t* i8_desc    = tau_typedesc_prim_i8_init();
        tau_typedesc_prim_t* i16_desc   = tau_typedesc_prim_i16_init();
        tau_typedesc_prim_t* i32_desc   = tau_typedesc_prim_i32_init();
        tau_typedesc_prim_t* i64_desc   = tau_typedesc_prim_i64_init();
        tau_typedesc_prim_t* isize_desc = tau_typedesc_prim_isize_init();
        tau_typedesc_prim_t* u8_desc    = tau_typedesc_prim_u8_init();
        tau_typedesc_prim_t* u16_desc   = tau_typedesc_prim_u16_init();
        tau_typedesc_prim_t* u32_desc   = tau_typedesc_prim_u32_init();
        tau_typedesc_prim_t* u64_desc   = tau_typedesc_prim_u64_init();
        tau_typedesc_prim_t* usize_desc = tau_typedesc_prim_usize_init();

        TEST_ASSERT_EQUAL(tau_typedesc_integer_bits((tau_typedesc_t*)i8_desc   ), 8                );
        TEST_ASSERT_EQUAL(tau_typedesc_integer_bits((tau_typedesc_t*)i16_desc  ), 16               );
        TEST_ASSERT_EQUAL(tau_typedesc_integer_bits((tau_typedesc_t*)i32_desc  ), 32               );
        TEST_ASSERT_EQUAL(tau_typedesc_integer_bits((tau_typedesc_t*)i64_desc  ), 64               );
        TEST_ASSERT_EQUAL(tau_typedesc_integer_bits((tau_typedesc_t*)isize_desc), sizeof(void*) * 8);
        TEST_ASSERT_EQUAL(tau_typedesc_integer_bits((tau_typedesc_t*)u8_desc   ), 8                );
        TEST_ASSERT_EQUAL(tau_typedesc_integer_bits((tau_typedesc_t*)u16_desc  ), 16               );
        TEST_ASSERT_EQUAL(tau_typedesc_integer_bits((tau_typedesc_t*)u32_desc  ), 32               );
        TEST_ASSERT_EQUAL(tau_typedesc_integer_bits((tau_typedesc_t*)u64_desc  ), 64               );
        TEST_ASSERT_EQUAL(tau_typedesc_integer_bits((tau_typedesc_t*)usize_desc), sizeof(void*) * 8);

        tau_typedesc_free((tau_typedesc_t*)i8_desc   );
        tau_typedesc_free((tau_typedesc_t*)i16_desc  );
        tau_typedesc_free((tau_typedesc_t*)i32_desc  );
        tau_typedesc_free((tau_typedesc_t*)i64_desc  );
        tau_typedesc_free((tau_typedesc_t*)isize_desc);
        tau_typedesc_free((tau_typedesc_t*)u8_desc   );
        tau_typedesc_free((tau_typedesc_t*)u16_desc  );
        tau_typedesc_free((tau_typedesc_t*)u32_desc  );
        tau_typedesc_free((tau_typedesc_t*)u64_desc  );
        tau_typedesc_free((tau_typedesc_t*)usize_desc);
}

TEST_CASE(tau_typedesc_implicitly_direct_convert_prim_to_self)
{
  tau_typedesc_prim_t* i8_desc    = tau_typedesc_prim_i8_init();
  tau_typedesc_prim_t* i16_desc   = tau_typedesc_prim_i16_init();
  tau_typedesc_prim_t* i32_desc   = tau_typedesc_prim_i32_init();
  tau_typedesc_prim_t* i64_desc   = tau_typedesc_prim_i64_init();
  tau_typedesc_prim_t* isize_desc = tau_typedesc_prim_isize_init();
  tau_typedesc_prim_t* u8_desc    = tau_typedesc_prim_u8_init();
  tau_typedesc_prim_t* u16_desc   = tau_typedesc_prim_u16_init();
  tau_typedesc_prim_t* u32_desc   = tau_typedesc_prim_u32_init();
  tau_typedesc_prim_t* u64_desc   = tau_typedesc_prim_u64_init();
  tau_typedesc_prim_t* usize_desc = tau_typedesc_prim_usize_init();

  TEST_ASSERT_TRUE(tau_typedesc_is_implicitly_direct_convertible((tau_typedesc_t*)i8_desc,    (tau_typedesc_t*)i8_desc   ));
  TEST_ASSERT_TRUE(tau_typedesc_is_implicitly_direct_convertible((tau_typedesc_t*)i16_desc,   (tau_typedesc_t*)i16_desc  ));
  TEST_ASSERT_TRUE(tau_typedesc_is_implicitly_direct_convertible((tau_typedesc_t*)i32_desc,   (tau_typedesc_t*)i32_desc  ));
  TEST_ASSERT_TRUE(tau_typedesc_is_implicitly_direct_convertible((tau_typedesc_t*)i64_desc,   (tau_typedesc_t*)i64_desc  ));
  TEST_ASSERT_TRUE(tau_typedesc_is_implicitly_direct_convertible((tau_typedesc_t*)isize_desc, (tau_typedesc_t*)isize_desc));
  TEST_ASSERT_TRUE(tau_typedesc_is_implicitly_direct_convertible((tau_typedesc_t*)u8_desc,    (tau_typedesc_t*)u8_desc   ));
  TEST_ASSERT_TRUE(tau_typedesc_is_implicitly_direct_convertible((tau_typedesc_t*)u16_desc,   (tau_typedesc_t*)u16_desc  ));
  TEST_ASSERT_TRUE(tau_typedesc_is_implicitly_direct_convertible((tau_typedesc_t*)u32_desc,   (tau_typedesc_t*)u32_desc  ));
  TEST_ASSERT_TRUE(tau_typedesc_is_implicitly_direct_convertible((tau_typedesc_t*)u64_desc,   (tau_typedesc_t*)u64_desc  ));
  TEST_ASSERT_TRUE(tau_typedesc_is_implicitly_direct_convertible((tau_typedesc_t*)usize_desc, (tau_typedesc_t*)usize_desc));

  tau_typedesc_free((tau_typedesc_t*)i8_desc   );
  tau_typedesc_free((tau_typedesc_t*)i16_desc  );
  tau_typedesc_free((tau_typedesc_t*)i32_desc  );
  tau_typedesc_free((tau_typedesc_t*)i64_desc  );
  tau_typedesc_free((tau_typedesc_t*)isize_desc);
  tau_typedesc_free((tau_typedesc_t*)u8_desc   );
  tau_typedesc_free((tau_typedesc_t*)u16_desc  );
  tau_typedesc_free((tau_typedesc_t*)u32_desc  );
  tau_typedesc_free((tau_typedesc_t*)u64_desc  );
  tau_typedesc_free((tau_typedesc_t*)usize_desc);
}

TEST_CASE(tau_typedesc_implicitly_direct_convert_mut_to_immut)
{
  tau_typedesc_prim_t* unit_desc = tau_typedesc_prim_unit_init();
  tau_typedesc_mut_t* mut_desc = tau_typedesc_mut_init();

  mut_desc->base_type = (tau_typedesc_t*)unit_desc;

  TEST_ASSERT_TRUE(tau_typedesc_is_implicitly_direct_convertible((tau_typedesc_t*)mut_desc, (tau_typedesc_t*)unit_desc));

  tau_typedesc_free((tau_typedesc_t*)mut_desc);
  tau_typedesc_free((tau_typedesc_t*)unit_desc);
}

TEST_CASE(tau_typedesc_implicitly_direct_convert_immut_to_mut)
{
  tau_typedesc_prim_t* unit_desc = tau_typedesc_prim_unit_init();
  tau_typedesc_mut_t* mut_desc = tau_typedesc_mut_init();

  mut_desc->base_type = (tau_typedesc_t*)unit_desc;

  TEST_ASSERT_TRUE(tau_typedesc_is_implicitly_direct_convertible((tau_typedesc_t*)unit_desc, (tau_typedesc_t*)mut_desc));

  tau_typedesc_free((tau_typedesc_t*)mut_desc);
  tau_typedesc_free((tau_typedesc_t*)unit_desc);
}

TEST_CASE(tau_typedesc_implicitly_direct_convert_ref_mut_to_ref_immut)
{
  tau_typedesc_prim_t* unit_desc = tau_typedesc_prim_unit_init();
  tau_typedesc_ref_t* ref_imm_desc = tau_typedesc_ref_init();
  tau_typedesc_mut_t* mut_desc = tau_typedesc_mut_init();
  tau_typedesc_ref_t* ref_mut_desc = tau_typedesc_ref_init();

  ref_imm_desc->base_type = (tau_typedesc_t*)unit_desc;
  mut_desc->base_type = (tau_typedesc_t*)unit_desc;
  ref_mut_desc->base_type = (tau_typedesc_t*)mut_desc;

  TEST_ASSERT_TRUE(tau_typedesc_is_implicitly_direct_convertible((tau_typedesc_t*)ref_mut_desc, (tau_typedesc_t*)ref_imm_desc));

  tau_typedesc_free((tau_typedesc_t*)ref_mut_desc);
  tau_typedesc_free((tau_typedesc_t*)mut_desc);
  tau_typedesc_free((tau_typedesc_t*)ref_imm_desc);
  tau_typedesc_free((tau_typedesc_t*)unit_desc);
}

TEST_CASE(tau_typedesc_implicitly_direct_convert_ref_immut_to_ref_mut)
{
  tau_typedesc_prim_t* unit_desc = tau_typedesc_prim_unit_init();
  tau_typedesc_ref_t* ref_imm_desc = tau_typedesc_ref_init();
  tau_typedesc_mut_t* mut_desc = tau_typedesc_mut_init();
  tau_typedesc_ref_t* ref_mut_desc = tau_typedesc_ref_init();

  ref_imm_desc->base_type = (tau_typedesc_t*)unit_desc;
  mut_desc->base_type = (tau_typedesc_t*)unit_desc;
  ref_mut_desc->base_type = (tau_typedesc_t*)mut_desc;

  TEST_ASSERT_FALSE(tau_typedesc_is_implicitly_direct_convertible((tau_typedesc_t*)ref_imm_desc, (tau_typedesc_t*)ref_mut_desc));

  tau_typedesc_free((tau_typedesc_t*)ref_mut_desc);
  tau_typedesc_free((tau_typedesc_t*)mut_desc);
  tau_typedesc_free((tau_typedesc_t*)ref_imm_desc);
  tau_typedesc_free((tau_typedesc_t*)unit_desc);
}

TEST_CASE(tau_typedesc_implicitly_direct_convert_ptr_mut_to_ptr_immut)
{
  tau_typedesc_prim_t* unit_desc = tau_typedesc_prim_unit_init();
  tau_typedesc_ptr_t* ptr_imm_desc = tau_typedesc_ptr_init();
  tau_typedesc_mut_t* mut_desc = tau_typedesc_mut_init();
  tau_typedesc_ptr_t* ptr_mut_desc = tau_typedesc_ptr_init();

  ptr_imm_desc->base_type = (tau_typedesc_t*)unit_desc;
  mut_desc->base_type = (tau_typedesc_t*)unit_desc;
  ptr_mut_desc->base_type = (tau_typedesc_t*)mut_desc;

  TEST_ASSERT_TRUE(tau_typedesc_is_implicitly_direct_convertible((tau_typedesc_t*)ptr_mut_desc, (tau_typedesc_t*)ptr_imm_desc));

  tau_typedesc_free((tau_typedesc_t*)ptr_mut_desc);
  tau_typedesc_free((tau_typedesc_t*)mut_desc);
  tau_typedesc_free((tau_typedesc_t*)ptr_imm_desc);
  tau_typedesc_free((tau_typedesc_t*)unit_desc);
}

TEST_CASE(tau_typedesc_implicitly_direct_convert_ptr_immut_to_ptr_mut)
{
  tau_typedesc_prim_t* unit_desc = tau_typedesc_prim_unit_init();
  tau_typedesc_ptr_t* ptr_imm_desc = tau_typedesc_ptr_init();
  tau_typedesc_mut_t* mut_desc = tau_typedesc_mut_init();
  tau_typedesc_ptr_t* ptr_mut_desc = tau_typedesc_ptr_init();

  ptr_imm_desc->base_type = (tau_typedesc_t*)unit_desc;
  mut_desc->base_type = (tau_typedesc_t*)unit_desc;
  ptr_mut_desc->base_type = (tau_typedesc_t*)mut_desc;

  TEST_ASSERT_FALSE(tau_typedesc_is_implicitly_direct_convertible((tau_typedesc_t*)ptr_imm_desc, (tau_typedesc_t*)ptr_mut_desc));

  tau_typedesc_free((tau_typedesc_t*)ptr_mut_desc);
  tau_typedesc_free((tau_typedesc_t*)mut_desc);
  tau_typedesc_free((tau_typedesc_t*)ptr_imm_desc);
  tau_typedesc_free((tau_typedesc_t*)unit_desc);
}

TEST_CASE(tau_typedesc_implicitly_direct_convert_ref_mut_ptr_mut_to_ref_mut_ptr_immut)
{
  tau_typedesc_prim_t* unit_desc = tau_typedesc_prim_unit_init();
  tau_typedesc_ptr_t* ptr_imm_desc = tau_typedesc_ptr_init();
  tau_typedesc_mut_t* mut_unit_desc = tau_typedesc_mut_init();
  tau_typedesc_mut_t* mut_ptr_desc = tau_typedesc_mut_init();
  tau_typedesc_ptr_t* ptr_mut_desc = tau_typedesc_ptr_init();
  tau_typedesc_ref_t* ref_ptr_mut_desc = tau_typedesc_ref_init();
  tau_typedesc_ref_t* ref_ptr_imm_desc = tau_typedesc_ref_init();

  ptr_imm_desc->base_type = (tau_typedesc_t*)unit_desc;
  ref_ptr_imm_desc->base_type = (tau_typedesc_t*)ptr_imm_desc;

  mut_unit_desc->base_type = (tau_typedesc_t*)unit_desc;
  ptr_mut_desc->base_type = (tau_typedesc_t*)mut_unit_desc;
  mut_ptr_desc->base_type = (tau_typedesc_t*)ptr_mut_desc;
  ref_ptr_mut_desc->base_type = (tau_typedesc_t*)mut_ptr_desc;

  TEST_ASSERT_TRUE(tau_typedesc_is_implicitly_direct_convertible((tau_typedesc_t*)ref_ptr_mut_desc, (tau_typedesc_t*)ref_ptr_imm_desc));

  tau_typedesc_free((tau_typedesc_t*)ref_ptr_imm_desc);
  tau_typedesc_free((tau_typedesc_t*)ref_ptr_mut_desc);
  tau_typedesc_free((tau_typedesc_t*)ptr_mut_desc);
  tau_typedesc_free((tau_typedesc_t*)mut_ptr_desc);
  tau_typedesc_free((tau_typedesc_t*)mut_unit_desc);
  tau_typedesc_free((tau_typedesc_t*)ptr_imm_desc);
  tau_typedesc_free((tau_typedesc_t*)unit_desc);
}

TEST_CASE(tau_typedesc_implicitly_direct_convert_ref_mut_ptr_immut_to_ref_mut_ptr_mut)
{
  tau_typedesc_prim_t* unit_desc = tau_typedesc_prim_unit_init();
  tau_typedesc_ptr_t* ptr_imm_desc = tau_typedesc_ptr_init();
  tau_typedesc_mut_t* mut_unit_desc = tau_typedesc_mut_init();
  tau_typedesc_mut_t* mut_ptr_desc = tau_typedesc_mut_init();
  tau_typedesc_ptr_t* ptr_mut_desc = tau_typedesc_ptr_init();
  tau_typedesc_ref_t* ref_ptr_mut_desc = tau_typedesc_ref_init();
  tau_typedesc_ref_t* ref_ptr_imm_desc = tau_typedesc_ref_init();

  ptr_imm_desc->base_type = (tau_typedesc_t*)unit_desc;
  ref_ptr_imm_desc->base_type = (tau_typedesc_t*)ptr_imm_desc;

  mut_unit_desc->base_type = (tau_typedesc_t*)unit_desc;
  ptr_mut_desc->base_type = (tau_typedesc_t*)mut_unit_desc;
  mut_ptr_desc->base_type = (tau_typedesc_t*)ptr_mut_desc;
  ref_ptr_mut_desc->base_type = (tau_typedesc_t*)mut_ptr_desc;

  TEST_ASSERT_FALSE(tau_typedesc_is_implicitly_direct_convertible((tau_typedesc_t*)ref_ptr_imm_desc, (tau_typedesc_t*)ref_ptr_mut_desc));

  tau_typedesc_free((tau_typedesc_t*)ref_ptr_imm_desc);
  tau_typedesc_free((tau_typedesc_t*)ref_ptr_mut_desc);
  tau_typedesc_free((tau_typedesc_t*)ptr_mut_desc);
  tau_typedesc_free((tau_typedesc_t*)mut_ptr_desc);
  tau_typedesc_free((tau_typedesc_t*)mut_unit_desc);
  tau_typedesc_free((tau_typedesc_t*)ptr_imm_desc);
  tau_typedesc_free((tau_typedesc_t*)unit_desc);
}

TEST_CASE(tau_typedesc_implicitly_direct_convert_ref_mut_ptr_mut_to_ref_immut_ptr_mut)
{
  tau_typedesc_prim_t* unit_desc = tau_typedesc_prim_unit_init();
  tau_typedesc_ref_t* ref_imm_desc = tau_typedesc_ref_init();
  tau_typedesc_mut_t* mut_unit_desc = tau_typedesc_mut_init();
  tau_typedesc_mut_t* mut_ptr_desc = tau_typedesc_mut_init();
  tau_typedesc_ptr_t* ptr_mut_desc = tau_typedesc_ptr_init();
  tau_typedesc_ref_t* ref_mut_desc = tau_typedesc_ref_init();

  ref_imm_desc->base_type = (tau_typedesc_t*)ptr_mut_desc;

  mut_unit_desc->base_type = (tau_typedesc_t*)unit_desc;
  ptr_mut_desc->base_type = (tau_typedesc_t*)mut_unit_desc;
  mut_ptr_desc->base_type = (tau_typedesc_t*)ptr_mut_desc;
  ref_mut_desc->base_type = (tau_typedesc_t*)mut_ptr_desc;

  TEST_ASSERT_TRUE(tau_typedesc_is_implicitly_direct_convertible((tau_typedesc_t*)ref_mut_desc, (tau_typedesc_t*)ref_imm_desc));

  tau_typedesc_free((tau_typedesc_t*)ref_mut_desc);
  tau_typedesc_free((tau_typedesc_t*)ptr_mut_desc);
  tau_typedesc_free((tau_typedesc_t*)mut_ptr_desc);
  tau_typedesc_free((tau_typedesc_t*)mut_unit_desc);
  tau_typedesc_free((tau_typedesc_t*)ref_imm_desc);
  tau_typedesc_free((tau_typedesc_t*)unit_desc);
}

TEST_CASE(tau_typedesc_implicitly_direct_convert_ref_immut_ptr_mut_to_ref_mut_ptr_mut)
{
  tau_typedesc_prim_t* unit_desc = tau_typedesc_prim_unit_init();
  tau_typedesc_ref_t* ref_imm_desc = tau_typedesc_ref_init();
  tau_typedesc_mut_t* mut_unit_desc = tau_typedesc_mut_init();
  tau_typedesc_mut_t* mut_ptr_desc = tau_typedesc_mut_init();
  tau_typedesc_ptr_t* ptr_mut_desc = tau_typedesc_ptr_init();
  tau_typedesc_ref_t* ref_mut_desc = tau_typedesc_ref_init();

  ref_imm_desc->base_type = (tau_typedesc_t*)ptr_mut_desc;

  mut_unit_desc->base_type = (tau_typedesc_t*)unit_desc;
  ptr_mut_desc->base_type = (tau_typedesc_t*)mut_unit_desc;
  mut_ptr_desc->base_type = (tau_typedesc_t*)ptr_mut_desc;
  ref_mut_desc->base_type = (tau_typedesc_t*)mut_ptr_desc;

  TEST_ASSERT_FALSE(tau_typedesc_is_implicitly_direct_convertible((tau_typedesc_t*)ref_imm_desc, (tau_typedesc_t*)ref_mut_desc));

  tau_typedesc_free((tau_typedesc_t*)ref_mut_desc);
  tau_typedesc_free((tau_typedesc_t*)ptr_mut_desc);
  tau_typedesc_free((tau_typedesc_t*)mut_ptr_desc);
  tau_typedesc_free((tau_typedesc_t*)mut_unit_desc);
  tau_typedesc_free((tau_typedesc_t*)ref_imm_desc);
  tau_typedesc_free((tau_typedesc_t*)unit_desc);
}

TEST_CASE(tau_typedesc_implicitly_direct_convert_ref_mut_ptr_mut_to_ref_immut_ptr_immut)
{
  tau_typedesc_prim_t* unit_desc = tau_typedesc_prim_unit_init();
  tau_typedesc_ptr_t* ptr_imm_desc = tau_typedesc_ptr_init();
  tau_typedesc_ref_t* ref_imm_desc = tau_typedesc_ref_init();
  tau_typedesc_mut_t* mut_unit_desc = tau_typedesc_mut_init();
  tau_typedesc_mut_t* mut_ptr_desc = tau_typedesc_mut_init();
  tau_typedesc_ptr_t* ptr_mut_desc = tau_typedesc_ptr_init();
  tau_typedesc_ref_t* ref_mut_desc = tau_typedesc_ref_init();

  ptr_imm_desc->base_type = (tau_typedesc_t*)unit_desc;
  ref_imm_desc->base_type = (tau_typedesc_t*)ptr_imm_desc;

  mut_unit_desc->base_type = (tau_typedesc_t*)unit_desc;
  ptr_mut_desc->base_type = (tau_typedesc_t*)mut_unit_desc;
  mut_ptr_desc->base_type = (tau_typedesc_t*)ptr_mut_desc;
  ref_mut_desc->base_type = (tau_typedesc_t*)mut_ptr_desc;

  TEST_ASSERT_TRUE(tau_typedesc_is_implicitly_direct_convertible((tau_typedesc_t*)ref_mut_desc, (tau_typedesc_t*)ref_imm_desc));

  tau_typedesc_free((tau_typedesc_t*)ref_mut_desc);
  tau_typedesc_free((tau_typedesc_t*)ptr_mut_desc);
  tau_typedesc_free((tau_typedesc_t*)mut_ptr_desc);
  tau_typedesc_free((tau_typedesc_t*)mut_unit_desc);
  tau_typedesc_free((tau_typedesc_t*)ref_imm_desc);
  tau_typedesc_free((tau_typedesc_t*)ptr_imm_desc);
  tau_typedesc_free((tau_typedesc_t*)unit_desc);
}

TEST_CASE(tau_typedesc_implicitly_direct_convert_ref_immut_ptr_immut_to_ref_mut_ptr_mut)
{
  tau_typedesc_prim_t* unit_desc = tau_typedesc_prim_unit_init();
  tau_typedesc_ptr_t* ptr_imm_desc = tau_typedesc_ptr_init();
  tau_typedesc_ref_t* ref_imm_desc = tau_typedesc_ref_init();
  tau_typedesc_mut_t* mut_unit_desc = tau_typedesc_mut_init();
  tau_typedesc_mut_t* mut_ptr_desc = tau_typedesc_mut_init();
  tau_typedesc_ptr_t* ptr_mut_desc = tau_typedesc_ptr_init();
  tau_typedesc_ref_t* ref_mut_desc = tau_typedesc_ref_init();

  ptr_imm_desc->base_type = (tau_typedesc_t*)unit_desc;
  ref_imm_desc->base_type = (tau_typedesc_t*)ptr_imm_desc;

  mut_unit_desc->base_type = (tau_typedesc_t*)unit_desc;
  ptr_mut_desc->base_type = (tau_typedesc_t*)mut_unit_desc;
  mut_ptr_desc->base_type = (tau_typedesc_t*)ptr_mut_desc;
  ref_mut_desc->base_type = (tau_typedesc_t*)mut_ptr_desc;

  TEST_ASSERT_FALSE(tau_typedesc_is_implicitly_direct_convertible((tau_typedesc_t*)ref_imm_desc, (tau_typedesc_t*)ref_mut_desc));

  tau_typedesc_free((tau_typedesc_t*)ref_mut_desc);
  tau_typedesc_free((tau_typedesc_t*)ptr_mut_desc);
  tau_typedesc_free((tau_typedesc_t*)mut_ptr_desc);
  tau_typedesc_free((tau_typedesc_t*)mut_unit_desc);
  tau_typedesc_free((tau_typedesc_t*)ref_imm_desc);
  tau_typedesc_free((tau_typedesc_t*)ptr_imm_desc);
  tau_typedesc_free((tau_typedesc_t*)unit_desc);
}

TEST_MAIN()
{
  TEST_RUN(tau_typedesc_remove_mut);
  TEST_RUN(tau_typedesc_remove_ptr);
  TEST_RUN(tau_typedesc_remove_array);
  TEST_RUN(tau_typedesc_remove_ref);
  TEST_RUN(tau_typedesc_remove_opt);
  TEST_RUN(tau_typedesc_remove_ref_mut);
  TEST_RUN(tau_typedesc_can_add_mut);
  TEST_RUN(tau_typedesc_cannot_add_mut);
  TEST_RUN(tau_typedesc_can_add_ptr);
  TEST_RUN(tau_typedesc_cannot_add_ptr);
  TEST_RUN(tau_typedesc_can_add_array);
  TEST_RUN(tau_typedesc_cannot_add_array);
  TEST_RUN(tau_typedesc_can_add_opt);
  TEST_RUN(tau_typedesc_cannot_add_opt);
  TEST_RUN(tau_typedesc_integer_bits);
  TEST_RUN(tau_typedesc_implicitly_direct_convert_prim_to_self);
  TEST_RUN(tau_typedesc_implicitly_direct_convert_mut_to_immut);
  TEST_RUN(tau_typedesc_implicitly_direct_convert_immut_to_mut);
  TEST_RUN(tau_typedesc_implicitly_direct_convert_ref_mut_to_ref_immut);
  TEST_RUN(tau_typedesc_implicitly_direct_convert_ref_immut_to_ref_mut);
  TEST_RUN(tau_typedesc_implicitly_direct_convert_ptr_mut_to_ptr_immut);
  TEST_RUN(tau_typedesc_implicitly_direct_convert_ptr_immut_to_ptr_mut);
  TEST_RUN(tau_typedesc_implicitly_direct_convert_ref_mut_ptr_mut_to_ref_mut_ptr_immut);
  TEST_RUN(tau_typedesc_implicitly_direct_convert_ref_mut_ptr_immut_to_ref_mut_ptr_mut);
  TEST_RUN(tau_typedesc_implicitly_direct_convert_ref_mut_ptr_mut_to_ref_immut_ptr_mut);
  TEST_RUN(tau_typedesc_implicitly_direct_convert_ref_immut_ptr_mut_to_ref_mut_ptr_mut);
  TEST_RUN(tau_typedesc_implicitly_direct_convert_ref_mut_ptr_mut_to_ref_immut_ptr_immut);
  TEST_RUN(tau_typedesc_implicitly_direct_convert_ref_immut_ptr_immut_to_ref_mut_ptr_mut);
}
