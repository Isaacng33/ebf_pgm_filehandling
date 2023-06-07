#include <stdio.h>

#include <stdlib.h>

#include "make2dArr.h"

#include "errorCheck.h"

#include "imagedata.h"

#include "errorflow.h"

#include <unistd.h>


#define SUCCESS 0
#define BAD_ARGS 1
#define BAD_FILE 2
#define BAD_MAGIC_NUMBER 3
#define BAD_DIM 4
#define BAD_MALLOC 5
#define BAD_DATA 6
#define BAD_OUTPUT 7
#define MAGIC_NUMBER 0x6265
#define MAX_DIMENSION 262144
#define MIN_DIMENSION 1

int main(int argc, char ** argv) { // main
    // Check if the correct number of arguments have been provided
    int checkArg = checkArgCount(argc, argv);
    if (checkArg == 0) {
        // If checkArgCount returns 0, there are no errors and the program can exit successfully
        return SUCCESS;
    } else if (checkArg == 1) {
        // If checkArgCount returns 1, the argument count is incorrect and the program should exit with an error code
        return BAD_ARGS;
    }

    // allocating memory for imagedata struct
    Imagedata * imageData1 = (Imagedata * ) malloc(sizeof(Imagedata));
    imageData1 -> f = fopen(argv[1], "r");
    imageData1 -> filename = argv[1];

    // Check for errors with the file and its format
    int check = checkErrorNoArr(imageData1, MAGIC_NUMBER);
    if (check == BAD_FILE) {
        return BAD_FILE;
    } else if (check == BAD_MAGIC_NUMBER) {
        return BAD_MAGIC_NUMBER;
    } else if (check == BAD_DIM) {
        return BAD_DIM;
    }

    // Allocate memory for the image array and check for malloc error
    imageData1 -> image = make2dArr(imageData1 -> height + 1, imageData1 -> width);
    check = checkMallocAndData(imageData1, imageData1, "eb", 0);
    if (check == BAD_MALLOC) {
        return BAD_MALLOC;
    } else if (check == BAD_DATA) {
        return BAD_DATA;
    }

    // Close the file
    fclose(imageData1 -> f);

    // allocating memory for imagedata struct
    Imagedata * imageData2 = (Imagedata * ) malloc(sizeof(Imagedata));
    imageData2 -> f = fopen(argv[2], "r");
    imageData2 -> filename = argv[2];

    // Check for errors with the file and its format
    check = checkErrorNoArr(imageData2, MAGIC_NUMBER);
    if (check == BAD_FILE) {
        return BAD_FILE;
    } else if (check == BAD_MAGIC_NUMBER) {
        return BAD_MAGIC_NUMBER;
    } else if (check == BAD_DIM) {
        return BAD_DIM;
    }

    // Allocate memory for the image array and check for malloc error
    imageData2 -> image = make2dArr(imageData2 -> height + 1, imageData2 -> width);
    check = checkMallocAndData(imageData2, imageData2, "eb", 0);
    if (check == BAD_MALLOC) {
        return BAD_MALLOC;
    } else if (check == BAD_DATA) {
        return BAD_DATA;
    }

    // Check if the two images are identical
    check = checkIdenticalAll(imageData1, imageData2);
    if (check == SUCCESS) {
        return SUCCESS;
    }

    fclose(imageData2 -> f);
    // free allocated memory before exit
    freeAll(imageData1);
    freeAll(imageData2);

    printf("IDENTICAL\n");
    return SUCCESS;
}