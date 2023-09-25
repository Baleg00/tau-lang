/**
 * \file lexer.h
 * 
 * \brief Lexical analyzer interface.
 * 
 * \details A lexical analyzer, also known as a lexer, breaks down the source
 * code into a sequence of tokens. It scans the input text character by
 * character and groups them into meaningful units, such as keywords,
 * identifiers, operators, and literals. The lexer helps in the process of
 * parsing by providing a structured representation of the input code, which
 * can be easily processed by subsequent stages of the compilation process.
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_LEXER_H
#define TAU_LEXER_H

#include <stdbool.h>
#include <stdio.h>

#include "location.h"
#include "token.h"

/**
 * \brief Represents a lexical analyzer.
 */
typedef struct lexer_s lexer_t;

/**
 * \brief Initializes a lexer with the provided parameters.
 *
 * \param[in] path The path of the source file.
 * \param[in] src Pointer to the source code.
 * \returns Pointer to the newly initialized lexer.
 */
lexer_t* lexer_init(const char* path, char* src);

/**
 * \brief Frees resources associated with a lexer.
 *
 * \param[in] lex Pointer to the lexer to free.
 */
void lexer_free(lexer_t* lex);

/**
 * \brief Creates a copy of the current location in the lexer.
 *
 * \param[in] lex Pointer to the lexer.
 * \returns Pointer to a new location structure with the same values as the
 * current location in the lexer.
 */
location_t* lexer_location_copy(lexer_t* lex);

/**
 * \brief Initializes a new token with the given kind in the lexer.
 *
 * \param[in] lex Pointer to the lexer.
 * \param[in] kind The kind of token to initialize.
 * \returns Pointer to the newly initialized token.
 */
token_t* lexer_token_init(lexer_t* lex, token_kind_t kind);

/**
 * \brief Checks if the current character in the lexer is a whitespace
 * character.
 *
 * \param[in] lex Pointer to the lexer.
 * \returns `true` if the current character is a whitespace, `false` otherwise.
 */
bool lexer_is_space(lexer_t* lex);

/**
 * \brief Checks if the current character in the lexer is the beginning of a
 * word.
 *
 * \param[in] lex Pointer to the lexer.
 * \returns `true` if the current character is the beginning of a word, `false`
 * otherwise.
 */
bool lexer_is_word_begin(lexer_t* lex);

/**
 * \brief Checks if the current character in the lexer is alphanumeric.
 *
 * \param[in] lex Pointer to the lexer.
 * \returns `true` if the current character is alphanumeric, `false`
 * otherwise.
 */
bool lexer_is_word(lexer_t* lex);

/**
 * \brief Checks if the current character in the lexer is a decimal digit.
 *
 * \param[in] lex Pointer to the lexer.
 * \returns `true` if the current character is a decimal digit, `false`
 * otherwise.
 */
bool lexer_is_decimal(lexer_t* lex);

/**
 * \brief Checks if the current character in the lexer is a hexadecimal digit.
 *
 * \param[in] lex Pointer to the lexer.
 * \returns `true` if the current character is a hexadecimal digit, `false`
 * otherwise.
 */
bool lexer_is_hexadecimal(lexer_t* lex);

/**
 * \brief Checks if the current character in the lexer is an octal digit.
 *
 * \param[in] lex Pointer to the lexer.
 * \returns `true` if the current character is an octal digit, `false`
 * otherwise.
 */
bool lexer_is_octal(lexer_t* lex);

/**
 * \brief Checks if the current character in the lexer is a binary digit.
 *
 * \param[in] lex Pointer to the lexer.
 * \returns `true` if the current character is a binary digit, `false`
 * otherwise.
 */
bool lexer_is_binary(lexer_t* lex);

/**
 * \brief Checks if the current character in the lexer is a punctuation
 * character.
 *
 * \param[in] lex Pointer to the lexer.
 * \returns `true` if the current character is a punctuation character, `false`
 * otherwise.
 */
bool lexer_is_punctuation(lexer_t* lex);

/**
 * \brief Returns the current character in the lexer.
 *
 * \param[in] lex Pointer to the lexer.
 * \returns The current character.
 */
char lexer_current(lexer_t* lex);

