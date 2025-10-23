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
typedef struct tau_command_env_var_t
{
  tau_string_t* key;   ///< The key of the environment variable.
  tau_string_t* value; ///< The value of the environment variable.
} tau_command_env_var_t;

struct tau_command_t
{
  tau_string_t* prog;
  tau_string_t* cwd;

  tau_vector_t* args;
  tau_vector_t* env;

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
static tau_command_env_var_t* tau_command_env_var_init(const char* key, const char* value)
{
  tau_command_env_var_t* var = (tau_command_env_var_t*)malloc(sizeof(tau_command_env_var_t));

  var->key = tau_string_init_with_cstr(key);
  var->value = tau_string_init_with_cstr(value);

  return var;
}

/**
 * \brief Frees all memory associated with an environment variable.
 *
 * \param[in] var Pointer to the environment variable to be freed.
 */
static void tau_command_env_var_free(tau_command_env_var_t* var)
{
  tau_string_free(var->key);
  tau_string_free(var->value);
  free(var);
}

#if TAU_OS_WINDOWS

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
static LPTSTR tau_command_get_commandline_tstr(tau_command_t* cmd)
{
  tau_string_t* commandline_str = tau_string_init();
  tau_string_append(commandline_str, cmd->prog);

  TAU_VECTOR_FOR_LOOP(i, cmd->args)
  {
    tau_string_t* arg = (tau_string_t*)tau_vector_get(cmd->args, i);

    tau_string_append_cstr(commandline_str, " ");
    tau_string_append(commandline_str, arg);
  }

  LPTSTR commandline_tstr = cstr_to_tstr(tau_string_begin(commandline_str), tau_string_length(commandline_str) + 1);

  tau_string_free(commandline_str);

  return commandline_tstr;
}

/**
 * \brief Frees a TCHAR string created by the `tau_command_get_commandline_tstr` function.
 *
 * \param[in] tstr The TCHAR string to be freed.
 */
static void tau_command_get_commandline_tstr_free(LPTSTR tstr)
{
  cstr_to_tstr_free(tstr);
}

/**
 * \brief Returns a string of environment variables as a TCHAR string.
 *
 * \param[in] cmd Pointer to the command to be used.
 * \returns TCHAR string of environment variables.
 */
static LPTSTR tau_command_get_environment_tstr(tau_command_t* cmd)
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

  TAU_VECTOR_FOR_LOOP(i, cmd->env)
  {
    tau_command_env_var_t* var = (tau_command_env_var_t*)tau_vector_get(cmd->env, i);

    cmd_env_len += tau_string_length(var->key) + 1; // key + '='
    cmd_env_len += tau_string_length(var->value) + 1; // value + '\0'
  }

  cmd_env_len += 1; // '\0'

  char* cmd_env_cstr = (char*)malloc(sizeof(char) * cmd_env_len);
  char* cmd_env_ptr = cmd_env_cstr;

