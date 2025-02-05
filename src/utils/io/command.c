/**
 * \file
 *
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "utils/io/command.h"

#include "utils/str.h"
#include "utils/collections/vector.h"

/**
 * \brief Represents an environment variable.
 */
typedef struct command_env_var_t
{
  string_t* key;   ///< The key of the environment variable.
  string_t* value; ///< The value of the environment variable.
} command_env_var_t;

struct command_t
{
  string_t* prog;
  string_t* cwd;

  vector_t* args;
  vector_t* env;

  FILE* stream_in;
  FILE* stream_out;
  FILE* stream_err;
};

/**
 * \brief Initalizes a new environment variable.
 *
 * \param[in] key The key of the environment variable.
 * \param[in] value The value of the environment variable.
 * \returns Pointer to the newly initialized environment variable.
 */
static command_env_var_t* command_env_var_init(const char* key, const char* value)
{
  command_env_var_t* var = (command_env_var_t*)malloc(sizeof(command_env_var_t));

  var->key = string_init_with_cstr(key);
  var->value = string_init_with_cstr(value);

  return var;
}

/**
 * \brief Frees all memory associated with an environment variable.
 *
 * \param[in] var Pointer to the environment variable to be freed.
 */
static void command_env_var_free(command_env_var_t* var)
{
  string_free(var->key);
  string_free(var->value);
  free(var);
}

#if defined(WIN32) || defined(_WIN32)

#include <windows.h>

#include <io.h>
#include <processthreadsapi.h>
#include <tchar.h>

/**
 * \brief Converts a TCHAR string to an ANSI CHAR string.
 *
 * \param[in] tstr The TCHAR string to convert.
 * \param[in] size Size of TCHAR string in characters.
 * \returns The converted ANSI CHAR string.
 */
static LPSTR tstr_to_cstr(LPTSTR tstr, size_t size)
{
#ifdef UNICODE

  size_t buf_size = (size_t)WideCharToMultiByte(CP_ACP, 0, tstr, (int)size, NULL, 0, NULL, NULL);
  LPSTR buf = (LPSTR)malloc(buf_size);
  WideCharToMultiByte(CP_ACP, 0, tstr, (int)size, buf, (int)buf_size, NULL, NULL);

#else

  size_t buf_size = strlen((const char*)tstr);
  char* buf = (char*)malloc(sizeof(char) * (buf_size + 1));
  strncpy(buf, (const char*)tstr, buf_size + 1);

#endif

  return (LPSTR)buf;
}

/**
 * \brief Frees an ANSI CHAR string created by the `tstr_to_cstr` function.
 *
 * \param[in] cstr The ANSI CHAR string to be freed.
 */
static void tstr_to_cstr_free(LPSTR cstr)
{
  free(cstr);
}

/**
 * \brief Converts an ANSI CHAR string to a TCHAR string.
 *
 * \param[in] cstr The ANSI CHAR string to convert.
 * \param[in] size Size of ANSI CHAR string in bytes.
 * \returns The converted TCHAR string.
 */
static LPTSTR cstr_to_tstr(LPSTR cstr, size_t size)
{
#ifdef UNICODE

  size_t buf_size = (size_t)MultiByteToWideChar(CP_ACP, 0, cstr, (int)size, NULL, 0);
  LPWSTR buf = (LPWSTR)malloc(sizeof(WCHAR) * buf_size);
  MultiByteToWideChar(CP_ACP, 0, cstr, (int)size, buf, (int)buf_size);

#else

  size_t buf_size = strlen((const char*)tstr);
  char* buf = (char*)malloc(sizeof(char) * (buf_size + 1));
  strncpy((char*)buf, (const char*)tstr, buf_size + 1);

#endif

  return (LPTSTR)buf;
}

/**
 * \brief Frees a TCHAR string created by the `cstr_to_tstr` function.
 *
 * \param[in] tstr The TCHAR string to be freed.
 */
static void cstr_to_tstr_free(LPTSTR tstr)
{
  free(tstr);
}

/**
 * \brief Returns the space separated commandline TCHAR string of the command.
 *
 * \param[in] cmd Pointer to the command to be used.
 * \returns Space separated commandline TCHAR string.
 */
