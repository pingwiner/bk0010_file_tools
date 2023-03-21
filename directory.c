#include "directory.h"


#define DIR_SIZE 		512 * 7
#define MAX_FILES_COUNT 112
#define FILE_DELETED 	0xE5

#pragma pack(1)
typedef struct DirectoryEntry {
	char filename[8];
	char extension[3];
	uint8_t attributes;
	char reserved[10];
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

err_code read_directory(FILE* f) {
	int read_result = fread(&directory, DIR_SIZE, 1, f);
	if (read_result != 1) {
	    return ERR_DISK_IO;
	}
	return ERR_OK;
}

void list_directory(OnNextFile on_next_file) {
	for (int i = 0; i < MAX_FILES_COUNT; i++) {
		if (directory[i].filename[0] == 0) continue;
		if ((uint8_t)directory[i].filename[0] == FILE_DELETED) continue;
		on_next_file(directory[i].filename, directory[i].extension, directory[i].address, directory[i].file_size);
	}
}

err_code find_file_by_name(const char* filename, FileParams* params) {
	char buf[16];
	for (int i = 0; i < MAX_FILES_COUNT; i++) {

	}
	return ERR_OK;
}

int directory_test() {
	char buf[16];
	copy_file_name(&directory[1], buf);
	printf("%s\n", buf);
	return 0;
}

