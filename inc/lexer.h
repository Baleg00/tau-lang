/**
 * \file
 * 
 * Lexical analyzer.
*/

#ifndef TAU_LEXER_H
#define TAU_LEXER_H

#include <stdio.h>
#include <stdbool.h>

#include "typedefs.h"
#include "list.h"

struct lexer_s
{
  arena_t* arena;

  location_t* loc; // Current location in source file.
};

/**
 * \brief Initializes a lexer instance.
 * 
 * \param[out] lex Lexer instance to be initialized.
 * \param[in] arena Arena allocator to be used.
 * \param[in] path Path to source file to be processed.
 * \param[in] src Pointer to source string to be processed.
 * \returns New lexer.
*/
void lexer_init(lexer_t* lex, arena_t* arena, const char* path, char* src);

/**
 * \brief Destroys a lexer.
 * 
 * \details The tokens are destroyed as well but the source string is not.
 * 
 * \param[in] lex Lexer to be destroyed.
*/
void lexer_free(lexer_t* lex);

/**
 * \brief Creates a copy of the current location.
 * 
 * \param[in] lex Lexer to be used.
 * \returns Copy of current location.
 */
location_t* lexer_location_copy(lexer_t* lex);

/**
 * \brief Initializes a new token using the current state.
 * 
 * \details The token's location will match the lexer's current location.
 * 
 * \param[in] lex Lexer to be used.
 * \param[in] kind Kind of token to be created.
 * \returns New token.
*/
token_t* lexer_token_init(lexer_t* lex, token_kind_t kind);

/**
 * \brief Checks if the current character is a whitespace.
 * 
 * \param[in] lex Lexer to be used.
 * \returns Non-zero value if the current character is a whitespace, zero
 * otherwise.
*/
bool lexer_is_space(lexer_t* lex);

/**
 * \brief Checks if the current character could be the beginning of a word.
 * 
 * \param[in] lex Lexer to be used.
 * \returns Non-zero value if the current character could be the beginning of a
 * word, zero otherwise.
*/
bool lexer_is_word_begin(lexer_t* lex);

/**
 * \brief Checks if the current character could appear in a word.
 * 
 * \param[in] lex Lexer to be used.
 * \returns Non-zero value if the current character could appear in a word,
 * zero otherwise.
*/
bool lexer_is_word(lexer_t* lex);

/**
 * \brief Checks if the current character is a decimal digit.
 * 
 * \param[in] lex Lexer to be used.
 * \returns Non-zero value if the current character is a decimal digit, zero
 * otherwise.
*/
bool lexer_is_decimal(lexer_t* lex);

/**
 * \brief Checks if the current character is a hexadecimal digit.
 * 
 * \param[in] lex Lexer to be used.
 * \returns Non-zero value if the current character is a hexadecimal digit, zero
 * otherwise.
*/
bool lexer_is_hexadecimal(lexer_t* lex);

/**
 * \brief Checks if the current character is an octal digit.
 * 
 * \param[in] lex Lexer to be used.
 * \returns Non-zero value if the current character is an octal digit, zero
 * otherwise.
*/
bool lexer_is_octal(lexer_t* lex);

/**
 * \brief Checks if the current character is a binary digit.
 * 
 * \param[in] lex Lexer to be used.
 * \returns Non-zero value if the current character is a binary digit, zero
 * otherwise.
*/
bool lexer_is_binary(lexer_t* lex);

/**
 * \brief Checks if the current character is a punctuation.
 * 
 * \param[in] lex Lexer to be used.
 * \returns Non-zero value if the current character is a punctuation, zero
 * otherwise.
*/
bool lexer_is_punctuation(lexer_t* lex);

/**
 * \brief Returns the character at the current location of the lexer.
 * 
 * \param[in] lex Lexer to be used.
 * \returns Current character or terminating-null if the end of the source is
 * reached.
*/
char lexer_current(lexer_t* lex);

/**
 * \brief Returns the character at the current location of the lexer and moves
 * location to the next one.
 * 
 * \param[in] lex Lexer to be used.
 * \returns Current character or terminating-null if the end of the source is
 * reached.
*/
char lexer_next(lexer_t* lex);

/**
 * \brief Returns the character after the current location of the lexer.
 * 
 * \param[in] lex Lexer to be used.
 * \returns Next character or terminating-null if the end of the source is
 * reached.
*/
char lexer_peek(lexer_t* lex);

/**
 * \brief Checks if the current character is the same as the one specified and
 * moves to the next location if it does.
 * 
 * \param[in] lex Lexer to be used.
 * \returns True if the characters are the same, false otherwise.
*/
bool lexer_consume(lexer_t* lex, char ch);

/**
 * \brief Skips all characters that match a given predicate.
 * 
 * \details Skips characters while the predicate returns a non-zero value.
 * 
 * \param[in] lex Lexer to be used.
 * \param[in] pred Predicate to be used.
 * \returns Number of characters skipped.
*/
size_t lexer_skip(lexer_t* lex, bool(*pred)(lexer_t*));

/**
 * \brief Skips the specified number of characters.
 * 
 * \details Stops if the end of the source is reached.
 * 
 * \param[in] lex Lexer to be used.
 * \param[in] n Number of characters to be skipped.
*/
void lexer_skip_n(lexer_t* lex, size_t n);

/**
 * \brief Read a word token which can either be a keyword or an identifier and
 * add it to the token list.
 * 
 * \param[in] lex Lexer to be used.
*/
token_t* lexer_read_word(lexer_t* lex);

/**
 * \brief Read an octal integer token and add it to the token list.
 * 
 * \param[in] lex Lexer to be used.
*/
token_t* lexer_read_octal_integer(lexer_t* lex);

/**
 * \brief Read a binary integer token and add it to the token list.
 * 
 * \param[in] lex Lexer to be used.
*/
token_t* lexer_read_binary_integer(lexer_t* lex);

/**
 * \brief Read a decimal integer token and add it to the token list.
 * 
 * \param[in] lex Lexer to be used.
*/
token_t* lexer_read_decimal_number(lexer_t* lex);

/**
 * \brief Read a hexadecimal integer token and add it to the token list.
 * 
 * \param[in] lex Lexer to be used.
*/
token_t* lexer_read_hexadecimal_integer(lexer_t* lex);

/**
 * \brief Read a number token which can either be a binary, octal, decimal or
 * hexadecimal integer or a floating-point number and add it to the token list.
 * 
 * \param[in] lex Lexer to be used.
*/
token_t* lexer_read_number(lexer_t* lex);

/**
 * \brief Read a string token and add it to the token list.
 * 
 * \param[in] lex Lexer to be used.
*/
token_t* lexer_read_string(lexer_t* lex);

/**
 * \brief Read a character token and add it to the token list.
 * 
 * \param[in] lex Lexer to be used.
*/
token_t* lexer_read_character(lexer_t* lex);

/**
 * \brief Read a punctuation token and add it to the token list.
 * 
 * \param[in] lex Lexer to be used.
*/
token_t* lexer_read_punctuation(lexer_t* lex);

/**
 * \brief Read a token and add it to the token list.
 * 
 * \param[in] lex Lexer to be used.
*/
token_t* lexer_read_next(lexer_t* lex);

/**
 * \brief Processes source while there are tokens to read.
 * 
 * \details It is the caller's responsibility to properly free the tokens.
 * 
 * \param[in] lex Lexer to be used.
 * \param[out] toks List to output tokens into.
*/
void lexer_lex(lexer_t* lex, list_t* toks);

#endif