#include "register.h"

#include "util.h"

const char* register_to_string(register_t reg)
{
  switch (reg)
  {
  case REG_A:   return "A";
  case REG_AHD: return "AHD";
  case REG_ALD: return "ALD";
  case REG_AHW: return "AHW";
  case REG_ALW: return "ALW";
  case REG_AHB: return "AHB";
  case REG_ALB: return "ALB";
  case REG_B:   return "B";
  case REG_BHD: return "BHD";
  case REG_BLD: return "BLD";
  case REG_BHW: return "BHW";
  case REG_BLW: return "BLW";
  case REG_BHB: return "BHB";
  case REG_BLB: return "BLB";
  case REG_C:   return "C";
  case REG_CHD: return "CHD";
  case REG_CLD: return "CLD";
  case REG_CHW: return "CHW";
  case REG_CLW: return "CLW";
  case REG_CHB: return "CHB";
  case REG_CLB: return "CLB";
  case REG_D:   return "D";
  case REG_DHD: return "DHD";
  case REG_DLD: return "DLD";
  case REG_DHW: return "DHW";
  case REG_DLW: return "DLW";
  case REG_DHB: return "DHB";
  case REG_DLB: return "DLB";
  case REG_E:   return "E";
  case REG_EHD: return "EHD";
  case REG_ELD: return "ELD";
  case REG_EHW: return "EHW";
  case REG_ELW: return "ELW";
  case REG_EHB: return "EHB";
  case REG_ELB: return "ELB";
  case REG_F:   return "F";
  case REG_FHD: return "FHD";
  case REG_FLD: return "FLD";
  case REG_FHW: return "FHW";
  case REG_FLW: return "FLW";
  case REG_FHB: return "FHB";
  case REG_FLB: return "FLB";
  case REG_SP:  return "SP";
  case REG_BP:  return "BP";
  case REG_IP:  return "IP";
  default: unreachable();
  }

  return NULL;
}

size_t register_bits(register_t reg)
{
  if (register_is_8bit(reg))
    return 8;

  if (register_is_16bit(reg))
    return 16;

  if (register_is_32bit(reg))
    return 32;

  if (register_is_64bit(reg))
    return 64;

  unreachable();

  return 0;
}

bool register_is_8bit(register_t reg)
{
  switch (reg)
  {
  case REG_AHB:
  case REG_ALB:
  case REG_BHB:
  case REG_BLB:
  case REG_CHB:
  case REG_CLB:
  case REG_DHB:
  case REG_DLB:
  case REG_EHB:
  case REG_ELB:
  case REG_FHB:
  case REG_FLB:
    return true;
  default:
    return false;
  }
}

bool register_is_16bit(register_t reg)
{
  switch (reg)
  {
  case REG_AHW:
  case REG_ALW:
  case REG_BHW:
  case REG_BLW:
  case REG_CHW:
  case REG_CLW:
  case REG_DHW:
  case REG_DLW:
  case REG_EHW:
  case REG_ELW:
  case REG_FHW:
  case REG_FLW:
    return true;
  default:
    return false;
  }
}

bool register_is_32bit(register_t reg)
{
  switch (reg)
  {
  case REG_AHD:
  case REG_ALD:
  case REG_BHD:
  case REG_BLD:
  case REG_CHD:
  case REG_CLD:
  case REG_DHD:
  case REG_DLD:
  case REG_EHD:
  case REG_ELD:
  case REG_FHD:
  case REG_FLD:
    return true;
  default:
    return false;
  }
}

bool register_is_64bit(register_t reg)
{
  switch (reg)
  {
  case REG_A:
  case REG_B:
  case REG_C:
  case REG_D:
  case REG_E:
  case REG_F:
  case REG_SP:
  case REG_BP:
  case REG_IP:
    return true;
  default:
    return false;
  }
}
