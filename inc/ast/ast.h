/**
 * \file
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

#include "ast/decl/decl.h"
#include "ast/decl/enum_constant.h"
#include "ast/decl/enum.h"
#include "ast/decl/fun.h"
#include "ast/decl/mod.h"
#include "ast/decl/param.h"
#include "ast/decl/struct.h"
#include "ast/decl/union.h"
#include "ast/decl/var.h"
#include "ast/expr/expr.h"
#include "ast/expr/id.h"
#include "ast/expr/lit/bool.h"
#include "ast/expr/lit/char.h"
#include "ast/expr/lit/flt.h"
#include "ast/expr/lit/int.h"
#include "ast/expr/lit/lit.h"
#include "ast/expr/lit/null.h"
#include "ast/expr/lit/str.h"
#include "ast/expr/op/bin/access.h"
#include "ast/expr/op/bin/bin.h"
#include "ast/expr/op/call.h"
#include "ast/expr/op/op.h"
#include "ast/expr/op/un.h"
#include "ast/id.h"
#include "ast/node.h"
#include "ast/path/access.h"
#include "ast/path/alias.h"
#include "ast/path/list.h"
#include "ast/path/segment.h"
#include "ast/path/wildcard.h"
#include "ast/prog.h"
#include "ast/stmt/block.h"
#include "ast/stmt/break.h"
#include "ast/stmt/continue.h"
#include "ast/stmt/defer.h"
#include "ast/stmt/expr.h"
#include "ast/stmt/for.h"
#include "ast/stmt/if.h"
#include "ast/stmt/return.h"
#include "ast/stmt/stmt.h"
#include "ast/stmt/while.h"
#include "ast/type/fun.h"
#include "ast/type/id.h"
#include "ast/type/mbr.h"
#include "ast/type/modif/array.h"
#include "ast/type/modif/const.h"
#include "ast/type/modif/modif.h"
#include "ast/type/modif/mut.h"
#include "ast/type/modif/opt.h"
#include "ast/type/modif/ptr.h"
#include "ast/type/modif/ref.h"
#include "ast/type/prim.h"
#include "ast/type/type.h"
#include "ast/use.h"

#endif