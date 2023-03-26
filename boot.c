#include "boot.h"
#include <string.h>
#include "test.h"

#pragma pack(1)
typedef struct BootSector {
	uint16_t 	boot_sign;			    // 0xA0
	uint16_t 	jump_to_code;		    // 0x11E
	char 		os_sign[7];		        // ANDOS
	uint16_t 	bytes_per_block;	    // 0x200
	uint8_t 	blocks_per_cluster;     // 4
	uint16_t 	bootloader_size; 	    // 1 blocks
	uint8_t		fat_tables_count;	    // 2
	uint16_t	max_files_in_dir;	    // 0x70
	uint16_t	blocks_count;		    // 0x708
	uint8_t		media_descriptor;	    // 0xF9
	uint16_t	blocks_per_fat;		    // 2
	uint16_t	sectors_per_track;	    // 0x0A
	uint16_t	heads_count;		    // 2
	char		reserved[15];		    // 0
	char		volume_label[11];	    // ANDOS V3.30
	char		os_label[8];		    // FAT12
	uint16_t	reserved2;				// 0
} __attribute__((packed)) BootSector;

static BootSector boot_sector;

static size_t image_offset = 0;

err_code boot_init(FILE* f, size_t offset) {
  image_offset = offset;
  fseek(f, offset, SEEK_SET);
  int read_result = fread(&boot_sector, sizeof(boot_sector), 1, f);
  if (read_result != 1) {
  	return ERR_DISK_IO;
  }
  return ERR_OK;	
}

uint16_t boot_blocks_count() {
	return boot_sector.blocks_count;
}

size_t boot_fat_offset() {
	return image_offset + boot_sector.bytes_per_block * boot_sector.bootloader_size;
}

size_t boot_dir_offset() {
	return boot_fat_offset() + boot_sector.blocks_per_fat * boot_sector.bytes_per_block * boot_sector.fat_tables_count;
}

size_t boot_image_offset() {
    return image_offset;
}

int boot_test() {
	ASSERT_EQUALS(boot_sector.boot_sign, 0xA0);
	ASSERT_EQUALS(boot_sector.bytes_per_block, 0x200);
	ASSERT_EQUALS(boot_sector.sectors_per_track, 0x0A);
	if (strcmp(boot_sector.os_label, "FAT12   ") != 0) return 1;
	return 0;
}

