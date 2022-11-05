#ifndef TAU_FILE_H
#define TAU_FILE_H

#include <stddef.h>

void file_read_to_string(const char* path, size_t* len, char** src);

#endif