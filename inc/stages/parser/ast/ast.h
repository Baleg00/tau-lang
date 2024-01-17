/**
 * \file ast.h
 * 
 * \brief Abstract syntax tree library interface.
 * 
 * \details The Abstract Syntax Tree (AST) is a hierarchical representation of
 * the structure of source code. It captures the syntactic and semantic
 * relationships among the different elements of the code, such as expressions,
 * statements, and declarations. Each node in the AST corresponds to a language
 * construct, and the tree's structure reflects the code's grammar and
 * organization. The AST is used to analyze and transform the source code during
 * the compilation process. It aids in performing tasks like type checking,
 * optimization, code generation, and error detection.
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_AST_H
#define TAU_AST_H

#include "stages/parser/ast/decl/decl.h"
#include "stages/parser/ast/decl/enum_constant.h"
#include "stages/parser/ast/decl/enum.h"
#include "stages/parser/ast/decl/fun.h"
#include "stages/parser/ast/decl/mod.h"
#include "stages/parser/ast/decl/param.h"
#include "stages/parser/ast/decl/struct.h"
#include "stages/parser/ast/decl/union.h"
#include "stages/parser/ast/decl/var.h"
#include "stages/parser/ast/expr/decl.h"
#include "stages/parser/ast/expr/expr.h"
#include "stages/parser/ast/expr/lit/bool.h"
#include "stages/parser/ast/expr/lit/char.h"
#include "stages/parser/ast/expr/lit/flt.h"
#include "stages/parser/ast/expr/lit/int.h"
#include "stages/parser/ast/expr/lit/lit.h"
#include "stages/parser/ast/expr/lit/null.h"
#include "stages/parser/ast/expr/lit/str.h"
#include "stages/parser/ast/expr/op/bin.h"
#include "stages/parser/ast/expr/op/call.h"
#include "stages/parser/ast/expr/op/op.h"
#include "stages/parser/ast/expr/op/un.h"
#include "stages/parser/ast/id.h"
#include "stages/parser/ast/node.h"
#include "stages/parser/ast/prog.h"
#include "stages/parser/ast/stmt/block.h"
#include "stages/parser/ast/stmt/break.h"
#include "stages/parser/ast/stmt/continue.h"
#include "stages/parser/ast/stmt/defer.h"
#include "stages/parser/ast/stmt/expr.h"
#include "stages/parser/ast/stmt/for.h"
#include "stages/parser/ast/stmt/if.h"
#include "stages/parser/ast/stmt/return.h"
#include "stages/parser/ast/stmt/stmt.h"
#include "stages/parser/ast/stmt/while.h"
#include "stages/parser/ast/type/array.h"
#include "stages/parser/ast/type/const.h"
#include "stages/parser/ast/type/decl.h"
#include "stages/parser/ast/type/fun.h"
#include "stages/parser/ast/type/mbr.h"
#include "stages/parser/ast/type/modif.h"
#include "stages/parser/ast/type/mut.h"
#include "stages/parser/ast/type/opt.h"
#include "stages/parser/ast/type/prim.h"
#include "stages/parser/ast/type/ptr.h"
#include "stages/parser/ast/type/ref.h"
#include "stages/parser/ast/type/type.h"

#endif