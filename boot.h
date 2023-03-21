#ifndef __BOOT_H__
#define __BOOT_H__

#include <stdint.h>
#include <stdio.h>
#include "err_code.h"

err_code read_boot_sector(FILE* f);

uint8_t	get_blocks_per_cluster();

uint16_t get_blocks_count();

uint16_t get_bytes_per_block();

uint16_t get_bootloader_size();

uint16_t get_blocks_per_fat();

uint8_t get_fat_tables_count();

int boot_test(); 

#endif //__BOOT_H__