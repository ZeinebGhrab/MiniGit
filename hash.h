#ifndef HASH_H
#define HASH_H

#include "list.h"
#include <limits.h>

#define HASH_PATH_SIZE 256

int hashFile(const char* source, const char* dest);
char* sha256file(const char* file);
List* listdir(const char* root_dir);
int file_exists(const char* file);
void cp(const char* to, const char* from);
char* hashToPath(const char* hash);
int blobFile(const char* file);

#endif
