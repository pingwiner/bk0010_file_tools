
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

void on_next_file(char* filename, char* extension, uint16_t address, uint32_t file_size, bool is_directory) {
  printf("%.8s %.3s    %.4x %d %s\n", filename, extension, address, file_size, is_directory ? "DIR" : "");
}

FILE* image_init(const char* filename) {
    FILE* f = fopen(img_path, "r+");
    if (f == null) {
        return f;
    }
    err_code err = boot_init(f, 0);
    if (err != ERR_OK) {
        fclose(f);
        return null;
    }
    return f;
}

int main() {
  FILE* f = image_init(img_path);
  if (f == null) return 1;

  if (fat_init(f) != ERR_OK) {
      fclose(f);
      return 1;
  }

  if (dir_init(f) != ERR_OK) {
      fclose(f);
      return 1;
  }


  //printf("filename ext    addr size\n");
  //printf("-------------------------\n");
  //list_directory(&on_next_file);


  if (boot_test() != 0) {
    printf("Boot test failed\n");
  }

  if (fat_test() != 0) {
    printf("FAT test failed\n");
  }

  dir_test();

  dir_list(0, on_next_file);

  file_test(f);

  fclose(f);
  return 0;
}

