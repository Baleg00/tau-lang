#ifndef TAU_TYPEDEFS_H
#define TAU_TYPEDEFS_H

/** Represents a memory arena. */
typedef struct arena_s arena_t;

/** Represents an node in a linked list. */
typedef struct list_node_s list_node_t;

/** Represents a linked list. */
typedef struct list_s list_t;

/** Function pointer type to be used by `list_for_each` function. */
typedef void(*list_for_each_func_t)(void*);

/** Represents a stack. */
typedef list_t stack_t;

/** Represents a queue. */
typedef list_t queue_t;

/**Indicates argument type. */
typedef enum cli_type_e
{
  CLI_TYPE_INTEGER, // Integer type
  CLI_TYPE_FLOAT, // Floating-point type
  CLI_TYPE_BOOLEAN, // Boolean type
  CLI_TYPE_STRING, // String type
  CLI_TYPE_SINK, // Absorbs all following arguments
} cli_type_t;

/** Represents a command-line option. */
typedef struct cli_opt_s cli_opt_t;

/** Represents a command-line interface instance. */
typedef struct cli_s cli_t;

/** Option callback type. */
typedef void(*cli_callback_t)(cli_t*, queue_t*, cli_opt_t*, const char*, void*);

/** Log level indicating the kind and severity of a message. */
typedef enum log_level_e
{
  LOG_LEVEL_TRACE, // Low-level, highly detailed behaviour.
  LOG_LEVEL_DEBUG, // Diagnostic messages.
  LOG_LEVEL_INFO,  // Normal behaviour, informative messages.
  LOG_LEVEL_WARN,  // Abnormal or unexpected behaviour.
  LOG_LEVEL_ERROR, // Severe issue or failure.
  LOG_LEVEL_FATAL, // Critical failure and program termination.
} log_level_t;

/** Indicates the crumb log kind. */
typedef enum crumb_kind_e
{
  CRUMB_WARN, // Undesired code or behaviour.
  CRUMB_ERROR // Illegal code or behaviour.
} crumb_kind_t;

/** Indicates the kind of a crumb log item. */
typedef enum crumb_item_kind_e
{
  CRUMB_ITEM_MESSAGE, // Plain text.
  CRUMB_ITEM_SNIPPET, // Source code snippet.
} crumb_item_kind_t;

/** Represents a crumb log message. */
typedef struct crumb_item_message_s crumb_item_message_t;

/** Represents a crumb log snippet. */
typedef struct crumb_item_snippet_s crumb_item_snippet_t;

/** Type large enough to hold any item kind. */
typedef struct crumb_item_s crumb_item_t;

/** Marks a location in a source file. */
typedef struct location_s location_t;

/** Indicates the category of a token. */
typedef enum token_flag_e
{
  TOK_FLAG_UNKNOWN = 0, // Unknown
  TOK_FLAG_ID      = (1 << 0) << 8, // Identifier
  TOK_FLAG_LIT     = (1 << 1) << 8, // Literal
  TOK_FLAG_KW      = (1 << 2) << 8, // Keyword
  TOK_FLAG_PUNCT   = (1 << 3) << 8, // Punctuation
  TOK_FLAG_EOF     = (1 << 4) << 8 // End of file
} token_flag_t;

