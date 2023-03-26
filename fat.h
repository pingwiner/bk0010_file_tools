#ifndef __FAT_H__
#define __FAT_H__

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include "err_code.h"
#include "types.h"

#define FAT_FREE 	0
#define FAT_EOF 	0xFFF
#define FAT_BAD		0xFF7
#define FAT_RSRVD	0xFF0

err_code fat_init(FILE* f);

bool fat_is_reserved(uint16_t value);

uint16_t fat_get_element(uint16_t index);

void fat_set_element(uint16_t index, uint16_t value);

err_code fat_sync(FILE* f);

uint16_t fat_find_free_cluster();

int fat_test();

#endif //__FAT_H__
