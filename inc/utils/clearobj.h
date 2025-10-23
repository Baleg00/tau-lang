/**
 * \file
 *
 * \brief TAU_CLEAROBJ utility macro.
 *
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_CLEAROBJ_H
#define TAU_CLEAROBJ_H

#include <string.h>

/**
 * \brief Fill the memory of an object with zeros.
 *
 * \param[in,out] OBJ Pointer to the object to be cleared.
*/
#define TAU_CLEAROBJ(OBJ) ((void)memset((OBJ), 0, sizeof(*(OBJ))))

#endif
