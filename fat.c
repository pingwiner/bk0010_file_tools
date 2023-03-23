#include "fat.h"
#include <string.h>
#include "types.h"
#include "const.h"

static uint8_t fat1[SECTOR_SIZE * 2];
static uint8_t fat2[SECTOR_SIZE * 2];

#define MAX_CLUSTER_INDEX 681

err_code read_fat(uint16_t fat_size, FILE* f) {
  int read_result = fread(fat1, fat_size, 1, f);
  if (read_result != 1) {
    return ERR_DISK_IO;
  }

  read_result = fread(fat2, fat_size, 1, f);
  if (read_result != 1) {
    return ERR_DISK_IO;
  }

  return ERR_OK;
}

bool is_reserved(uint16_t value) {
  return ((value >= FAT_RSRVD) && (value < FAT_BAD));
}

uint16_t get_fat_element(uint16_t index) {
  if (index > MAX_CLUSTER_INDEX) return FAT_BAD;
  uint16_t offset = index * 3 / 2;
  uint16_t value = *( (uint16_t*) &fat1[offset] );
  if (index & 1) {
    return value >> 4;
  } else {
    return value & 0xFFF;
  }
}

void set_fat_element(uint16_t index, uint16_t value) {
    if (index > MAX_CLUSTER_INDEX) return;
    uint16_t offset = index * 3 / 2;
    uint16_t* p = (uint16_t*) &fat1[offset];
    uint16_t tmp = *p;
    if (index & 1) {
        tmp &= 0xF;
        tmp |= (value << 4);
    } else {
        tmp &= 0xF000;
        tmp |= (value & 0xFFF);
    }
    *p = tmp;
}

err_code fat_sync(FILE* f) {
    memcpy(fat2, fat1, sizeof(fat1));
    fseek(f, SECTOR_SIZE, SEEK_SET);
    int write_result = fwrite(fat1, sizeof(fat1), 1, f);
    if (write_result != 1) {
        return ERR_DISK_IO;
    }

    write_result = fwrite(fat2, sizeof(fat2), 1, f);
    if (write_result != 1) {
        return ERR_DISK_IO;
    }

    return ERR_OK;
}

int fat_test() {
  if (memcmp(fat1, fat2, SECTOR_SIZE * 2) != 0) return 1;
  if (fat1[0] != 0xF9) return 1;
  if (fat1[1] != 0xFF) return 1;
  if (fat1[2] != 0xFF) return 1;

  return 0;
}