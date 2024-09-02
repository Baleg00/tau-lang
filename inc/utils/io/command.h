/**
 * \file
 *
 * \brief Terminal command interface.
 *
 * \details This library provides an API for constructing and executing terminal commands programmatically. It allows
 * the definition of command arguments, managing environment variables, and capturing the output or errors generated by
 * the executed commands. It abstracts away platform-specific details, enabling consistent command execution across
 * different operating systems.
 *
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_COMMAND_H
#define TAU_COMMAND_H

#include <stdio.h>

/**
 * \brief Represents a terminal command.
 */
typedef struct command_t command_t;

/**
 * \brief Initializes a new terminal command for a specific program.
 *
 * \param[in] prog The name or path of the program.
 * \returns Pointer to the newly initialized command.
 */
command_t* command_init(const char* prog);

/**
 * \brief Frees all memory associated with a command.
 *
 * \param[in] cmd Pointer to the command to be freed.
 */
void command_free(command_t* cmd);

/**
 * \brief Adds an argument to the command.
 *
 * \param[in] cmd Pointer to the command to be used.
 * \param[in] arg Argument to be added.
 */
void command_add_arg(command_t* cmd, const char* arg);

/**
 * \brief Sets the working directory of the command.
 *
 * \param[in] cmd Pointer to the command to be used.
 * \param[in] cwd Path to working directory.
 */
void command_set_cwd(command_t* cmd, const char* cwd);

/**
 * \brief Add an environment variable to the command.
 *
 * \param[in] cmd Pointer to the command to be used.
 * \param[in] key The key of the environment variable.
 * \param[in] value The value of the environment variable.
 */
void command_add_env(command_t* cmd, const char* key, const char* value);

/**
 * \brief Sets the input stream for the command.
 *
 * \param[in] cmd Pointer to the command to be used.
 * \param[in] stream Pointer to the input stream.
 */
void command_set_stdin(command_t* cmd, FILE* stream);

/**
 * \brief Sets the output stream for the command.
 *
 * \param[in] cmd Pointer to the command to be used.
 * \param[in] stream Pointer to the output stream.
 */
void command_set_stdout(command_t* cmd, FILE* stream);

/**
 * \brief Sets the error output stream for the command.
 *
 * \param[in] cmd Pointer to the command to be used.
 * \param[in] stream Pointer to the error output stream.
 */
void command_set_stderr(command_t* cmd, FILE* stream);

/**
 * \brief Run the command on a new process and return its exit status.
 *
 * \param[in] cmd Pointer to the command to be used.
 * \returns The process' exit status.
 */
int command_run(command_t* cmd);

/**
 * \brief Resets a command to its initial state.
 *
 * \param[in] cmd Pointer to the command to be used.
 */
void command_reset(command_t* cmd);

#endif