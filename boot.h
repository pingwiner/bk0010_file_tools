#ifndef __BOOT_H__
#define __BOOT_H__

#include <stdint.h>
#include <stdio.h>
#include "err_code.h"

err_code boot_init(FILE* f, size_t offset);

uint16_t boot_blocks_count();

size_t boot_fat_offset();

size_t boot_dir_offset();

size_t boot_image_offset();

size_t boot_cluster_size();

int boot_test();

#endif //__BOOT_H__