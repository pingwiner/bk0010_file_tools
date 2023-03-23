
#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "boot.h"
#include "fat.h"
#include "directory.h"
#include "err_code.h"
#include "file.h"

const char* img_path = "/Users/nightrain/bk/bk2010/disks/Andos330.img";

void on_next_file(char* filename, char* extension, uint16_t address, uint32_t file_size) {
  printf("%.8s %.3s    %.4x %d\n", filename, extension, address, file_size);
}

int main() {
  FILE* f = fopen(img_path, "r");
  read_boot_sector(f);
  
  uint16_t bytes_per_block = get_bytes_per_block();
  uint16_t bootloader_size = get_bootloader_size();
  uint16_t blocks_per_fat = get_blocks_per_fat();
  uint16_t fat_size = bytes_per_block * blocks_per_fat;
  uint8_t fat_tables_count = get_fat_tables_count();

  fseek(f, bytes_per_block * bootloader_size, SEEK_SET);
  read_fat(fat_size, f);
  fseek(f, bytes_per_block * (bootloader_size + blocks_per_fat * fat_tables_count), SEEK_SET);
  read_directory(f);

  //printf("filename ext    addr size\n");
  //printf("-------------------------\n");
  //list_directory(&on_next_file);


  if (boot_test() != 0) {
    printf("Boot test failed\n");
  }

  if (fat_test() != 0) {
    printf("FAT test failed\n");
  }

  directory_test();

  file_test(f);

  fclose(f);
  return 0;
}

