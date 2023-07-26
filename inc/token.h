/**
 * \file
 * 
 * Tokens are atomic words of the language such as identifiers,
 * literals, keywords and punctuations.
*/

#ifndef TAU_TOKEN_H
#define TAU_TOKEN_H

#include <stdbool.h>
#include <stdio.h>

#include "list.h"
#include "location.h"
#include "typedefs.h"

struct token_s
{
  token_kind_t kind;
  location_t* loc;
};

/**
 * \brief Initializes a token instance.
 * 
 * \param[out] tok Token instance to be initialized.
 * \param[in] kind Token kind.
 * \param[in] loc Token location.
*/
void token_init(token_t* tok, token_kind_t kind, location_t* loc);

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
