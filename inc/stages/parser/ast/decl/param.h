/**
 * \file param.h
 * 
 * \brief Abstract syntax tree parameter declaration node interface.
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_AST_DECL_PARAM_H
#define TAU_AST_DECL_PARAM_H

#include <llvm-c/Types.h>

#include "stages/parser/ast/decl/decl.h"

/**
 * \brief AST parameter declaration node.
 */
typedef struct ast_decl_param_t
{
  AST_DECL_HEADER;
  ast_node_t* type; // The associated type.
  ast_node_t* expr; // The associated default value expression (if present).
  bool is_vararg; // Is variadic parameter.

  LLVMTypeRef llvm_type; // The associated LLVM type.
  LLVMValueRef llvm_value; // The associated LLVM value.
} ast_decl_param_t;

/**
 * \brief Initializes a new AST parameter declaration node.
 * 
 * \returns Pointer to the newly initialized AST node.
 */
ast_decl_param_t* ast_decl_param_init(void);

/**
 * \brief Writes a JSON dump of an AST parameter declaration node into a stream.
 * 
 * \param[in] stream The stream to be written to.
 * \param[in] node Pointer to the AST node to be dumped.
*/
void ast_decl_param_dump_json(FILE* stream, ast_decl_param_t* node);

#endif