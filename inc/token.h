/**
 * \file
 * 
 * Tokens are atomic words of the language such as identifiers,
 * literals, keywords and punctuations.
*/

#ifndef TAU_TOKEN_H
#define TAU_TOKEN_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "typedefs.h"

/** Utility macro which expands to fields that all tokens must have. */
#define TOKEN_HEADER\
  struct\
  {\
    token_kind_t kind;\
    location_t* loc;\
  }

struct token_s
{
  TOKEN_HEADER;
};

struct token_id_s
{
  TOKEN_HEADER;
  char* value;
};

struct token_lit_int_s
{
  TOKEN_HEADER;
  uint64_t value;
};

struct token_lit_flt_s
{
  TOKEN_HEADER;
  long double value;
};

struct token_lit_str_s
{
  TOKEN_HEADER;
  char* value;
};

struct token_lit_char_s
{
  TOKEN_HEADER;
  char value;
};

struct token_lit_bool_s
{
  TOKEN_HEADER;
  bool value;
};

#undef TOKEN_HEADER

/**
 * \brief Initializes a new token.
 * 
 * \param[in] kind Token kind.
 * \param[in] loc Token location.
 * \returns New token.
*/
token_t* token_init(token_kind_t kind, location_t* loc);

/**
 * \brief Initializes a new identifier token.
 * 
 * \param[in] loc Token location.
 * \param[in] value Identifier string.
 * \returns New identifier token.
*/
token_id_t* token_id_init(location_t* loc, char* value);

/**
 * \brief Initializes a new integer literal token.
 * 
 * \param[in] loc Token location.
 * \param[in] value Integer value.
 * \returns New integer literal token.
*/
token_lit_int_t* token_lit_int_init(location_t* loc, uint64_t value);

/**
 * \brief Initializes a new floating-point literal token.
 * 
 * \param[in] loc Token location.
 * \param[in] value Floating-point value.
 * \returns New floating-point literal token.
*/
token_lit_flt_t* token_lit_flt_init(location_t* loc, long double value);

/**
 * \brief Initializes a new string literal token.
 * 
 * \param[in] loc Token location.
 * \param[in] value String value.
 * \returns New string literal token.
*/
token_lit_str_t* token_lit_str_init(location_t* loc, char* value);

/**
 * \brief Initializes a new character literal token.
 * 
 * \param[in] loc Token location.
 * \param[in] value Character value.
 * \returns New character literal token.
*/
token_lit_char_t* token_lit_char_init(location_t* loc, char value);

/**
 * \brief Destroys a token.
 * 
 * \param[in] tok Token to be destroyed.
*/
void token_free(token_t* tok);

/**
 * \brief Writes a JSON dump of a list of tokens into a stream.
 * 
 * \param[in] stream Stream to be written to.
 * \param[in] list List of tokens to be dumped.
*/
void token_list_json_dump(FILE* stream, list_t* list);

/**
 * \brief Returns a string representation of a token kind.
 * 
 * \param[in] kind Token kind.
 * \returns String representation.
*/
const char* token_kind_to_string(token_kind_t kind);

/**
 * \brief Checks if a token is a literal.
 * 
 * \param[in] tok Token to be checked.
 * \returns True if token is a literal, false otherwise.
*/
bool token_is_literal(token_t* tok);

/**
 * \brief Checks if a token is a literal integer.
 * 
 * \param[in] tok Token to be checked.
 * \returns True if token is a literal integer, false otherwise.
*/
bool token_is_literal_integer(token_t* tok);

/**
 * \brief Checks if a token is a literal float.
 * 
 * \param[in] tok Token to be checked.
 * \returns True if token is a literal float, false otherwise.
*/
bool token_is_literal_float(token_t* tok);

/**
 * \brief Checks if a token is a keyword.
 * 
 * \param[in] tok Token to be checked.
 * \returns True if token is a keyword, false otherwise.
*/
bool token_is_keyword(token_t* tok);

/**
 * \brief Checks if a token is a punctuation.
 * 
 * \param[in] tok Token to be checked.
 * \returns True if token is a punctuation, false otherwise.
*/
bool token_is_punctuation(token_t* tok);

#endif
