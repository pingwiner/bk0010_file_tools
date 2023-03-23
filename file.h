#ifndef __FILE_H__
#define __FILE_H__

#include "err_code.h"
#include "directory.h"

err_code file_extract(FILE* image_file, const FileParams* params, const char* dest_file);

err_code file_delete(FILE* image_file, const FileParams* params);

int file_test(FILE* f);

#endif //__FILE_H__