static LPTSTR command_get_commandline_tstr(command_t* cmd)
{
  string_t* commandline_str = string_init();
  string_append(commandline_str, cmd->prog);

  VECTOR_FOR_LOOP(i, cmd->args)
  {
    string_t* arg = (string_t*)vector_get(cmd->args, i);

    string_append_cstr(commandline_str, " ");
    string_append(commandline_str, arg);
  }

  LPTSTR commandline_tstr = cstr_to_tstr(string_begin(commandline_str), string_length(commandline_str) + 1);

  string_free(commandline_str);

  return commandline_tstr;
}

/**
 * \brief Frees a TCHAR string created by the `command_get_commandline_tstr` function.
 *
 * \param[in] tstr The TCHAR string to be freed.
 */
static void command_get_commandline_tstr_free(LPTSTR tstr)
{
  cstr_to_tstr_free(tstr);
}

/**
 * \brief Returns a string of environment variables as a TCHAR string.
 *
 * \param[in] cmd Pointer to the command to be used.
 * \returns TCHAR string of environment variables.
 */
static LPTSTR command_get_environment_tstr(command_t* cmd)
{
  LPTSTR default_env_tstr = GetEnvironmentStrings();

  size_t default_env_len = 0;
  LPTSTR default_env_ptr = default_env_tstr;

  while (*default_env_ptr)
  {
    default_env_len += _tcslen(default_env_ptr) + 1; // key + value + '\0'
    default_env_ptr += _tcslen(default_env_ptr) + 1;
  }

  default_env_len += 1; // '\0'

  size_t cmd_env_len = 0;

  VECTOR_FOR_LOOP(i, cmd->env)
  {
    command_env_var_t* var = (command_env_var_t*)vector_get(cmd->env, i);

    cmd_env_len += string_length(var->key) + 1; // key + '='
    cmd_env_len += string_length(var->value) + 1; // value + '\0'
  }

  cmd_env_len += 1; // '\0'

  char* cmd_env_cstr = (char*)malloc(sizeof(char) * cmd_env_len);
  char* cmd_env_ptr = cmd_env_cstr;

  VECTOR_FOR_LOOP(i, cmd->env)
  {
    command_env_var_t* var = (command_env_var_t*)vector_get(cmd->env, i);

    strcpy(cmd_env_ptr, string_begin(var->key));
    cmd_env_ptr += string_length(var->key);

    *cmd_env_ptr++ = '=';

    strcpy(cmd_env_ptr, string_begin(var->value));
    cmd_env_ptr += string_length(var->value);

    *cmd_env_ptr++ = '\0';
  }

  *cmd_env_ptr = '\0';

  LPTSTR cmd_env_tstr = cstr_to_tstr((LPSTR)cmd_env_cstr, cmd_env_len);

  size_t joined_env_len = default_env_len + cmd_env_len;
  LPTSTR joined_env_tstr = (LPTSTR)malloc(sizeof(TCHAR) * joined_env_len);

  memcpy(joined_env_tstr, default_env_tstr, default_env_len * sizeof(TCHAR));
  memcpy(joined_env_tstr + default_env_len - 1, cmd_env_tstr, cmd_env_len * sizeof(TCHAR));

  cstr_to_tstr_free(cmd_env_tstr);
  free(cmd_env_cstr);
  FreeEnvironmentStrings(default_env_tstr);

  return joined_env_tstr;
}

/**
 * \brief Frees a TCHAR string created by the `command_get_environment_tstr` function.
 *
 * \param[in] tstr The TCHAR string to be freed.
 */
static void command_get_environment_tstr_free(LPTSTR tstr)
{
  free(tstr);
}

command_t* command_init(const char* prog)
{
  command_t* cmd = (command_t*)malloc(sizeof(command_t));

  cmd->prog = string_init_with_cstr(prog);
  cmd->cwd = NULL;
  cmd->args = vector_init();
  cmd->env = vector_init();
  cmd->stream_in = NULL;
  cmd->stream_out = NULL;
  cmd->stream_err = NULL;

  return cmd;
}

void command_free(command_t* cmd)
{
  string_free(cmd->prog);

  if (cmd->cwd != NULL)
    string_free(cmd->cwd);

  vector_for_each(cmd->args, string_free);
  vector_free(cmd->args);

  vector_for_each(cmd->env, command_env_var_free);
  vector_free(cmd->env);

  free(cmd);
}

void command_add_arg(command_t* cmd, const char* arg)
{
  vector_push(cmd->args, string_init_with_cstr(arg));
}

