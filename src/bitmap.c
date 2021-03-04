/*
 * In The Name Of God
 * ========================================
 * [] File Name : bitmap.c
 *
 * [] Creation Date : 22-12-2015
 *
 * [] Created By : Parham Alvani (parham.alvani@gmail.com)
 * =======================================
*/
/*
 * Copyright (c) 2015 Parham Alvani.
*/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "bitmap.h"

void WRITE_BITMAP_IMAGE(struct BITMAP_IMAGE *image) {

    FILE* fp=fopen("sample.bmp","wb");

    //FILE HEADER
    fwrite(image->file_header, sizeof(struct BITMAP_FILE_HEADER), 1, fp);
    // END OF FILE HEADER

    //INFO HEADER
    fwrite(image->info_header, sizeof(struct BITMAP_INFO_HEADER), 1, fp);
    // END OF INFO HEADER

    //COLOR TABLE
    fwrite(image->color_table, sizeof(struct RGBQUAD),(image->info_header->image_width)*(image->info_header->image_height),fp);
    //END OF COLOR TABLE

    //PIXEL DATA
    fwrite(image->info_header->image_size, sizeof(image->info_header->image_size) ,1,fp);
    //END OF PIXEL DATA
}


//function to free memory

struct BITMAP_IMAGE *bitmap_image_new_from_fd(int fd) {
    struct BITMAP_IMAGE *image = malloc(sizeof(struct BITMAP_IMAGE));
    image->fd = fd;
    image->info_header = NULL;
    image->file_header = NULL;
    image->color_table = NULL;
    image->pixel_table = NULL;
    return image;
}

ssize_t bitmap_image_pixel_table(struct BITMAP_IMAGE *image) {
    lseek(image->fd, image->file_header->file_offset_to_pixel_array, SEEK_SET);

    int i;
    ssize_t retval = 0;

    image->pixel_table = malloc(
            image->info_header->image_width * sizeof(struct RGBAX *));
    for (i = 0; i < image->info_header->image_width; i++) {
        image->pixel_table[i] = malloc(
                image->info_header->image_height *
                sizeof(struct RGBAX));
    }

    return retval;
}

ssize_t bitmap_image_color_table(struct BITMAP_IMAGE *image) {

    lseek(image->fd, (sizeof(struct BITMAP_FILE_HEADER) + sizeof(struct BITMAP_INFO_HEADER)), SEEK_SET);

    image->color_table = malloc(
            sizeof(struct BITMAP_IMAGE) * (image->info_header->image_height) * (image->info_header->image_width));

    ssize_t retval;

    retval = read(image->fd, image->color_table,
                  sizeof(struct BITMAP_IMAGE) * (image->info_header->image_height) * (image->info_header->image_width));

    if (retval < sizeof(struct RGBQUAD)) {

        return retval;

    }

}


ssize_t bitmap_image_header(struct BITMAP_IMAGE *image) {
    lseek(image->fd, 0, SEEK_SET);

    ssize_t retval;

    image->file_header = malloc(sizeof(struct BITMAP_FILE_HEADER));
    retval = read(image->fd, image->file_header,
                  sizeof(struct BITMAP_FILE_HEADER));
    if (retval < sizeof(struct BITMAP_FILE_HEADER))
        return retval;

    image->info_header = malloc(sizeof(struct BITMAP_INFO_HEADER));
    retval += read(image->fd, image->info_header,
                   sizeof(struct BITMAP_INFO_HEADER));
    if (retval - sizeof(struct BITMAP_FILE_HEADER) <
        sizeof(struct BITMAP_INFO_HEADER))
        return retval;

    return retval;
}


