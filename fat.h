#ifndef __FAT_H__
#define __FAT_H__

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include "err_code.h"

#define FAT_FREE 	0
#define FAT_EOF 	0xFFF
#define FAT_BAD		0xFF7
#define FAT_RSRVD	0xFF0

err_code read_fat(uint16_t fat_size, FILE* f);

void free_fat();

int fat_test();

#endif //__FAT_H__
