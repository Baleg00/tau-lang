#include "register.h"

#include "util.h"

uint8_t register_encode(register_t reg)
{
  // Value of `reg` needs to be remapped to fit into 4 bits.
  // When decoding, bit width is used to exactly determine the register.
  switch (reg)
  {
  case REG_A:
  case REG_ALD:
  case REG_ALB:
  case REG_ALW:
    return 0;
  case REG_AHD:
  case REG_AHW:
  case REG_AHB:
    return 1;
  case REG_B:
  case REG_BLD:
  case REG_BLB:
  case REG_BLW:
    return 2;
  case REG_BHD:
  case REG_BHW:
  case REG_BHB:
    return 3;
  case REG_C:
  case REG_CLD:
  case REG_CLB:
  case REG_CLW:
    return 4;
  case REG_CHD:
  case REG_CHW:
  case REG_CHB:
    return 5;
  case REG_D:
  case REG_DLD:
  case REG_DLB:
  case REG_DLW:
    return 6;
  case REG_DHD:
  case REG_DHW:
  case REG_DHB:
    return 7;
  case REG_E:
  case REG_ELD:
  case REG_ELB:
  case REG_ELW:
    return 8;
  case REG_EHD:
  case REG_EHW:
  case REG_EHB:
    return 9;
  case REG_F:
  case REG_FLD:
  case REG_FLB:
  case REG_FLW:
    return 10;
  case REG_FHD:
  case REG_FHW:
  case REG_FHB:
    return 11;
  case REG_SP:
    return 12;
  case REG_BP:
    return 13;
  case REG_IP:
    return 14;
  default:
    unreachable();
  }

  return -1;
}

register_t register_decode(uint8_t data, opcode_width_t width)
{
  switch (width)
  {
  case OPCODE_WIDTH_8BIT:
    switch (data)
    {
    case 0:  return REG_ALB;
    case 1:  return REG_AHB;
    case 2:  return REG_BLB;
    case 3:  return REG_BHB;
    case 4:  return REG_CLB;
    case 5:  return REG_CHB;
    case 6:  return REG_DLB;
    case 7:  return REG_DHB;
    case 8:  return REG_ELB;
    case 9:  return REG_EHB;
    case 10: return REG_FLB;
    case 11: return REG_FHB;
    default: unreachable();
    }
    break;
  case OPCODE_WIDTH_16BIT:
    switch (data)
    {
    case 0:  return REG_ALW;
    case 1:  return REG_AHW;
    case 2:  return REG_BLW;
    case 3:  return REG_BHW;
    case 4:  return REG_CLW;
    case 5:  return REG_CHW;
    case 6:  return REG_DLW;
    case 7:  return REG_DHW;
    case 8:  return REG_ELW;
    case 9:  return REG_EHW;
    case 10: return REG_FLW;
    case 11: return REG_FHW;
    default: unreachable();
    }
    break;
  case OPCODE_WIDTH_32BIT:
    switch (data)
    {
    case 0:  return REG_ALD;
    case 1:  return REG_AHD;
    case 2:  return REG_BLD;
    case 3:  return REG_BHD;
    case 4:  return REG_CLD;
    case 5:  return REG_CHD;
    case 6:  return REG_DLD;
    case 7:  return REG_DHD;
    case 8:  return REG_ELD;
    case 9:  return REG_EHD;
    case 10: return REG_FLD;
    case 11: return REG_FHD;
    default: unreachable();
    }
    break;
  case OPCODE_WIDTH_64BIT:
    switch (data)
    {
    case 0:  return REG_A;
    case 2:  return REG_B;
    case 4:  return REG_C;
    case 6:  return REG_D;
    case 8:  return REG_E;
    case 10: return REG_F;
    case 12: return REG_SP;
    case 13: return REG_BP;
    case 14: return REG_IP;
    default: unreachable();
    }
    break;
  default:
    unreachable();
  }

  return -1;
}

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
