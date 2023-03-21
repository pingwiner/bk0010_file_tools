#include "fat.h"
#include <string.h>
#include "types.h"

static uint8_t* fat1;
static uint8_t* fat2;
static uint16_t max_cluster_index;

err_code read_fat(uint16_t fat_size, FILE* f) {
  fat1 = (uint8_t*) malloc(fat_size); 
  if (!fat1) {
    return ERR_MEMORY_ALLOCATION;
  }

  fat2 = (uint8_t*) malloc(fat_size); 
  if (!fat2) {
    free(fat1);
    return ERR_MEMORY_ALLOCATION;
  }

  int read_result = fread(fat1, fat_size, 1, f);
  if (read_result != 1) {
    free(fat1);
    free(fat2);
    return ERR_DISK_IO;
  }

  read_result = fread(fat2, fat_size, 1, f);
  if (read_result != 1) {
    free(fat1);
    free(fat2);
    return ERR_DISK_IO;
  }

  max_cluster_index = fat_size / 1.5;

  return ERR_OK;
}

bool is_reserved(uint16_t value) {
  return ((value >= FAT_RSRVD) && (value < FAT_BAD));
}

uint16_t get_fat_element(uint16_t index) {
  uint16_t offset = index * 1.5;
  uint16_t value = *( (uint16_t*) &fat1[offset] );
  if (index & 1) {
    return value >> 4;
  } else {
    return value & 0xFFF;
  }
}

void free_fat() {
  free(fat1);
  free(fat2);  
  max_cluster_index = 0;
}

int fat_test() {
  if (memcmp(fat1, fat2, 1024) != 0) return 1;
  if (fat1[0] != 0xF9) return 1;
  if (fat1[1] != 0xFF) return 1;
  if (fat1[2] != 0xFF) return 1;

  return 0;
}