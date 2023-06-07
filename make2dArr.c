#include <stdlib.h>

#include <stdio.h>

#include "imagedata.h"

// 2d Array maker
unsigned int ** make2dArr(int height, int width) {
    unsigned int ** wholeArray = (unsigned int ** ) malloc(height * sizeof(unsigned int * ));
    unsigned int * dataBlock = (unsigned int * ) malloc(height * width * sizeof(unsigned int));
    int current = 0;
    for (current = 0; current < height; current++) {
        wholeArray[current] = dataBlock + current * (width);
    }
    return wholeArray;
};

// 2d Array Checker
int arrCheck(Imagedata * imageData) {
    // if malloc is unsuccessful, it will return a null pointer
    if ( * imageData -> image == NULL) {
        printf("ERROR: Image Malloc Failed\n");
        fclose(imageData -> f);
        free(imageData);
        return 1;
    }
    return 0;
}