/**
 * \brief Returns the current character in the lexer and advances to the next
 * character.
 *
 * \param[in] lex Pointer to the lexer.
 * \returns The current character.
 */
char lexer_next(lexer_t* lex);

/**
 * \brief Peeks at the next character in the lexer without advancing.
 *
 * \param[in] lex Pointer to the lexer.
 * \returns The next character.
 */
char lexer_peek(lexer_t* lex);

/**
 * \brief Consumes the current character if it matches the given character.
 *
 * \param[in] lex Pointer to the lexer.
 * \param[in] ch The character to consume.
 * \returns `true` if the current character matches the given character and is
 * consumed, `false` otherwise.
 */
bool lexer_consume(lexer_t* lex, char ch);

/**
 * \brief Skips characters in the lexer as long as the provided predicate
 * function returns `true`.
 *
 * \param[in] lex Pointer to the lexer.
 * \param[in] pred Pointer to the predicate function.
 * \returns The number of characters skipped.
 */
size_t lexer_skip(lexer_t* lex, bool(*pred)(lexer_t*));

/**
 * \brief Skips a specific number of characters in the lexer.
 *
 * \param[in] lex Pointer to the lexer.
 * \param[in] n The number of characters to skip.
 */
void lexer_skip_n(lexer_t* lex, size_t n);

/**
 * \brief Skips an integer suffix if present.
 *
 * \param[in] lex Pointer to the lexer.
 * \returns The number of characters skipped.
 */
size_t lexer_skip_integer_suffix(lexer_t* lex);

/**
 * \brief Reads a word token from the lexer.
 *
 * \param[in] lex Pointer to the lexer.
 * \returns Pointer to the newly read word token.
 */
token_t* lexer_read_word(lexer_t* lex);

/**
 * \brief Reads an octal integer literal token from the lexer.
 *
 * \param[in] lex Pointer to the lexer.
 * \returns Pointer to the newly read octal integer token.
 */
token_t* lexer_read_octal_integer(lexer_t* lex);

/**
 * \brief Reads a binary integer literal token from the lexer.
 *
 * \param[in] lex Pointer to the lexer.
 * \returns Pointer to the newly read binary integer token.
 */
token_t* lexer_read_binary_integer(lexer_t* lex);

/**
 * \brief Reads a decimal number literal token from the lexer.
 *
 * \param[in] lex Pointer to the lexer.
 * \returns Pointer to the newly read decimal number token.
 */
token_t* lexer_read_decimal_number(lexer_t* lex);

/**
 * \brief Reads a hexadecimal integer literal token from the lexer.
 *
 * \param[in] lex Pointer to the lexer.
 * \returns Pointer to the newly read hexadecimal integer token.
 */
token_t* lexer_read_hexadecimal_integer(lexer_t* lex);

/**
 * \brief Reads a number literal token from the lexer.
 *
 * \param[in] lex Pointer to the lexer.
 * \returns Pointer to the newly read number token.
 */
token_t* lexer_read_number(lexer_t* lex);

/**
 * \brief Reads a string literal token from the lexer.
 *
 * \param[in] lex Pointer to the lexer.
 * \returns Pointer to the newly read string token.
 */
token_t* lexer_read_string(lexer_t* lex);

/**
 * \brief Reads a character literal token from the lexer.
 *
 * \param[in] lex Pointer to the lexer.
 * \returns Pointer to the newly read character token.
 */
token_t* lexer_read_character(lexer_t* lex);

/**
 * \brief Reads a punctuation token from the lexer.
 *
 * \param[in] lex Pointer to the lexer.
 * \returns Pointer to the newly read punctuation token.
 */
token_t* lexer_read_punctuation(lexer_t* lex);

/**
 * \brief Reads the next token from the lexer.
 *
 * \param[in] lex Pointer to the lexer.
 * \returns Pointer to the newly read token.
 */
token_t* lexer_read_next(lexer_t* lex);

/**
 * \brief Lexes the source code and generates a list of tokens.
 *
 * \param[in] lex Pointer to the lexer.
 * \param[out] toks Pointer to the list where the tokens will be stored.
 */
void lexer_lex(lexer_t* lex, list_t* toks);

#endif