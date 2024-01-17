/**
 * \file callconv.h
 * 
 * \brief Calling conventions.
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_CALLCONV_H
#define TAU_CALLCONV_H

/**
 * \brief Enumeration of calling convention kinds.
 */
typedef enum callconv_kind_t
{
  CALLCONV_TAU, // The default Tau calling convention.
  CALLCONV_CDECL, // Standard C calling convention, parameters pushed onto the stack.
  CALLCONV_STDCALL, // Win32 API standard calling convention.
  CALLCONV_WIN64, // Windows 64-bit calling convention.
  CALLCONV_SYSV64, // Calling convention of 64-bit Unix-like operating systems.
  CALLCONV_AAPCS, // ARM Architecture Procedure Call Standard.
  CALLCONV_FASTCALL, // Fast parameter passing via registers.
  CALLCONV_VECTORCALL, // Calling convention for SIMD (Single Instruction, Multiple Data) operations.
  CALLCONV_THISCALL, // Windows C++ method calling convention.
} callconv_kind_t;

/**
 * \brief Returns a C-string representation of a calling convention kind.
 * 
 * \param[in] kind The calling convention kind.
 * \returns C-string representation.
*/
const char* callconv_kind_to_cstr(callconv_kind_t kind);

#endif