/** Indicates the kind of a token. */
typedef enum token_kind_e
{
  TOK_UNKNOWN = TOK_FLAG_UNKNOWN, // Unknown token

  TOK_ID = TOK_FLAG_ID, // Identifier

  TOK_LIT_INT = TOK_FLAG_LIT, // Literal integer (e.g. 123)
  TOK_LIT_FLT, // Literal float (e.g. 1.23)
  TOK_LIT_STR, // Literal string (e.g. "abc")
  TOK_LIT_CHAR, // Literal character (e.g. 'a')
  TOK_LIT_BOOL, // Literal boolean
  TOK_LIT_NULL, // Literal null

  TOK_KW_IS = TOK_FLAG_KW, // Keyword `is`
  TOK_KW_AS, // Keyword `as`
  TOK_KW_SIZEOF, // Keyword `sizeof`
  TOK_KW_ALIGNOF, // Keyword `alignof`
  TOK_KW_TYPEOF, // Keyword `typeof`
  TOK_KW_IN, // Keyword `in`
  TOK_KW_FUN, // Keyword `fun`
  TOK_KW_GEN, // Keyword `gen`
  TOK_KW_STRUCT, // Keyword `struct`
  TOK_KW_UNION, // Keyword `union`
  TOK_KW_ENUM, // Keyword `enum`
  TOK_KW_MOD, // Keyword `mod`
  TOK_KW_USE, // Keyword `use`
  TOK_KW_IF, // Keyword `if`
  TOK_KW_THEN, // Keyword `then`
  TOK_KW_ELSE, // Keyword `else`
  TOK_KW_FOR, // Keyword `for`
  TOK_KW_WHILE, // Keyword `while`
  TOK_KW_DO, // Keyword `do`
  TOK_KW_BREAK, // Keyword `break`
  TOK_KW_CONTINUE, // Keyword `continue`
  TOK_KW_RETURN, // Keyword `return`
  TOK_KW_YIELD, // Keyword `yield`
  TOK_KW_MUT, // Keyword `mut`
  TOK_KW_CONST, // Keyword `const`
  TOK_KW_TYPE, // Keyword `type`
  TOK_KW_SELF, // Keyword `Self`
  TOK_KW_I8, // Keyword `i8`
  TOK_KW_I16, // Keyword `i16`
  TOK_KW_I32, // Keyword `i32`
  TOK_KW_I64, // Keyword `i64`
  TOK_KW_ISIZE, // Keyword `isize`
  TOK_KW_U8, // Keyword `u8`
  TOK_KW_U16, // Keyword `u16`
  TOK_KW_U32, // Keyword `u32`
  TOK_KW_U64, // Keyword `u64`
  TOK_KW_USIZE, // Keyword `usize`
  TOK_KW_F32, // Keyword `f32`
  TOK_KW_F64, // Keyword `f64`
  TOK_KW_BOOL, // Keyword `bool`
  TOK_KW_UNIT, // Keyword `unit`

  TOK_PUNCT_PLUS = TOK_FLAG_PUNCT, // Punctuation `+`
  TOK_PUNCT_PLUS_PLUS, // Punctuation `++`
  TOK_PUNCT_PLUS_EQUAL, // Punctuation `+=`
  TOK_PUNCT_MINUS, // Punctuation `-`
  TOK_PUNCT_MINUS_MINUS, // Punctuation `--`
  TOK_PUNCT_MINUS_EQUAL, // Punctuation `-=`
  TOK_PUNCT_ASTERISK, // Punctuation `*`
  TOK_PUNCT_ASTERISK_EQUAL, // Punctuation `*=`
  TOK_PUNCT_ASTERISK_DOT, // Punctuation `*.`
  TOK_PUNCT_SLASH, // Punctuation `/`
  TOK_PUNCT_SLASH_EQUAL, // Punctuation `/=`
  TOK_PUNCT_PERCENT, // Punctuation `%`
  TOK_PUNCT_PERCENT_EQUAL, // Punctuation `%=`
  TOK_PUNCT_AMPERSAND, // Punctuation `&`
  TOK_PUNCT_AMPERSAND_AMPERSAND, // Punctuation `&&`
  TOK_PUNCT_AMPERSAND_EQUAL, // Punctuation `&=`
  TOK_PUNCT_BAR, // Punctuation `|`
  TOK_PUNCT_BAR_BAR, // Punctuation `||`
  TOK_PUNCT_BAR_EQUAL, // Punctuation `|=`
  TOK_PUNCT_HAT, // Punctuation `^`
  TOK_PUNCT_HAT_EQUAL, // Punctuation `^=`
  TOK_PUNCT_TILDE, // Punctuation `~`
  TOK_PUNCT_LESS, // Punctuation `<`
  TOK_PUNCT_LESS_LESS, // Punctuation `<<`
  TOK_PUNCT_LESS_LESS_EQUAL, // Punctuation `<<=`
  TOK_PUNCT_LESS_EQUAL, // Punctuation `<=`
  TOK_PUNCT_GREATER, // Punctuation `>`
  TOK_PUNCT_GREATER_GREATER, // Punctuation `>>`
  TOK_PUNCT_GREATER_GREATER_EQUAL, // Punctuation `>>=`
  TOK_PUNCT_GREATER_EQUAL, // Punctuation `>=`
  TOK_PUNCT_BANG, // Punctuation `!`
  TOK_PUNCT_BANG_EQUAL, // Punctuation `!=`
  TOK_PUNCT_DOT, // Punctuation `.`
  TOK_PUNCT_DOT_DOT, // Punctuation `..`
  TOK_PUNCT_DOT_DOT_DOT, // Punctuation `...`
  TOK_PUNCT_QUESTION, // Punctuation `?`
  TOK_PUNCT_QUESTION_DOT, // Punctuation `?.`
  TOK_PUNCT_EQUAL, // Punctuation `=`
  TOK_PUNCT_EQUAL_EQUAL, // Punctuation `==`
  TOK_PUNCT_COMMA, // Punctuation `,`
  TOK_PUNCT_COLON, // Punctuation `:`
  TOK_PUNCT_SEMICOLON, // Punctuation `;`
  TOK_PUNCT_PAREN_LEFT, // Punctuation `(`
  TOK_PUNCT_PAREN_RIGHT, // Punctuation `)`
  TOK_PUNCT_BRACKET_LEFT, // Punctuation `[`
  TOK_PUNCT_BRACKET_RIGHT, // Punctuation `]`
  TOK_PUNCT_BRACE_LEFT, // Punctuation `{`
  TOK_PUNCT_BRACE_RIGHT, // Punctuation `}`

  TOK_EOF = TOK_FLAG_EOF // End of file
} token_kind_t;