  TAU_VECTOR_FOR_LOOP(i, cmd->env)
  {
    tau_command_env_var_t* var = (tau_command_env_var_t*)tau_vector_get(cmd->env, i);

    strcpy(cmd_env_ptr, tau_string_begin(var->key));
    cmd_env_ptr += tau_string_length(var->key);

    *cmd_env_ptr++ = '=';

    strcpy(cmd_env_ptr, tau_string_begin(var->value));
    cmd_env_ptr += tau_string_length(var->value);

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
 * \brief Frees a TCHAR string created by the `tau_command_get_environment_tstr` function.
 *
 * \param[in] tstr The TCHAR string to be freed.
 */
static void tau_command_get_environment_tstr_free(LPTSTR tstr)
{
  free(tstr);
}

tau_command_t* tau_command_init(const char* prog)
{
  tau_command_t* cmd = (tau_command_t*)malloc(sizeof(tau_command_t));

  cmd->prog = tau_string_init_with_cstr(prog);
  cmd->cwd = NULL;
  cmd->args = tau_vector_init();
  cmd->env = tau_vector_init();
  cmd->stream_in = NULL;
  cmd->stream_out = NULL;
  cmd->stream_err = NULL;

  return cmd;
}

void tau_command_free(tau_command_t* cmd)
{
  tau_string_free(cmd->prog);

  if (cmd->cwd != NULL)
    tau_string_free(cmd->cwd);

  tau_vector_for_each(cmd->args, tau_string_free);
  tau_vector_free(cmd->args);

  tau_vector_for_each(cmd->env, tau_command_env_var_free);
  tau_vector_free(cmd->env);

  free(cmd);
}

void tau_command_add_arg(tau_command_t* cmd, const char* arg)
{
  tau_vector_push(cmd->args, tau_string_init_with_cstr(arg));
}

void tau_command_set_cwd(tau_command_t* cmd, const char* cwd)
{
  if (cmd->cwd != NULL)
  {
    tau_string_clear(cmd->cwd);
    tau_string_append_cstr(cmd->cwd, cwd);
  }
  else
  {
    cmd->cwd = tau_string_init_with_cstr(cwd);
  }
}

void tau_command_add_env(tau_command_t* cmd, const char* key, const char* value)
{
  tau_vector_push(cmd->env, tau_command_env_var_init(key, value));
}

void tau_command_set_stdin(tau_command_t* cmd, FILE* stream)
{
  cmd->stream_in = stream;
}

void tau_command_set_stdout(tau_command_t* cmd, FILE* stream)
{
  cmd->stream_out = stream;
}

void tau_command_set_stderr(tau_command_t* cmd, FILE* stream)
{
  cmd->stream_err = stream;
}

int tau_command_run(tau_command_t* cmd)
{
  LPTSTR commandline_tstr = tau_command_get_commandline_tstr(cmd);
  LPTSTR environemnt_tstr = tau_command_get_environment_tstr(cmd);
  LPTSTR cwd_tstr = cmd->cwd == NULL ? NULL : cstr_to_tstr(tau_string_begin(cmd->cwd), tau_string_length(cmd->cwd) + 1);

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

  tau_command_get_environment_tstr_free(environemnt_tstr);
  tau_command_get_commandline_tstr_free(commandline_tstr);

  if (!result)
  {
    tau_log_error("windows", "CreateProcess failed! (%d)", error);
    return error;
  }

  WaitForSingleObject(process_info.hProcess, INFINITE);

  DWORD exit_code = 0;
  GetExitCodeProcess(process_info.hProcess, &exit_code);

  CloseHandle(process_info.hProcess);
  CloseHandle(process_info.hThread);

  return (int)exit_code;
}

void tau_command_reset(tau_command_t* cmd)
{
  if (cmd->cwd != NULL)
  {
    tau_string_free(cmd->cwd);
    cmd->cwd = NULL;
  }

  tau_vector_for_each(cmd->args, tau_string_free);
  tau_vector_clear(cmd->args);

  tau_vector_for_each(cmd->env, tau_command_env_var_free);
  tau_vector_clear(cmd->env);
}

#elif TAU_OS_LINUX

tau_command_t* tau_command_init(const char* TAU_UNUSED(prog))
{
  TAU_UNREACHABLE();
  return NULL;
}

void tau_command_free(tau_command_t* TAU_UNUSED(cmd))
{
  TAU_UNREACHABLE();
}

void tau_command_add_arg(tau_command_t* TAU_UNUSED(cmd), const char* TAU_UNUSED(arg))
{
  TAU_UNREACHABLE();
}

void tau_command_set_cwd(tau_command_t* TAU_UNUSED(cmd), const char* TAU_UNUSED(cwd))
{
  TAU_UNREACHABLE();
}

void tau_command_add_env(tau_command_t* TAU_UNUSED(cmd), const char* TAU_UNUSED(key), const char* TAU_UNUSED(value))
{
  TAU_UNREACHABLE();
}

void tau_command_set_stdin(tau_command_t* TAU_UNUSED(cmd), FILE* TAU_UNUSED(stream))
{
  TAU_UNREACHABLE();
}

void tau_command_set_stdout(tau_command_t* TAU_UNUSED(cmd), FILE* TAU_UNUSED(stream))
{
  TAU_UNREACHABLE();
}

void tau_command_set_stderr(tau_command_t* TAU_UNUSED(cmd), FILE* TAU_UNUSED(stream))
{
  TAU_UNREACHABLE();
}

int tau_command_run(tau_command_t* TAU_UNUSED(cmd))
{
  TAU_UNREACHABLE();
  return EXIT_FAILURE;
}

void tau_command_reset(tau_command_t* TAU_UNUSED(cmd))
{
  TAU_UNREACHABLE();
}

#else
# error "Commands are not implemented for this platform!"
#endif
