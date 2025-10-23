/**
 * \file
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "ast/callconv.h"

const char* tau_callconv_kind_to_cstr(tau_callconv_kind_t kind)
{
  switch (kind)
  {
  case TAU_CALLCONV_UNKNOWN:    return "unknown";
  case TAU_CALLCONV_TAU:        return "Tau";
  case TAU_CALLCONV_CDECL:      return "cdecl";
  case TAU_CALLCONV_STDCALL:    return "stdcall";
  case TAU_CALLCONV_WIN64:      return "win64";
  case TAU_CALLCONV_SYSV64:     return "sysv64";
  case TAU_CALLCONV_AAPCS:      return "aapcs";
  case TAU_CALLCONV_FASTCALL:   return "fastcall";
  case TAU_CALLCONV_VECTORCALL: return "vectorcall";
  case TAU_CALLCONV_THISCALL:   return "thiscall";
  default: TAU_UNREACHABLE();
  }

  return NULL;
}

size_t tau_callconv_mangle(tau_callconv_kind_t kind, char* buf, size_t len)
{
  const char* mangled_callconv = NULL;

  switch (kind)
  {
  case TAU_CALLCONV_TAU:        mangled_callconv = "C"; break;
  case TAU_CALLCONV_CDECL:      mangled_callconv = "C"; break;
  case TAU_CALLCONV_STDCALL:    mangled_callconv = "S"; break;
  case TAU_CALLCONV_WIN64:      mangled_callconv = "W"; break;
  case TAU_CALLCONV_SYSV64:     mangled_callconv = "Y"; break;
  case TAU_CALLCONV_AAPCS:      mangled_callconv = "A"; break;
  case TAU_CALLCONV_FASTCALL:   mangled_callconv = "F"; break;
  case TAU_CALLCONV_VECTORCALL: mangled_callconv = "V"; break;
  case TAU_CALLCONV_THISCALL:   mangled_callconv = "T"; break;
  default: TAU_UNREACHABLE();
  }

  return snprintf(buf, len, "%s", mangled_callconv);
}