/** Base type for all tokens. */
typedef struct token_s token_t;

/** Token type for identifiers. */
typedef struct token_id_s token_id_t;

/** Token type for integer literals. */
typedef struct token_lit_int_s token_lit_int_t;

/** Token type for floating-point literals. */
typedef struct token_lit_flt_s token_lit_flt_t;

/** Token type for string literals. */
typedef struct token_lit_str_s token_lit_str_t;

/** Token type for character literals. */
typedef struct token_lit_char_s token_lit_char_t;

/** Token type for boolean literals. */
typedef struct token_lit_bool_s token_lit_bool_t;

/** Represents a lexical analyzer. */
typedef struct lexer_s lexer_t;

/** Indicates the category of a node. */
typedef enum ast_flag_e
{
  AST_FLAG_UNKNOWN    = 0, // Unknown
  AST_FLAG_ID         = (1 << 0) << 8, // Identifier
  AST_FLAG_TYPE       = (1 << 1) << 8, // Type
  AST_FLAG_EXPR       = (1 << 2) << 8, // Expression
  AST_FLAG_STMT       = (1 << 3) << 8, // Statement
  AST_FLAG_DECL       = (1 << 4) << 8, // Declaration
  AST_FLAG_PARAM      = (1 << 5) << 8, // Parameter
  AST_FLAG_ENUMERATOR = (1 << 6) << 8, // Enumerator
  AST_FLAG_PROG       = (1 << 7) << 8, // Program
} ast_flag_t;

