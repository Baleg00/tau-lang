/**
 * \file typedesc.h
 * 
 * \brief Type descriptor library interface.
 * 
 * \details Type descriptors store information about the characteristics and
 * properties of data types. They provide essential details about a type, such
 * as its size, alignment, modifiers (like const and mut), and its relationship
 * to other types (e.g., pointers, arrays, functions). Type descriptors
 * facilitate type checking, memory allocation, code generation, and other
 * compiler tasks by enabling the compiler to understand and manage the
 * semantics of different data types.
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_TYPEDESC_H
#define TAU_TYPEDESC_H

#include "stages/analysis/types/typedesc/array.h"
#include "stages/analysis/types/typedesc/base.h"
#include "stages/analysis/types/typedesc/const.h"
#include "stages/analysis/types/typedesc/decl.h"
#include "stages/analysis/types/typedesc/enum.h"
#include "stages/analysis/types/typedesc/fun.h"
#include "stages/analysis/types/typedesc/modif.h"
#include "stages/analysis/types/typedesc/mut.h"
#include "stages/analysis/types/typedesc/null.h"
#include "stages/analysis/types/typedesc/opt.h"
#include "stages/analysis/types/typedesc/prim.h"
#include "stages/analysis/types/typedesc/ptr.h"
#include "stages/analysis/types/typedesc/ref.h"
#include "stages/analysis/types/typedesc/struct.h"
#include "stages/analysis/types/typedesc/union.h"

#endif