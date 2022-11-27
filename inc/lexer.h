#ifndef TAU_LEXER_H
#define TAU_LEXER_H

#include <stdio.h>
#include <stdbool.h>

#include "forward_decls.h"

struct lexer_s
{
  location_t* loc;
  list_t* toks;
};

lexer_t* lexer_init(char* path, char* src);
void lexer_free(lexer_t* lex);

token_t* lexer_token_init(lexer_t* lex, token_kind_t kind);

void lexer_token_push(lexer_t* lex, token_t* tok);

int lexer_is_space(lexer_t* lex);
int lexer_is_word_begin(lexer_t* lex);
int lexer_is_word(lexer_t* lex);
int lexer_is_decimal(lexer_t* lex);
int lexer_is_hexadecimal(lexer_t* lex);
int lexer_is_octal(lexer_t* lex);
int lexer_is_binary(lexer_t* lex);
int lexer_is_punctuation(lexer_t* lex);

char lexer_current(lexer_t* lex);
char lexer_next(lexer_t* lex);
char lexer_peek(lexer_t* lex);
bool lexer_consume(lexer_t* lex, char ch);

size_t lexer_skip(lexer_t* lex, int(*pred)(lexer_t*));
void lexer_skip_n(lexer_t* lex, size_t n);

void lexer_read_word(lexer_t* lex);
void lexer_read_octal_integer(lexer_t* lex);
void lexer_read_binary_integer(lexer_t* lex);
void lexer_read_decimal_number(lexer_t* lex);
void lexer_read_hexadecimal_integer(lexer_t* lex);
void lexer_read_number(lexer_t* lex);
void lexer_read_string(lexer_t* lex);
void lexer_read_character(lexer_t* lex);
void lexer_read_punctuation(lexer_t* lex);

void lexer_read_next(lexer_t* lex);

void lexer_lex(lexer_t* lex);

#endif