#ifndef pbm_rw_pbm_h
#define pbm_rw_pbm_h

#include <stdlib.h>
#include <stdio.h>
#include <string.h> //for memcpy

struct image_header{
    char format[3]; //Image format, example: P5
    int rows;       //Image height
    int cols;       //Image width
    int levels;     //Number of gray/each color levels
};

char *readImage(const char *file_name, unsigned long *length, struct image_header *ih);
void writeImage(const char *file_name, const char *image_data, const struct image_header ih);

void *d_Sampling_img(const char *image_data, int rows, int cols);
void *u_Sampling_img(const char *image_data, int rows, int cols);


#endif
