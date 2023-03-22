#ifndef __DIRECTORY_H__
#define __DIRECTORY_H__

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include "err_code.h"

typedef struct FileParams {
	char filename[16];
    size_t size;
    size_t first_cluster;
	uint16_t address;
    uint16_t index;
} FileParams;

typedef void (*OnNextFile)(char* filename, char* extension, uint16_t address, uint32_t size);

err_code read_directory(FILE* f); 

void list_directory(OnNextFile on_next_file);

err_code find_file_by_name(const char* filename, FileParams* params);

int directory_test();

#endif //__DIRECTORY_H__