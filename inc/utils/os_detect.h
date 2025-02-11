/**
 * \file
 *
 * \brief Platform operating system detection header.
 *
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_OS_DETECT_H
#define TAU_OS_DETECT_H

#if defined(_WIN32) || defined(_WIN64) || defined(WIN32) || defined(WIN64) || defined(__WINDOWS__)
# define TAU_OS_WINDOWS 1
#else
# define TAU_OS_WINDOWS 0
#endif

#if defined(__APPLE__) && defined(__MACH__) || defined(__DARWIN__) || defined(macintosh) || defined(Macintosh)
# define TAU_OS_MACOS 1
#else
# define TAU_OS_MACOS 0
#endif

#if defined(__linux__) || defined(__linux) || defined(linux) || defined(__gnu_linux__)
# define TAU_OS_LINUX 1
#else
# define TAU_OS_LINUX 0
#endif

#if TAU_OS_MACOS || TAU_OS_LINUX || defined(unix) || defined(__unix__) || defined(__unix)
# define TAU_OS_UNIX 1
#else
# define TAU_OS_UNIX 0
#endif

#if !TAU_OS_WINDOWS && !TAU_OS_MACOS && !TAU_OS_LINUX
# error "Unknown operating system!"
#endif

#endif
