#ifndef __FILE_H__
#define __FILE_H__

#include "err_code.h"
#include "directory.h"

err_code extract_file(const FileParams* params, FILE* image_file, const char* dest_file);

int file_test(FILE* f);

#endif //__FILE_H__
