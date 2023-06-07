#pragma once

#include <stdio.h>

// data structure for image data
typedef struct _imagedata {
    FILE * f;
    char * filename;
    unsigned short * magicNumberValue;
    int height;
    int width;
    unsigned int ** image;
}
Imagedata;