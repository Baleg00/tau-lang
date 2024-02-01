/**
 * \file callconv.c
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "ast/callconv.h"

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

size_t callconv_mangle(callconv_kind_t kind, char* buf, size_t len)
{
  switch (kind)
  {
  case CALLCONV_TAU:        return snprintf(buf, len, "C");
  case CALLCONV_CDECL:      return snprintf(buf, len, "C");
  case CALLCONV_STDCALL:    return snprintf(buf, len, "S");
  case CALLCONV_WIN64:      return snprintf(buf, len, "W");
  case CALLCONV_SYSV64:     return snprintf(buf, len, "Y");
  case CALLCONV_AAPCS:      return snprintf(buf, len, "A");
  case CALLCONV_FASTCALL:   return snprintf(buf, len, "F");
  case CALLCONV_VECTORCALL: return snprintf(buf, len, "V");
  case CALLCONV_THISCALL:   return snprintf(buf, len, "T");
  default: unreachable();
  }

  return 0;
}