/** Indicates the kind of a node. */
typedef enum ast_kind_e
{
  AST_UNKNOWN = AST_FLAG_UNKNOWN, // Unknown node

  AST_ID = AST_FLAG_ID, // Identifier

  AST_TYPE_MUT = AST_FLAG_TYPE, // Mutable type
  AST_TYPE_CONST, // Compile-time type
  AST_TYPE_PTR, // Pointer type
  AST_TYPE_ARRAY, // Array type
  AST_TYPE_REF, // Reference type
  AST_TYPE_OPT, // Optional type
  AST_TYPE_FUN, // Function type
  AST_TYPE_GEN, // Generator type
  AST_TYPE_TYPE, // Type type
  AST_TYPE_SELF, // Self type
  AST_TYPE_I8, // Built-in type i8
  AST_TYPE_I16, // Built-in type i16
  AST_TYPE_I32, // Built-in type i32
  AST_TYPE_I64, // Built-in type i64
  AST_TYPE_ISIZE, // Built-in type isize
  AST_TYPE_U8, // Built-in type u8
  AST_TYPE_U16, // Built-in type u16
  AST_TYPE_U32, // Built-in type u32
  AST_TYPE_U64, // Built-in type u64
  AST_TYPE_USIZE, // Built-in type usize
  AST_TYPE_F32, // Built-in type f32
  AST_TYPE_F64, // Built-in type f64
  AST_TYPE_BOOL, // Built-in type bool
  AST_TYPE_UNIT, // Built-in type unit
  AST_TYPE_MEMBER, // Member type

  AST_EXPR_LIT_INT = AST_FLAG_EXPR, // Literal integer expression
  AST_EXPR_LIT_FLT, // Literal float expression
  AST_EXPR_LIT_STR, // Literal string expression
  AST_EXPR_LIT_CHAR, // Literal character expression
  AST_EXPR_LIT_BOOL, // Literal boolean expression
  AST_EXPR_LIT_NULL, // Literal null expression
  AST_EXPR_OP, // Operation expression
  
  AST_STMT_IF = AST_FLAG_STMT, // If-statement
  AST_STMT_FOR, // For-statement
  AST_STMT_WHILE, // While-statement
  AST_STMT_BREAK, // Break-statement
  AST_STMT_CONTINUE, // Continue-statement
  AST_STMT_RETURN, // Return-statement
  AST_STMT_YIELD, // Yield-statement
  AST_STMT_BLOCK, // Block-statement
  AST_STMT_EXPR, // Expression-statement
  
  AST_DECL_VAR = AST_FLAG_DECL, // Variable declaration
  AST_DECL_LOOP_VAR, // For-loop variable declaration
  AST_DECL_FUN, // Function declaration
  AST_DECL_GEN, // Generator declaration
  AST_DECL_STRUCT, // Struct declaration
  AST_DECL_UNION, // Union declaration
  AST_DECL_ENUM, // Enum declaration
  AST_DECL_MOD, // Module declaration
  AST_DECL_GENERIC, // Generic declaration

  AST_PARAM = AST_FLAG_PARAM, // Function or generator parameter
  AST_PARAM_DEFAULT, // Default parameter
  AST_PARAM_VARIADIC, // Variadic parameter
  AST_PARAM_GENERIC, // Generic parameter

  AST_ENUMERATOR = AST_FLAG_ENUMERATOR, // Enumerator

  AST_PROG = AST_FLAG_PROG, // Program
} ast_kind_t;

/** Base type for all nodes. */
typedef struct ast_node_s ast_node_t;

/** Node type for identifiers. */
typedef struct ast_id_s ast_id_t;

/** Base type for type nodes. */
typedef struct ast_type_s ast_type_t;

/** Node type for mutable types. */
typedef struct ast_type_mut_s ast_type_mut_t;

/** Node type for compile-time types. */
typedef struct ast_type_const_s ast_type_const_t;

/** Node type for pointer types. */
typedef struct ast_type_ptr_s ast_type_ptr_t;

/** Node type for array types. */
typedef struct ast_type_array_s ast_type_array_t;

/** Node type for reference types. */
typedef struct ast_type_ref_s ast_type_ref_t;

/** Node type for optional types. */
typedef struct ast_type_opt_s ast_type_opt_t;

/** Node type for function types. */
typedef struct ast_type_fun_s ast_type_fun_t;

/** Node type for generator types. */
typedef struct ast_type_gen_s ast_type_gen_t;

/** Node type for member types. */
typedef struct ast_type_member_s ast_type_member_t;

/** Base type for expression nodes. */
typedef struct ast_expr_s ast_expr_t;

/** Base type for literal expression nodes. */
typedef struct ast_expr_lit_s ast_expr_lit_t;

/** Base type for operator expression nodes. */
typedef struct ast_expr_op_s ast_expr_op_t;

/** Node type for unary expressions. */
typedef struct ast_expr_op_un_s ast_expr_op_un_t;

/** Node type for binary expressions. */
typedef struct ast_expr_op_bin_s ast_expr_op_bin_t;

/** Node type for call expressions. */
typedef struct ast_expr_op_call_s ast_expr_op_call_t;

/** Base type for statement nodes. */
typedef struct ast_stmt_s ast_stmt_t;

/** Node type for if-statements. */
typedef struct ast_stmt_if_s ast_stmt_if_t;

/** Node type for for-loops. */
typedef struct ast_stmt_for_s ast_stmt_for_t;

/** Node type for while-loops. */
typedef struct ast_stmt_while_s ast_stmt_while_t;

/** Node type for break statements. */
typedef struct ast_stmt_break_s ast_stmt_break_t;

/** Node type for continue statements. */
typedef struct ast_stmt_continue_s ast_stmt_continue_t;

/** Node type for return-statements. */
typedef struct ast_stmt_return_s ast_stmt_return_t;

/** Node type for yield-statements. */
typedef struct ast_stmt_yield_s ast_stmt_yield_t;

/** Node type for expression statements. */
typedef struct ast_stmt_expr_s ast_stmt_expr_t;

