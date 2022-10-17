#ifndef TAU_LEXER_H
#define TAU_LEXER_H

#include <stdio.h>

#include "token.h"

/// Struct containing informtion for a single lexical analysis.
typedef struct lexer_s
{
  /// Path to the source file.
  const char* filepath;

  /// Source string.
  char* src;

  /// Current position in source string.
  char* cur;

  /// Current row in source string. 
  size_t row;

  /// Current column in source string.
  size_t col;

  /// List of tokens.
  token_list_t* toks;
} lexer_t;

/// Initialize lexer with a given source file.
/// @param filepath Path to source file.
/// @return Pointer to the lexer for this file.
lexer_t* lexer_init(const char* filepath);

/// Free lexer.
/// @param lex Lexer to be freed.
void lexer_free(lexer_t* lex);

/// Initialize token with a given kind and current location.
/// @param lex The lexer.
/// @param kind The token kind.
/// @return Pointer to the new token.
token_t* lexer_token_init(lexer_t* lex, token_kind_t kind);

/// Push token onto lexer token list.
/// @param lex The lexer.
/// @param tok The token.
void lexer_token_push(lexer_t* lex, token_t* tok);

/// Determine whether the current character is a whitespace.
/// @param lex The lexer.
/// @return A non-zero integer if the character is a whitespace, 0 otherwise.
int lexer_is_space(lexer_t* lex);

/// Determine whether the current character is a word begin character.
/// @param lex The lexer.
/// @return A non-zero integer if the character is a word begin character, 0 otherwise.
int lexer_is_word_begin(lexer_t* lex);

/// Determine whether the current character is a word character.
/// @param lex The lexer.
/// @return A non-zero integer if the character is a word character, 0 otherwise.
int lexer_is_word(lexer_t* lex);

/// Determine whether the current character is a decimal digit.
/// @param lex The lexer.
/// @return A non-zero integer if the character is a decimal digit, 0 otherwise.
int lexer_is_decimal(lexer_t* lex);

/// Determine whether the current character is a hexadecimal digit.
/// @param lex The lexer.
/// @return A non-zero integer if the character is a hexadecimal digit, 0 otherwise.
int lexer_is_hexadecimal(lexer_t* lex);

/// Determine whether the current character is an octal digit.
/// @param lex The lexer.
/// @return A non-zero integer if the character is an octal digit, 0 otherwise.
int lexer_is_octal(lexer_t* lex);

/// Determine whether the current character is a binary digit.
/// @param lex The lexer.
/// @return A non-zero integer if the character is a binary digit, 0 otherwise.
int lexer_is_binary(lexer_t* lex);

/// Determine whether the current character is a punctuation.
/// @param lex The lexer.
/// @return A non-zero integer if the character is a punctuation, 0 otherwise.
int lexer_is_punctuation(lexer_t* lex);

/// Get the current character from the lexer.
/// @param lex The lexer.
/// @return The current character.
char lexer_current(lexer_t* lex);

/// Get the current character from the lexer, and advance to the next.
/// @param lex The lexer.
/// @return The current character.
char lexer_next(lexer_t* lex);

/// Get the next character from the lexer.
/// @param lex The lexer.
/// @return The next character or 0 if there are no more characters.
char lexer_peek(lexer_t* lex);

/// Advance to the next character, if the current one is equal to the given character.
/// @param lex The lexer.
/// @param ch The given character.
/// @return True if the current character is equal to the given one, false otehrwise.
bool lexer_consume(lexer_t* lex, char ch);

/// Skip characters while a predicate is true or until the end of the file is reached.
/// @param lex The lexer.
/// @param pred The predicate.
/// @return The number of characters skipped.
size_t lexer_skip(lexer_t* lex, int(*pred)(lexer_t*));

/// Skip the given number of characters, or stop at the end of the file.
/// @param lex The lexer.
/// @param n The number of characters to skip.
void lexer_skip_n(lexer_t* lex, size_t n);

/// Read a word token. Can be a keyword, a literal or an identifier.
/// @param lex The lexer.
void lexer_read_word(lexer_t* lex);

/// Read an octal integer literal.
/// @param lex The lexer.
void lexer_read_octal_integer(lexer_t* lex);

/// Read a binary integer literal.
/// @param lex The lexer.
void lexer_read_binary_integer(lexer_t* lex);

/// Read a decimal number literal. Either an integer or a float.
/// @param lex The lexer.
void lexer_read_decimal_number(lexer_t* lex);

/// Read a hexadecimal number literal. Either an integer or a float.
/// @param lex The lexer.
void lexer_read_hexadecimal_number(lexer_t* lex);

/// Read a number literal.
/// @param lex The lexer.
void lexer_read_number(lexer_t* lex);

/// Read a string literal.
/// @param lex The lexer.
void lexer_read_string(lexer_t* lex);

/// Read a character literal.
/// @param lex The lexer.
void lexer_read_character(lexer_t* lex);

/// Read a punctuation.
/// @param lex The lexer.
void lexer_read_punctuation(lexer_t* lex);

/// Read the next token or a TOK_EOF if the end of the file is reached.
/// @param lex The lexer.
void lexer_read_next(lexer_t* lex);

/// Perform lexical analysis and produce a list of tokens as a result.
/// @param lex The lexer.
void lexer_lex(lexer_t* lex);

#endif