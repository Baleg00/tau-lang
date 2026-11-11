/**
 * \file
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

#include "stages/lexer/token/token.h"
#include "utils/common.h"
#include "utils/error.h"

TAU_EXTERN_C_BEGIN

/**
 * \brief Represents a lexical analyzer.
 */
typedef struct tau_lexer_t tau_lexer_t;

/**
 * \brief Initializes a new lexer.
 *
 * \returns Pointer to the newly initialized lexer.
 */
tau_lexer_t* tau_lexer_init(void);

/**
 * \brief Frees resources associated with a lexer.
 *
 * \param[in] lex Pointer to the lexer to free.
 */
void tau_lexer_free(tau_lexer_t* lex);

/**
 * \brief Initializes a new token with the given kind in the lexer.
 *
 * \param[in] lex Pointer to the lexer.
 * \param[in] kind The kind of token to initialize.
 * \returns Pointer to the newly initialized token.
 */
tau_token_t* tau_lexer_token_init(tau_lexer_t* lex, tau_token_kind_t kind);

/**
 * \brief Returns the location of the current lexer position.
 *
 * \param[in] lex Pointer to the lexer.
 * \returns Location of the current lexer position.
 */
tau_location_t tau_lexer_location(tau_lexer_t* lex);

/**
 * \brief Checks if the current character in the lexer is a whitespace
 * character.
 *
 * \param[in] lex Pointer to the lexer.
 * \returns `true` if the current character is a whitespace, `false` otherwise.
 */
bool tau_lexer_is_space(tau_lexer_t* lex);

/**
 * \brief Checks if the current character in the lexer is the beginning of a
 * word.
 *
 * \param[in] lex Pointer to the lexer.
 * \returns `true` if the current character is the beginning of a word, `false`
 * otherwise.
 */
bool tau_lexer_is_word_begin(tau_lexer_t* lex);

/**
 * \brief Checks if the current character in the lexer is alphanumeric.
 *
 * \param[in] lex Pointer to the lexer.
 * \returns `true` if the current character is alphanumeric, `false`
 * otherwise.
 */
bool tau_lexer_is_word(tau_lexer_t* lex);

/**
 * \brief Checks if the current character in the lexer is a decimal digit.
 *
 * \param[in] lex Pointer to the lexer.
 * \returns `true` if the current character is a decimal digit, `false`
 * otherwise.
 */
bool tau_lexer_is_decimal(tau_lexer_t* lex);

/**
 * \brief Checks if the current character in the lexer is a hexadecimal digit.
 *
 * \param[in] lex Pointer to the lexer.
 * \returns `true` if the current character is a hexadecimal digit, `false`
 * otherwise.
 */
bool tau_lexer_is_hexadecimal(tau_lexer_t* lex);

/**
 * \brief Checks if the current character in the lexer is an octal digit.
 *
 * \param[in] lex Pointer to the lexer.
 * \returns `true` if the current character is an octal digit, `false`
 * otherwise.
 */
bool tau_lexer_is_octal(tau_lexer_t* lex);

/**
 * \brief Checks if the current character in the lexer is a binary digit.
 *
 * \param[in] lex Pointer to the lexer.
 * \returns `true` if the current character is a binary digit, `false`
 * otherwise.
 */
bool tau_lexer_is_binary(tau_lexer_t* lex);

/**
 * \brief Checks if the current character in the lexer is a punctuation
 * character.
 *
 * \param[in] lex Pointer to the lexer.
 * \returns `true` if the current character is a punctuation character, `false`
 * otherwise.
 */
bool tau_lexer_is_punctuation(tau_lexer_t* lex);

/**
 * \brief Returns the current character in the lexer.
 *
 * \param[in] lex Pointer to the lexer.
 * \returns The current character.
 */
char tau_lexer_current(tau_lexer_t* lex);

/**
 * \brief Returns the current character in the lexer and advances to the next
 * character.
 *
 * \param[in] lex Pointer to the lexer.
 * \returns The current character.
 */
char tau_lexer_next(tau_lexer_t* lex);

/**
 * \brief Peeks at the next character in the lexer without advancing.
 *
 * \param[in] lex Pointer to the lexer.
 * \returns The next character.
 */
char tau_lexer_peek(tau_lexer_t* lex);

/**
 * \brief Consumes the current character if it matches the given character.
 *
 * \param[in] lex Pointer to the lexer.
 * \param[in] ch The character to consume.
 * \returns `true` if the current character matches the given character and is
 * consumed, `false` otherwise.
 */