/** Node type for block statements. */
typedef struct ast_stmt_block_s ast_stmt_block_t;

/** Base type for declaration nodes. */
typedef struct ast_decl_s ast_decl_t;

/** Node type for variable declarations. */
typedef struct ast_decl_var_s ast_decl_var_t;

/** Node type for for-loop variable declarations. */
typedef struct ast_decl_loop_var_s ast_decl_loop_var_t;

/** Node type for function declarations. */
typedef struct ast_decl_fun_s ast_decl_fun_t;

/** Node type for generator declarations. */
typedef struct ast_decl_gen_s ast_decl_gen_t;

/** Node type for struct declarations. */
typedef struct ast_decl_struct_s ast_decl_struct_t;

/** Node type for union declarations. */
typedef struct ast_decl_union_s ast_decl_union_t;

/** Node type for enum declarations. */
typedef struct ast_decl_enum_s ast_decl_enum_t;

/** Node type for module declarations. */
typedef struct ast_decl_mod_s ast_decl_mod_t;

/** Node type for generic declarations. */
typedef struct ast_decl_generic_s ast_decl_generic_t;

/** Node type for function or generator parameters. */
typedef struct ast_param_s ast_param_t;

/** Node type for default function or generator parameters. */
typedef struct ast_param_default_s ast_param_default_t;

/** Node type for variadic parameters. */
typedef struct ast_param_variadic_s ast_param_variadic_t;

/** Node type for generic parameters. */
typedef struct ast_param_generic_s ast_param_generic_t;

/** Node type for enumerators. */
typedef struct ast_enumerator_s ast_enumerator_t;

/** Node type for a program. */
typedef struct ast_prog_s ast_prog_t;

/** Indicates the category of a type descriptor. */
typedef enum typedesc_flag_e
{
  TYPEDESC_FLAG_MODIFIER   = (1 << 0) << 8, // Modifier (pointer, reference etc.)
  TYPEDESC_FLAG_BUILTIN    = (1 << 1) << 8, // Built-in (i32, unit etc.)
  TYPEDESC_FLAG_INTEGER    = (1 << 2) << 8, // Integer (i32, u32 etc.)
  TYPEDESC_FLAG_FLOAT      = (1 << 3) << 8, // Float (f32, f64)
  TYPEDESC_FLAG_ARITHMETIC = (1 << 4) << 8, // Arithmetic (i32, f32 etc.)
  TYPEDESC_FLAG_SIGNED     = (1 << 5) << 8, // Signed (i8, i16 etc.)
  TYPEDESC_FLAG_UNSIGNED   = (1 << 6) << 8, // Unsigned (u8, u16 etc.)
  TYPEDESC_FLAG_OWNER      = (1 << 7) << 8, // Owner (struct, union etc.)
} typedesc_flag_t;

/** Indicates the kind of a type descriptor. */
typedef enum typedesc_kind_e
{
  TYPEDESC_FUN, // Function type
  TYPEDESC_GEN, // Generator type
  TYPEDESC_TYPE, // Type of type
  TYPEDESC_MUT = TYPEDESC_FLAG_MODIFIER, // Mutable type
  TYPEDESC_CONST, // Compile-time type
  TYPEDESC_PTR, // Pointer type
  TYPEDESC_ARRAY, // Array type
  TYPEDESC_REF, // Reference type
  TYPEDESC_OPT, // Optional type
  TYPEDESC_I8 = TYPEDESC_FLAG_INTEGER | TYPEDESC_FLAG_ARITHMETIC | TYPEDESC_FLAG_SIGNED | TYPEDESC_FLAG_BUILTIN, // Built-in type i8
  TYPEDESC_I16, // Built-in type i16
  TYPEDESC_I32, // Built-in type i32
  TYPEDESC_I64, // Built-in type i64
  TYPEDESC_ISIZE, // Built-in type isize
  TYPEDESC_U8 = TYPEDESC_FLAG_INTEGER | TYPEDESC_FLAG_ARITHMETIC | TYPEDESC_FLAG_UNSIGNED | TYPEDESC_FLAG_BUILTIN, // Built-in type u8
  TYPEDESC_U16, // Built-in type u16
  TYPEDESC_U32, // Built-in type u32
  TYPEDESC_U64, // Built-in type u64
  TYPEDESC_USIZE, // Built-in type usize
  TYPEDESC_F32 = TYPEDESC_FLAG_FLOAT | TYPEDESC_FLAG_ARITHMETIC | TYPEDESC_FLAG_SIGNED | TYPEDESC_FLAG_BUILTIN, // Built-in type f32
  TYPEDESC_F64, // Built-in type f64
  TYPEDESC_BOOL = TYPEDESC_FLAG_BUILTIN, // Built-in type bool
  TYPEDESC_UNIT, // Built-in type unit
  TYPEDESC_NULL, // Null type
  TYPEDESC_STRUCT = TYPEDESC_FLAG_OWNER, // Struct type
  TYPEDESC_UNION, // Union type
  TYPEDESC_ENUM, // Enum type
  TYPEDESC_MOD, // Module type
} typedesc_kind_t;

