#include <stdio.h>

#include <stdlib.h>

#include <unistd.h>

#include "make2dArr.h"

#include "errorCheck.h"

#include "imagedata.h"

#include "utilities.h"

#include "errorflow.h"


#define SUCCESS 0
#define BAD_ARGS 1
#define BAD_FILE 2
#define BAD_MAGIC_NUMBER 3
#define BAD_DIM 4
#define BAD_MALLOC 5
#define BAD_DATA 6
#define BAD_OUTPUT 7
#define MAGIC_NUMBER 0x6265
#define MAGIC_NUMBER_EBU 0x7565
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
    Imagedata * imageData = (Imagedata * ) malloc(sizeof(Imagedata));
    imageData -> f = fopen(argv[1], "rb");
    imageData -> filename = argv[1];

    // Check for errors with the file and its format
    int check = checkErrorNoArr(imageData, MAGIC_NUMBER_EBU);
    if (check == BAD_FILE) {
        return BAD_FILE;
    } else if (check == BAD_MAGIC_NUMBER) {
        return BAD_MAGIC_NUMBER;
    } else if (check == BAD_DIM) {
        return BAD_DIM;
    }

    // Allocate memory for the image array and check for malloc error
    imageData -> image = make2dArr(imageData -> height + 1, imageData -> width);
    check = checkMallocAndData(imageData, imageData, "eu", 0);
    if (check == BAD_MALLOC) {
        return BAD_MALLOC;
    } else if (check == BAD_DATA) {
        return BAD_DATA;
    }

    // Close the file
    fclose(imageData -> f);

    // open the output file in write mode;
    imageData -> f = fopen(argv[2], "w");
    imageData -> filename = argv[2];

    // Check for errors with the file and its format then write
    check = checkAndWrite(imageData, "eb", 0);
    if (check == BAD_FILE) {
        return BAD_FILE;
    } else if (check == BAD_DATA) {
        return BAD_DATA;
    } else if (check == BAD_OUTPUT) {
        return BAD_OUTPUT;
    }

    // close the output file before exit
    fclose(imageData -> f);
    // free allocated memory before exit
    freeAll(imageData);

    // print final success message and return
    printf("CONVERTED\n");
    return SUCCESS;
}