bool tau_lexer_consume(tau_lexer_t* lex, char ch);

/**
 * \brief Skips characters in the lexer as long as the provided predicate
 * function returns `true`.
 *
 * \param[in] lex Pointer to the lexer.
 * \param[in] pred Pointer to the predicate function.
 * \returns The number of characters skipped.
 */
size_t tau_lexer_skip(tau_lexer_t* lex, bool(*pred)(tau_lexer_t*));

/**
 * \brief Skips a specific number of characters in the lexer.
 *
 * \param[in] lex Pointer to the lexer.
 * \param[in] n The number of characters to skip.
 */
void tau_lexer_skip_n(tau_lexer_t* lex, size_t n);

/**
 * \brief Skips an integer suffix if present.
 *
 * \param[in] lex Pointer to the lexer.
 * \returns The number of characters skipped.
 */
size_t tau_lexer_skip_integer_suffix(tau_lexer_t* lex);

/**
 * \brief Skips a line comment.
 *
 * \param[in] lex Pointer to the lexer.
 */
void tau_lexer_skip_comment_line(tau_lexer_t* lex);

/**
 * \brief Skips a block comment.
 *
 * \param[in] lex Pointer to the lexer.
 */
void tau_lexer_skip_comment_block(tau_lexer_t* lex);

/**
 * \brief Reads a word token from the lexer.
 *
 * \param[in] lex Pointer to the lexer.
 * \returns Pointer to the newly read word token.
 */
tau_token_t* tau_lexer_read_word(tau_lexer_t* lex);

/**
 * \brief Reads an octal integer literal token from the lexer.
 *
 * \param[in] lex Pointer to the lexer.
 * \returns Pointer to the newly read octal integer token.
 */
tau_token_t* tau_lexer_read_octal_integer(tau_lexer_t* lex);

/**
 * \brief Reads a binary integer literal token from the lexer.
 *
 * \param[in] lex Pointer to the lexer.
 * \returns Pointer to the newly read binary integer token.
 */
tau_token_t* tau_lexer_read_binary_integer(tau_lexer_t* lex);

/**
 * \brief Reads a decimal number literal token from the lexer.
 *
 * \param[in] lex Pointer to the lexer.
 * \returns Pointer to the newly read decimal number token.
 */
tau_token_t* tau_lexer_read_decimal_number(tau_lexer_t* lex);

/**
 * \brief Reads a hexadecimal integer literal token from the lexer.
 *
 * \param[in] lex Pointer to the lexer.
 * \returns Pointer to the newly read hexadecimal integer token.
 */
tau_token_t* tau_lexer_read_hexadecimal_integer(tau_lexer_t* lex);

/**
 * \brief Reads a number literal token from the lexer.
 *
 * \param[in] lex Pointer to the lexer.
 * \returns Pointer to the newly read number token.
 */
tau_token_t* tau_lexer_read_number(tau_lexer_t* lex);

/**
 * \brief Reads a string literal token from the lexer.
 *
 * \param[in] lex Pointer to the lexer.
 * \returns Pointer to the newly read string token.
 */
tau_token_t* tau_lexer_read_string(tau_lexer_t* lex);

/**
 * \brief Reads a character literal token from the lexer.
 *
 * \param[in] lex Pointer to the lexer.
 * \returns Pointer to the newly read character token.
 */
tau_token_t* tau_lexer_read_character(tau_lexer_t* lex);

/**
 * \brief Reads a punctuation token from the lexer.
 *
 * \param[in] lex Pointer to the lexer.
 * \returns Pointer to the newly read punctuation token.
 */
tau_token_t* tau_lexer_read_punctuation(tau_lexer_t* lex);

/**
 * \brief Reads the next token from the lexer.
 *
 * \param[in] lex Pointer to the lexer.
 * \returns Pointer to the newly read token.
 */
tau_token_t* tau_lexer_read_next(tau_lexer_t* lex);

/**
 * \brief Lexes the source code and generates a list of tokens.
 *
 * \param[in] lex Pointer to the lexer.
 * \param[in] path The path of the source file.
 * \param[in] src Pointer to the source code.
 * \param[out] tokens Pointer to the vector to add tokens to.
 * \param[out] errors Pointer to the error bag to add errors to.
 */
void tau_lexer_lex(tau_lexer_t* lex, const char* path, const char* src, tau_vector_t* tokens, tau_error_bag_t* errors);

TAU_EXTERN_C_END

#endif
