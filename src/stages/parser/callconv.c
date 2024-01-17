/**
 * \file callconv.c
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "stages/parser/callconv.h"

#include "utils/common.h"

const char* callconv_kind_to_cstr(callconv_kind_t kind)
{
  switch (kind)
  {
  case CALLCONV_TAU:        return "Tau";
  case CALLCONV_CDECL:      return "cdecl";
  case CALLCONV_STDCALL:    return "stdcall";
  case CALLCONV_WIN64:      return "win64";
  case CALLCONV_SYSV64:     return "sysv64";
  case CALLCONV_AAPCS:      return "aapcs";
  case CALLCONV_FASTCALL:   return "fastcall";
  case CALLCONV_VECTORCALL: return "vectorcall";
  case CALLCONV_THISCALL:   return "thiscall";
  default: unreachable();
  }

  return NULL;
}