/** Base type for all type descriptors. */
typedef struct typedesc_s typedesc_t;

/** Type for mutable type descriptors. */
typedef struct typedesc_mut_s typedesc_mut_t;

/** Type for compile-time type descriptors. */
typedef struct typedesc_const_s typedesc_const_t;

/** Type for pointer type descriptors. */
typedef struct typedesc_ptr_s typedesc_ptr_t;

/** Type for array type descriptors. */
typedef struct typedesc_array_s typedesc_array_t;

/** Type for reference type descriptors. */
typedef struct typedesc_ref_s typedesc_ref_t;

/** Type for optional type descriptors. */
typedef struct typedesc_opt_s typedesc_opt_t;

/** Type for function type descriptors. */
typedef struct typedesc_fun_s typedesc_fun_t;

/** Type for generator type descriptors. */
typedef struct typedesc_gen_s typedesc_gen_t;

/** Base type for all declared type descriptors. */
typedef struct typedesc_decl_s typedesc_decl_t;

/** Type for struct type descriptors. */
typedef struct typedesc_struct_s typedesc_struct_t;

/** Type for union type descriptors. */
typedef struct typedesc_union_s typedesc_union_t;

/** Type for enum type descriptors. */
typedef struct typedesc_enum_s typedesc_enum_t;

/** Type for module type descriptors. */
typedef struct typedesc_mod_s typedesc_mod_t;

/** Represents a syntax analyzer. */
typedef struct parser_s parser_t;

/** Function pointer type to be used when parsing lists. */
typedef ast_node_t*(*parse_func_t)(parser_t*);

/**
 * \brief Represents a symbol.
 * 
 * \details A symbol is an AST node with an identifier.
 * (e.g. module, struct, variable)
 */
typedef struct symbol_s symbol_t;

/**
 * \brief Represents a symbol table.
 * 
 * \details A symbol table is a hierarchical collection of symbols
 */
typedef struct symtable_s symtable_t;

