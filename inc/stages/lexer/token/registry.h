/**
* \file
 *
 * \brief Token registry.
 *
 * \details The token registry is a singleton for managing and tracking tokens
 * in a centralized manner. This registry provides a convenient way to register
 * tokens and free them all at once.
 *
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_TOKEN_REGISTRY_H
#define TAU_TOKEN_REGISTRY_H

#include "stages/lexer/token/token.h"

/**
 * \brief Adds a file to associate tokens with in the registry.
 *
 * \param[in] path Path to the source file.
 * \param[in] src The contents of the source file.
 */
void token_registry_register_file(const char* path, const char* src);

/**
 * \brief Adds a new token to the registry.
 *
 * \param[in] path Path to the associated source file.
 * \param[in] kind The kind of the token.
 * \param[in] pos The position of the token in the source file.
 * \returns Pointer to the newly registered token.
 */
token_t* token_registry_token_init(const char* path, token_kind_t kind, size_t pos);

/**
 * \brief Retrieves the path and contents of the source file associated with a token.
 *
 * \param[in] tok Pointer to the token.
 * \param[out] path Pointer to a variable where the path is to be written.
 * \param[out] src Pointer to a variable where the contents are to be written.
 */
void token_registry_path_and_src(token_t* tok, const char** path, const char** src);

/**
 * \brief Frees all registered tokens.
 */
void token_registry_free(void);

#endif
