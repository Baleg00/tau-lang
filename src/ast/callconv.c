/**
 * \file
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
  default: UNREACHABLE();
  }

  return NULL;
}

size_t callconv_mangle(callconv_kind_t kind, char* buf, size_t len)
{
  const char* mangled_callconv = NULL;

  switch (kind)
  {
  case CALLCONV_TAU:        mangled_callconv = "C"; break;
  case CALLCONV_CDECL:      mangled_callconv = "C"; break;
  case CALLCONV_STDCALL:    mangled_callconv = "S"; break;
  case CALLCONV_WIN64:      mangled_callconv = "W"; break;
  case CALLCONV_SYSV64:     mangled_callconv = "Y"; break;
  case CALLCONV_AAPCS:      mangled_callconv = "A"; break;
  case CALLCONV_FASTCALL:   mangled_callconv = "F"; break;
  case CALLCONV_VECTORCALL: mangled_callconv = "V"; break;
  case CALLCONV_THISCALL:   mangled_callconv = "T"; break;
  default: UNREACHABLE();
  }

  return snprintf(buf, len, "%s", mangled_callconv);
}