/** Indicates the kind of an operator. */
typedef enum op_kind_e
{
  OP_UNKNOWN, // Unknown operator

  OP_IS, // "is" type check
  OP_AS, // "as" type cast
  OP_SIZEOF, // "sizeof" size of expression type
  OP_ALIGNOF, // "alignof" align of expression type
  OP_TYPEOF, // "typeof" type of expression
  OP_IN, // "in" in range
  
  OP_ARIT_INC_PRE, // "++" arithmetic pre increment
  OP_ARIT_INC_POST, // "++" arithmetic post increment
  OP_ARIT_DEC_PRE, // "--" arithmetic pre decrement
  OP_ARIT_DEC_POST, // "--" arithmetic post decrement
  OP_ARIT_POS, // "+" arithmetic positive
  OP_ARIT_NEG, // "-" arithmetic negative
  OP_ARIT_ADD, // "+" arithmetic add
  OP_ARIT_SUB, // "-" arithmetic subtract
  OP_ARIT_MUL, // "*" arithmetic multiply
  OP_ARIT_DIV, // "/" arithmetic divide
  OP_ARIT_MOD, // "%" arithmetic modulo
  
  OP_BIT_AND, // "&" bitwise and
  OP_BIT_OR, // "|" bitwise or
  OP_BIT_XOR, // "^" bitwise xor
  OP_BIT_NOT, // "~" bitwise not
  OP_BIT_LSH, // "<<" bitwise left shift
  OP_BIT_RSH, // ">>" bitwise right shift

  OP_LOGIC_AND, // "&&" logical and
  OP_LOGIC_OR, // "||" logical or
  OP_LOGIC_NOT, // "!" logical not

  OP_COMP_EQ, // "==" comparison equal
  OP_COMP_NE, // "!=" comparison not equal
  OP_COMP_LT, // "<" comparison less than
  OP_COMP_LE, // "<=" comparison less equal
  OP_COMP_GT, // ">" comparison greater than
  OP_COMP_GE, // ">=" comparison greater equal

  OP_ASSIGN, // "=" assign
  OP_ARIT_ADD_ASSIGN, // "+=" arithmetic add assign
  OP_ARIT_SUB_ASSIGN, // "-=" arithmetic subtract assign
  OP_ARIT_MUL_ASSIGN, // "*=" arithmetic multiply assign
  OP_ARIT_DIV_ASSIGN, // "/=" arithmetic divide assign
  OP_ARIT_MOD_ASSIGN, // "%=" arithmetic modulo assign
  OP_BIT_AND_ASSIGN, // "&=" bitwise and assign
  OP_BIT_OR_ASSIGN, // "|=" bitwise or assign
  OP_BIT_XOR_ASSIGN, // "^=" bitwise xor assign
  OP_BIT_LSH_ASSIGN, // "<<=" bitwise left shift assign
  OP_BIT_RSH_ASSIGN, // ">>=" bitwise right shift assign

  OP_SUBS, // "[]" subscript
  OP_IND, // "*" indirection
  OP_ADDR, // "&" address of
  OP_MEMBER, // "." member access
  OP_IND_MEMBER, // "*." indirect member access
  OP_NULL_SAFE_MEMBER, // "?." null-safe member access
  OP_RANGE, // ".." generate range
  OP_SEMICOLON, // ";" semicolon
  OP_CALL, // "()" function call
} op_kind_t;

/** Represents an analyzer. */
typedef struct analyzer_s analyzer_t;

/** Represents a register in the virtual machine. */
typedef enum register_e {
  REG_A,    // 64-bit general purpose register A
  REG_AHD,  // 32-bit general purpose register A high double word
  REG_ALD,  // 32-bit general purpose register A low double word
  REG_AHW,  // 16-bit general purpose register A high word
  REG_ALW,  // 16-bit general purpose register A low word
  REG_AHB,  // 8-bit general purpose register A high byte
  REG_ALB,  // 8-bit general purpose register A low byte

  REG_B,    // 64-bit general purpose register B
  REG_BHD,  // 32-bit general purpose register B high double word
  REG_BLD,  // 32-bit general purpose register B low double word
  REG_BHW,  // 16-bit general purpose register B high word
  REG_BLW,  // 16-bit general purpose register B low word
  REG_BHB,  // 8-bit general purpose register B high byte
  REG_BLB,  // 8-bit general purpose register B low byte

  REG_C,    // 64-bit general purpose register C
  REG_CHD,  // 32-bit general purpose register C high double word
  REG_CLD,  // 32-bit general purpose register C low double word
  REG_CHW,  // 16-bit general purpose register C high word
  REG_CLW,  // 16-bit general purpose register C low word
  REG_CHB,  // 8-bit general purpose register C high byte
  REG_CLB,  // 8-bit general purpose register C low byte

  REG_D,    // 64-bit general purpose register D
  REG_DHD,  // 32-bit general purpose register D high double word
  REG_DLD,  // 32-bit general purpose register D low double word
  REG_DHW,  // 16-bit general purpose register D high word
  REG_DLW,  // 16-bit general purpose register D low word
  REG_DHB,  // 8-bit general purpose register D high byte
  REG_DLB,  // 8-bit general purpose register D low byte

  REG_E,    // 64-bit general purpose register E
  REG_EHD,  // 32-bit general purpose register E high double word
  REG_ELD,  // 32-bit general purpose register E low double word
  REG_EHW,  // 16-bit general purpose register E high word
  REG_ELW,  // 16-bit general purpose register E low word
  REG_EHB,  // 8-bit general purpose register E high byte
  REG_ELB,  // 8-bit general purpose register E low byte

  REG_F,    // 64-bit general purpose register F
  REG_FHD,  // 32-bit general purpose register F high double word
  REG_FLD,  // 32-bit general purpose register F low double word
  REG_FHW,  // 16-bit general purpose register F high word
  REG_FLW,  // 16-bit general purpose register F low word
  REG_FHB,  // 8-bit general purpose register F high byte
  REG_FLB,  // 8-bit general purpose register F low byte

  REG_SP,   // 64-bit stack pointer register
  REG_BP,   // 64-bit base pointer register
  REG_IP,   // 64-bit instruction pointer register
} register_t;

