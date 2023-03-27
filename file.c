#include "file.h"
#include "fat.h"
#include "types.h"
#include "boot.h"

#define CLUSTER_SIZE    (512 * 4)

static uint8_t io_buffer[CLUSTER_SIZE];

err_code file_extract(FILE* image_file, const FileParams* params, const char* dest_file) {
    FILE* f_out = fopen(dest_file, "w");
    if (!f_out) {
        return ERR_DISK_IO;
    }
    size_t bytes_to_write = params->size;
    uint16_t cluster = params->first_cluster;
    size_t res;
    size_t chunk_size;
    size_t offset = boot_image_offset();

    while(bytes_to_write > 0) {
        fseek(image_file, (cluster + 1) * CLUSTER_SIZE + offset, SEEK_SET);
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
        cluster = fat_get_element(cluster);
        if ((cluster == FAT_EOF) && (bytes_to_write > 0)) {
            fclose(f_out);
            return ERR_DISK_IO;
        }
    }

    fclose(f_out);
    return ERR_OK;
}

err_code file_delete(FILE* image_file, const FileParams* params) {
    uint16_t cluster = params->first_cluster;
    uint16_t next_cluster;

    while(true) {
        next_cluster = fat_get_element(cluster);
        fat_set_element(cluster, 0);
        if (next_cluster == FAT_EOF) break;
        cluster = next_cluster;
    }

    return fat_sync(image_file);
}


//TODO:
err_code file_add(FILE* image_file, const char* src_file) {
    uint8_t io_buf[CLUSTER_SIZE];
    uint16_t cluster = fat_find_free_cluster();
    uint16_t chunk_size;
    size_t res;
    size_t offset = boot_image_offset();

    FILE* f_in = fopen(src_file, "r");
    fseek(f_in, 0L, SEEK_END);
    size_t bytes_to_write = ftell(f_in);
    fseek(f_in, 0L, SEEK_SET);

    while (bytes_to_write > 0) {
        cluster = fat_find_free_cluster();
        if (cluster == 0) return ERR_DISK_FULL;
        chunk_size = (bytes_to_write > CLUSTER_SIZE) ? CLUSTER_SIZE : bytes_to_write;
        res = fread(io_buf, chunk_size, 1, f_in);
        if (res != 1) {
            fclose(f_in);
            return ERR_DISK_IO;
        }
        fseek(image_file, (cluster + 1) * CLUSTER_SIZE + offset, SEEK_SET);
        res = fwrite(io_buf, chunk_size, 1, image_file);
        if (res != 1) {
            fclose(f_in);
            return ERR_DISK_IO;
        }

    }
    fclose(f_in);
    return ERR_OK;
}

int file_test(FILE* image_file) {
    FileParams params;
    if (dir_file_by_name("MLIST.TXT", &params) != ERR_OK) return 1;
    if (file_extract(image_file, &params, "MLIST.TXT") != ERR_OK) return 1;
    return 0;
}