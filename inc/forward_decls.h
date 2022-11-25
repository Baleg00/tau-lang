#ifndef TAU_FORWARD_DECLS_H
#define TAU_FORWARD_DECLS_H

typedef struct list_elem_s list_elem_t;
typedef struct list_s list_t;
typedef list_t stack_t;
typedef list_t queue_t;

typedef enum cli_type_e cli_type_t;
typedef struct cli_opt_s cli_opt_t;
typedef struct cli_s cli_t;
typedef void(*cli_callback_t)(cli_t*, queue_t*, cli_opt_t*, void*);

typedef struct location_s location_t;

typedef enum token_kind_e token_kind_t;
typedef struct token_s token_t;

typedef enum op_kind_e op_kind_t;

typedef enum type_kind_e type_kind_t;
typedef struct type_s type_t;

typedef enum shyd_kind_e shyd_kind_t;
typedef struct shyd_elem_s shyd_elem_t;
typedef struct shyd_s shyd_t;

typedef enum ast_kind_e ast_kind_t;
typedef struct ast_node_s ast_node_t;

typedef struct symbol_s symbol_t;

typedef struct symtable_s symtable_t;

typedef struct lexer_s lexer_t;

typedef struct parser_s parser_t;

typedef struct analyzer_s analyzer_t;

#endif