/** Represents an operation code for the virtual machine. */
typedef enum opcode_e {
  OPCODE_NOP,   // NOP

  OPCODE_MOV,   // MOV <reg|mem>, <reg|mem|imm>
  OPCODE_PSH,   // PSH <reg|mem|imm>
  OPCODE_POP,   // POP <reg|mem>
  OPCODE_LEA,   // LEA <reg>, <mem>

  OPCODE_IADD,  // IADD <reg|mem>, <reg|mem|imm>
  OPCODE_ISUB,  // ISUB <reg|mem>, <reg|mem|imm>
  OPCODE_IMUL,  // IMUL <reg|mem>, <reg|mem|imm>
  OPCODE_IDIV,  // IDIV <reg|mem>, <reg|mem|imm>
  OPCODE_IMOD,  // IMOD <reg|mem>, <reg|mem|imm>
  OPCODE_INEG,  // INEG <reg|mem>

  OPCODE_IINC,  // IINC <reg|mem>
  OPCODE_IDEC,  // IDEC <reg|mem>

  OPCODE_FADD,  // FADD <reg|mem>, <reg|mem|imm>
  OPCODE_FSUB,  // FSUB <reg|mem>, <reg|mem|imm>
  OPCODE_FMUL,  // FMUL <reg|mem>, <reg|mem|imm>
  OPCODE_FDIV,  // FDIV <reg|mem>, <reg|mem|imm>
  OPCODE_FMOD,  // FMOD <reg|mem>, <reg|mem|imm>
  OPCODE_FNEG,  // FNEG <reg|mem>

  OPCODE_AND,   // AND <reg|mem>, <reg|mem|imm>
  OPCODE_OR,    // OR <reg|mem>, <reg|mem|imm>
  OPCODE_XOR,   // XOR <reg|mem>, <reg|mem|imm>
  OPCODE_NOT,   // NOT <reg|mem>
  OPCODE_SHL,   // SHL <reg|mem>, <reg|mem|imm>
  OPCODE_SHR,   // SHL <reg|mem>, <reg|mem|imm>
  OPCODE_ROL,   // ROL <reg|mem>, <reg|mem|imm>
  OPCODE_ROR,   // ROR <reg|mem>, <reg|mem|imm>

  OPCODE_JMP,   // JMP <label>

  OPCODE_JE,    // JE <label>
  OPCODE_JNE,   // JNE <label>
  OPCODE_JL,    // JL <label>
  OPCODE_JLE,   // JLE <label>
  OPCODE_JG,    // JG <label>
  OPCODE_JGE,   // JGE <label>
  OPCODE_JZ,    // JZ <label>
  OPCODE_JNZ,   // JNZ <label>

  OPCODE_CMP,   // CMP <reg|mem>, <reg|mem|imm>

  OPCODE_CLF,   // CLF

  OPCODE_CALL,  // CALL <label>
  OPCODE_RET,   // RET

  OPCODE_HLT,   // HLT
} opcode_t;

/** Operation code parameter types. */
typedef enum opcode_param_e
{
  OPCODE_PARAM_REG_REG  /* = 0b000 */,
  OPCODE_PARAM_MEM_REG  /* = 0b001 */,
  OPCODE_PARAM_REG_MEM  /* = 0b010 */,
  OPCODE_PARAM_MEM_MEM  /* = 0b011 */,
  OPCODE_PARAM_REG_IMM  /* = 0b100 */,
  OPCODE_PARAM_MEM_IMM  /* = 0b101 */,
  OPCODE_PARAM_LABEL    /* = 0b110 */,
  OPCODE_PARAM_RESERVED /* = 0b111 */,
} opcode_param_t;

/** Operation code parameter widths. */
typedef enum opcode_width_e
{
  OPCODE_WIDTH_8BIT  /* = 0b00 */,
  OPCODE_WIDTH_16BIT /* = 0b01 */,
  OPCODE_WIDTH_32BIT /* = 0b10 */,
  OPCODE_WIDTH_64BIT /* = 0b11 */,
} opcode_width_t;

/** Represents a virtual machine instance. */
typedef struct vm_s vm_t;

/** Represents a compiler instance. */
typedef struct compiler_s compiler_t;

#endif