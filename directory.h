#ifndef __DIRECTORY_H__
#define __DIRECTORY_H__

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include "err_code.h"
#include "types.h"

typedef struct FileParams {
	char filename[16];
    size_t size;
    size_t first_cluster;
    bool is_directory;
	uint16_t address;
    uint16_t index;
} FileParams;

typedef void (*OnNextFile)(char* filename, char* extension, uint16_t address, uint32_t size, bool is_directory);

err_code dir_init(FILE* f);

void dir_list(uint8_t dir, OnNextFile on_next_file);

err_code dir_file_by_name(const char* filename, FileParams* params);

int dir_test();

#endif //__DIRECTORY_H__