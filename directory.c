#include "directory.h"
#include "boot.h"
#include <string.h>
#include <stdio.h>
#include "types.h"

#define DIR_SIZE 		512 * 7
#define MAX_FILES_COUNT 112
#define FILE_DELETED 	0xE5

#pragma pack(1)
typedef struct DirectoryEntry {
	char filename[8];
	char extension[3];
	uint8_t attributes;
	char reserved[8];
    uint8_t dir_number;
    uint8_t parent_dir;
	uint16_t address;
	uint16_t date;
	uint16_t first_cluster;
	uint32_t file_size;
} __attribute__((packed)) DirectoryEntry;

static DirectoryEntry directory[MAX_FILES_COUNT];

static void copy_file_name(const DirectoryEntry* entry, char* dest) {
	int i;
	for (i = 0; i < 8; i++) {
		if (entry->filename[i] == ' ') break;
		if (entry->filename[i] == 0) break;
		dest[i] = entry->filename[i];
	}	
	if ((entry->extension[0] == ' ') || (entry->extension[0] == 0)) {
		dest[i] = 0;
		return;
	}
	dest[i++] = '.';
	for (int j = 0; j < 3; j++) {
		if (entry->extension[j] == ' ') break;
		if (entry->extension[j] == 0) break;		
		dest[i++] = entry->extension[j];
	}
	dest[i] = 0;
}

err_code dir_init(FILE* f) {
    size_t dir_offset = boot_dir_offset();
    fseek(f, dir_offset, SEEK_SET);
	int read_result = fread(&directory, DIR_SIZE, 1, f);
	if (read_result != 1) {
	    return ERR_DISK_IO;
	}
	return ERR_OK;
}

static bool is_directory(const DirectoryEntry* params) {
    return (params->file_size == 0) && (params->address == 0) && (params->first_cluster == 0);
}

void dir_list(uint8_t dir, OnNextFile on_next_file) {
	for (int i = 0; i < MAX_FILES_COUNT; i++) {
		if (directory[i].filename[0] == 0) continue;
		if ((uint8_t)directory[i].filename[0] == FILE_DELETED) continue;
        if (directory[i].parent_dir != dir) continue;
		on_next_file(directory[i].filename, directory[i].extension, directory[i].address, directory[i].file_size, is_directory(&directory[i]));
	}
}

err_code dir_file_by_name(const char* filename, FileParams* params) {
	char buf[16];
	for (int i = 0; i < MAX_FILES_COUNT; i++) {
        copy_file_name(&directory[i], buf);
        if (strcmp(filename, buf) == 0) {
            strcpy(params->filename, buf);
            params->address = directory[i].address;
            params->first_cluster = directory[i].first_cluster;
            params->size = directory[i].file_size;
            params->index = i;
            return ERR_OK;
        }
	}
	return ERR_NOT_FOUND;
}

err_code dir_create_file(const char* filename, size_t file_size) {

}

int dir_test() {
    FileParams params;
    dir_file_by_name("DPRESS.OVL", &params);
	printf("%s %x %zu\n", params.filename, params.address, params.size);
	return 0;
}