void command_set_cwd(command_t* cmd, const char* cwd)
{
  if (cmd->cwd != NULL)
  {
    string_clear(cmd->cwd);
    string_append_cstr(cmd->cwd, cwd);
  }
  else
  {
    cmd->cwd = string_init_with_cstr(cwd);
  }
}

void command_add_env(command_t* cmd, const char* key, const char* value)
{
  vector_push(cmd->env, command_env_var_init(key, value));
}

void command_set_stdin(command_t* cmd, FILE* stream)
{
  cmd->stream_in = stream;
}

void command_set_stdout(command_t* cmd, FILE* stream)
{
  cmd->stream_out = stream;
}

void command_set_stderr(command_t* cmd, FILE* stream)
{
  cmd->stream_err = stream;
}

int command_run(command_t* cmd)
{
  LPTSTR commandline_tstr = command_get_commandline_tstr(cmd);
  LPTSTR environemnt_tstr = command_get_environment_tstr(cmd);
  LPTSTR cwd_tstr = cmd->cwd == NULL ? NULL : cstr_to_tstr(string_begin(cmd->cwd), string_length(cmd->cwd) + 1);

  DWORD flags = 0;

#ifdef UNICODE
  flags |= CREATE_UNICODE_ENVIRONMENT;
#endif

  STARTUPINFO startup_info;
  ZeroMemory(&startup_info, sizeof(STARTUPINFO));
  startup_info.cb = sizeof(STARTUPINFO);

  if (cmd->stream_in != NULL || cmd->stream_out != NULL || cmd->stream_err != NULL)
    startup_info.dwFlags = STARTF_USESTDHANDLES;

  if (cmd->stream_in != NULL)
    startup_info.hStdInput = (HANDLE)_get_osfhandle(_fileno(cmd->stream_in));

  if (cmd->stream_out != NULL)
    startup_info.hStdOutput = (HANDLE)_get_osfhandle(_fileno(cmd->stream_out));

  if (cmd->stream_err != NULL)
    startup_info.hStdError = (HANDLE)_get_osfhandle(_fileno(cmd->stream_err));

  PROCESS_INFORMATION process_info;
  ZeroMemory(&process_info, sizeof(PROCESS_INFORMATION));

  BOOL result = CreateProcess(
    NULL,
    commandline_tstr,
    NULL,
    NULL,
    TRUE,
    flags,
    environemnt_tstr,
    cwd_tstr,
    &startup_info,
    &process_info
  );

  DWORD error = GetLastError();

  if (cwd_tstr != NULL)
    cstr_to_tstr_free(cwd_tstr);

  command_get_environment_tstr_free(environemnt_tstr);
  command_get_commandline_tstr_free(commandline_tstr);

  if (!result)
  {
    log_error("windows", "CreateProcess failed! (%d)", error);
    return error;
  }

  WaitForSingleObject(process_info.hProcess, INFINITE);

  DWORD exit_code = 0;
  GetExitCodeProcess(process_info.hProcess, &exit_code);

  CloseHandle(process_info.hProcess);
  CloseHandle(process_info.hThread);

  return (int)exit_code;
}

void command_reset(command_t* cmd)
{
  if (cmd->cwd != NULL)
  {
    string_free(cmd->cwd);
    cmd->cwd = NULL;
  }

  vector_for_each(cmd->args, string_free);
  vector_clear(cmd->args);

  vector_for_each(cmd->env, command_env_var_free);
  vector_clear(cmd->env);
}

#elif defined TAU_OS_LINUX

command_t* command_init(const char* UNUSED(prog)) {
  return NULL;
}

void command_free(command_t* UNUSED(cmd)) {

}

void command_add_arg(command_t* UNUSED(cmd), const char* UNUSED(arg)) {
  
}

void command_set_cwd(command_t* UNUSED(cmd), const char* UNUSED(cwd)) {

}

void command_add_env(command_t* UNUSED(cmd), const char* UNUSED(key), const char* UNUSED(value)) {

}

void command_set_stdin(command_t* UNUSED(cmd), FILE* UNUSED(stream)) {

}

void command_set_stdout(command_t* UNUSED(cmd), FILE* UNUSED(stream)) {

}

void command_set_stderr(command_t* UNUSED(cmd), FILE* UNUSED(stream)) {

}

int command_run(command_t* UNUSED(cmd)) {
  return 0;
}

void command_reset(command_t* UNUSED(cmd)) {

}

#else
//# error "Commands are not implemented for this platform!"
#endif
