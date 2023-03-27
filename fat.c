#include "fat.h"
#include "boot.h"
#include <string.h>
#include "types.h"
#include "const.h"

//ANDOS always uses these values
#define SECTOR_SIZE 512
#define FAT_SIZE SECTOR_SIZE * 2

static uint8_t fat1[FAT_SIZE];
static uint8_t fat2[FAT_SIZE];

static uint16_t max_cluster_index = 0;

err_code fat_init(FILE* f) {
  size_t fat_offset = boot_fat_offset();
  fseek(f, fat_offset, SEEK_SET);
  int read_result = fread(fat1, FAT_SIZE, 1, f);
  if (read_result != 1) {
    return ERR_DISK_IO;
  }

  read_result = fread(fat2, FAT_SIZE, 1, f);
  if (read_result != 1) {
    return ERR_DISK_IO;
  }

  if (fat_test() != 0) {
      return ERR_FAT_DATA_INCORRECT;
  }

    max_cluster_index = boot_blocks_count() / 4 - 2;

  return ERR_OK;
}

bool fat_is_reserved(uint16_t value) {
  return ((value >= FAT_RSRVD) && (value < FAT_BAD));
}

uint16_t fat_get_element(uint16_t index) {
  if (index > max_cluster_index) return FAT_BAD;
  uint16_t offset = index * 3 / 2;
  uint16_t value = *( (uint16_t*) &fat1[offset] );
  if (index & 1) {
    return value >> 4;
  } else {
    return value & 0xFFF;
  }
}

void fat_set_element(uint16_t index, uint16_t value) {
    if (index > max_cluster_index) return;
    if (index < 2) return;
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
    size_t fat_offset = boot_fat_offset();
    fseek(f, fat_offset, SEEK_SET);
    int write_result = fwrite(fat1, FAT_SIZE, 1, f);
    if (write_result != 1) {
        return ERR_DISK_IO;
    }

    write_result = fwrite(fat2, FAT_SIZE, 1, f);
    if (write_result != 1) {
        return ERR_DISK_IO;
    }

    return ERR_OK;
}

uint16_t fat_find_free_cluster() {
    for (uint16_t i = 2; i <= max_cluster_index; i++) {
        if (fat_get_element(i) == 0) return i;
    }
    return 0;
}

size_t fat_get_free_space() {
    size_t free_cluster_count = 0;
    for (int i = 2; i <= max_cluster_index; i++) {
        if (fat1[i] == FAT_FREE) free_cluster_count++;
    }
    return free_cluster_count * boot_cluster_size();
}

size_t fat_get_file_size(uint16_t cluster) {
    size_t clusters_count = 0;
    while (cluster != FAT_EOF) {
        clusters_count++;
        cluster = fat_get_element(cluster);
    }
    return clusters_count * boot_cluster_size();
}

int fat_test() {
  if (memcmp(fat1, fat2, SECTOR_SIZE * 2) != 0) return 1;
  if (fat1[0] != 0xF9) return 1;
  if (fat1[1] != 0xFF) return 1;
  if (fat1[2] != 0xFF) return 1;

  return 0;
}