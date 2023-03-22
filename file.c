#include "file.h"
#include "fat.h"

#define CLUSTER_SIZE    (512 * 4)

static uint8_t io_buffer[CLUSTER_SIZE];
static size_t global_offset = 0;

err_code extract_file(const FileParams* params, FILE* image_file, const char* dest_file) {
    FILE* f_out = fopen(dest_file, "w");
    if (!f_out) {
        return ERR_DISK_IO;
    }
    size_t bytes_to_write = params->size;
    uint16_t cluster = params->first_cluster;
    size_t res;
    size_t chunk_size;

    while(bytes_to_write > 0) {
        fseek(image_file, (cluster + 1) * CLUSTER_SIZE + global_offset, SEEK_SET);
        chunk_size = (bytes_to_write > CLUSTER_SIZE) ? CLUSTER_SIZE : bytes_to_write;
        res = fread(io_buffer, chunk_size, 1, image_file);
        if (res != 1) {
            fclose(f_out);
            return ERR_DISK_IO;
        }
        res = fwrite(io_buffer, chunk_size, 1, f_out);
        if (res != 1) {
            fclose(f_out);
            return ERR_DISK_IO;
        }
        bytes_to_write -= chunk_size;
        cluster = get_fat_element(cluster);
        if ((cluster == FAT_EOF) && (bytes_to_write > 0)) {
            fclose(f_out);
            return ERR_DISK_IO;
        }
    }

    fclose(f_out);
    return ERR_OK;
}

int file_test(FILE* image_file) {
    FileParams params;
    if (find_file_by_name("DPRESS.OVL", &params) != ERR_OK) return 1;
    if (extract_file(&params, image_file, "DPRESS.OVL") != ERR_OK) return 1;
    return 0;
}