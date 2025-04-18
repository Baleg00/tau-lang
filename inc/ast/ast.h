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
#include "ast/decl/generic/fun.h"
#include "ast/decl/generic/param.h"
#include "ast/decl/mod.h"
#include "ast/decl/param.h"
#include "ast/decl/struct.h"
#include "ast/decl/type_alias.h"
#include "ast/decl/union.h"
#include "ast/decl/var.h"
#include "ast/expr/expr.h"
#include "ast/expr/id.h"
#include "ast/expr/lit/bool.h"
#include "ast/expr/lit/char.h"
#include "ast/expr/lit/flt.h"
#include "ast/expr/lit/int.h"
#include "ast/expr/lit/lit.h"
#include "ast/expr/lit/mat.h"
#include "ast/expr/lit/null.h"
#include "ast/expr/lit/str.h"
#include "ast/expr/lit/vec.h"
#include "ast/expr/op/bin/access/direct.h"
#include "ast/expr/op/bin/arit/add.h"
#include "ast/expr/op/bin/arit/div.h"
#include "ast/expr/op/bin/arit/mod.h"
#include "ast/expr/op/bin/arit/mul.h"
#include "ast/expr/op/bin/arit/sub.h"
#include "ast/expr/op/bin/as.h"
#include "ast/expr/op/bin/assign/arit/add.h"
#include "ast/expr/op/bin/assign/arit/div.h"
#include "ast/expr/op/bin/assign/arit/mod.h"
#include "ast/expr/op/bin/assign/arit/mul.h"
#include "ast/expr/op/bin/assign/arit/sub.h"
#include "ast/expr/op/bin/assign/assign.h"
#include "ast/expr/op/bin/assign/bit/and.h"
#include "ast/expr/op/bin/assign/bit/lsh.h"
#include "ast/expr/op/bin/assign/bit/or.h"
#include "ast/expr/op/bin/assign/bit/rsh.h"
#include "ast/expr/op/bin/assign/bit/xor.h"
#include "ast/expr/op/bin/bin.h"
#include "ast/expr/op/bin/bit/and.h"
#include "ast/expr/op/bin/bit/lsh.h"
#include "ast/expr/op/bin/bit/or.h"
#include "ast/expr/op/bin/bit/rsh.h"
#include "ast/expr/op/bin/bit/xor.h"
#include "ast/expr/op/bin/cmp/eq.h"
#include "ast/expr/op/bin/cmp/ge.h"
#include "ast/expr/op/bin/cmp/gt.h"
#include "ast/expr/op/bin/cmp/le.h"
#include "ast/expr/op/bin/cmp/lt.h"
#include "ast/expr/op/bin/cmp/ne.h"
#include "ast/expr/op/bin/logic/and.h"
#include "ast/expr/op/bin/logic/or.h"
#include "ast/expr/op/bin/subs.h"
#include "ast/expr/op/call.h"
#include "ast/expr/op/op.h"
#include "ast/expr/op/spec.h"
#include "ast/expr/op/un/addr.h"
#include "ast/expr/op/un/alignof.h"
#include "ast/expr/op/un/arit/dec_post.h"
#include "ast/expr/op/un/arit/dec_pre.h"
#include "ast/expr/op/un/arit/inc_post.h"
#include "ast/expr/op/un/arit/inc_pre.h"
#include "ast/expr/op/un/arit/neg.h"
#include "ast/expr/op/un/arit/pos.h"
#include "ast/expr/op/un/bit/not.h"
#include "ast/expr/op/un/ind.h"
#include "ast/expr/op/un/logic/not.h"
#include "ast/expr/op/un/sizeof.h"
#include "ast/expr/op/un/un.h"
#include "ast/expr/op/un/unwrap_safe.h"
#include "ast/expr/op/un/unwrap_unsafe.h"
#include "ast/id.h"
#include "ast/node.h"
#include "ast/path/access.h"
#include "ast/path/alias.h"
#include "ast/path/list.h"
#include "ast/path/segment.h"
#include "ast/path/wildcard.h"
#include "ast/poison.h"
#include "ast/prog.h"
#include "ast/stmt/block.h"
#include "ast/stmt/break.h"
#include "ast/stmt/continue.h"
#include "ast/stmt/defer.h"
#include "ast/stmt/do_while.h"
#include "ast/stmt/expr.h"
#include "ast/stmt/for.h"
#include "ast/stmt/if.h"
#include "ast/stmt/loop.h"
#include "ast/stmt/return.h"
#include "ast/stmt/stmt.h"
#include "ast/stmt/while.h"
#include "ast/type/fun.h"
#include "ast/type/id.h"
#include "ast/type/mat.h"
#include "ast/type/mbr.h"
#include "ast/type/modif/array.h"
#include "ast/type/modif/modif.h"
#include "ast/type/modif/mut.h"
#include "ast/type/modif/opt.h"
#include "ast/type/modif/ptr.h"
#include "ast/type/modif/ref.h"
#include "ast/type/prim.h"
#include "ast/type/type.h"
#include "ast/type/type_type.h"
#include "ast/type/vec.h"
#include "ast/use.h"

#endif
