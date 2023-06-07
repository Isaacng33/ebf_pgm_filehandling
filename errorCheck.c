#include <stdlib.h>

#include <stdio.h>

#include <unistd.h>

#include "imagedata.h"

#define SUCCESS 0
#define BAD_ARGS 1
#define MAX_DIMENSION 262144
#define MIN_DIMENSION 1
#define MAGIC_NUMBER 0x6265
#define MAGIC_NUMBER_EBU 0x7565
#define MAGIC_NUMBER_EBC 0x6365

void freeImage(Imagedata * imageData) {
    imageData = NULL;
    free(imageData);
}

void freeAll(Imagedata * imageData) {
    * imageData -> image = NULL;
    free( * imageData -> image);
    imageData -> image = NULL;
    free(imageData -> image);
    freeImage(imageData);
}

int checkArgCount(int argCount, char ** argVar) {
    // validate that user has enter 2 arguments (plus the executable name)
    if (argCount == 1) { // check if arg is empty;
        printf("Usage: %s file1 file2\n", argVar[0] + 2);
        return SUCCESS; // SUCCESS = 0
    } else if (argCount != 3) { // check arg count
        printf("ERROR: Bad Arguments\n");
        return BAD_ARGS; // BAD ARGS = 1
    } // check arg count
    return -1; // if no error is found
}

// Check if File opened successfully
int checkFile(Imagedata * imageData) {
    if (imageData -> f == NULL) { // check file pointer
        printf("ERROR: Bad File Name (%s)\n", imageData -> filename);
        freeImage(imageData);
        return 1;
    }
    return 0;
}

// Magic mumber checking error
int checkMagicNum(Imagedata * imageData, unsigned short magic) {
    unsigned char magicNumber[2];
    // get first 2 characters which should be magic number
    magicNumber[0] = getc(imageData -> f);
    magicNumber[1] = getc(imageData -> f);
    // stores to struct
    imageData -> magicNumberValue = (unsigned short * ) magicNumber;
    // checking against the casted value due to endienness.
    if ( * imageData -> magicNumberValue != magic) {
        printf("ERROR: Bad Magic Number (%s)\n", imageData -> filename);
        fclose(imageData -> f);
        freeImage(imageData);
        return 1;
    }
    return 0;
}

// Dimensions checking error
int checkDimensions(Imagedata * imageData) { // check for the valid dimensions
    int check = fscanf(imageData -> f, "%d %d", & imageData -> height, & imageData -> width);
    if (check != 2 || imageData -> width < MIN_DIMENSION || imageData -> width > MAX_DIMENSION || imageData -> height < MIN_DIMENSION || imageData -> height > MAX_DIMENSION) {
        // print appropriate error message and return
        printf("ERROR: Bad Dimensions (%s)\n", imageData -> filename);
        fclose(imageData -> f);
        freeImage(imageData);
        return 1;
    } // check dimensions
    return 0;
}

// Bad Data Checking
int checkBadData(Imagedata * imageData) {
    // initialise variables used for loop
    int current = 0;
    int current2 = 0;
    // read in each grey value from the file
    for (current = 0; current < imageData -> height; current++) {
        for (current2 = 0; current2 < imageData -> width; current2++) {
            // reading in
            int check = fscanf(imageData -> f, "%u", & imageData -> image[current][current2]);
            // validate that we have captured 1 pixel value
            if (check != 1 || imageData -> image[current][current2] > 31 || imageData -> image[current][current2] < 0) { // check inputted data
                // ensure that allocated data is freed before exit.
                printf("ERROR: Bad Data (%s)\n", imageData -> filename);
                fclose(imageData -> f);
                freeAll(imageData);
                return 1;
            } // check inputted data
        } // reading in
    }
    // checking if it is EOF which will return value of -1
    if (fscanf(imageData -> f, "%u", & imageData -> image[current][current2 + 1]) != -1) {
        printf("ERROR: Bad Data (%s)\n", imageData -> filename);
        fclose(imageData -> f);
        freeAll(imageData);
        return 1;
    }
    return 0;
}

int checkBadRead(Imagedata * imageData) {
    // checking to theres permission to read
    if (access(imageData -> filename, R_OK)) {
        printf("ERROR: Bad File Name (%s)\n", imageData -> filename);
        fclose(imageData -> f);
        freeAll(imageData);
        return 1;
    }
    return 0;
}

int checkBadWrite(Imagedata * imageData) {
    // checking if theres permission to write
    if (access(imageData -> filename, W_OK)) {
        printf("ERROR: Bad File Name (%s)\n", imageData -> filename);
        fclose(imageData -> f);
        freeAll(imageData);
        return 1;
    }
    return 0;
}

int checkIdenticalMGV(Imagedata * imageData1, Imagedata * imageData2) {
    // compare the data from the two files:
    // start with magic number values
    if (( * imageData1 -> magicNumberValue) != ( * imageData2 -> magicNumberValue)) { // free and exit
        printf("DIFFERENT\n");
        fclose(imageData2 -> f);
        freeAll(imageData1);
        freeAll(imageData2);
        return 1;
    } // free and exit
    return 0;
}

int checkIdenticalDim(Imagedata * imageData1, Imagedata * imageData2) {
    // checking against the 2 objects 
    if (((imageData1 -> height) != (imageData2 -> height)) || ((imageData1 -> width) != (imageData2 -> width))) { // free and exit
        fclose(imageData2 -> f);
        freeAll(imageData1);
        freeAll(imageData2);
        printf("DIFFERENT\n");
        return 1;
    } // free and exit
    return 0;
}

int checkIdentical(Imagedata * imageData1, Imagedata * imageData2) {
    int n = 0;
    int n2 = 0;
    // Checking grey values between 2 files if it is identical
    for (n = 0; n < imageData1 -> height; n++) {
        for (n2 = 0; n2 < imageData1 -> width; n2++) {
            if (imageData1 -> image[n][n2] != imageData2 -> image[n][n2]) {
                fclose(imageData2 -> f);
                freeAll(imageData1);
                freeAll(imageData2);
                printf("DIFFERENT\n");
                return 1;
            } // free and exit
        }
    }
    return 0;
}

int writeHeader(Imagedata * imageData, char magic[3]) {
    // write the header data in one block
    int check = fprintf(imageData -> f, "%s\n%d %d\n", magic, imageData -> height, imageData -> width);
    // and use the return from fprintf to check that we wrote.
    if (check <= 0) { // check write
        printf("ERROR: Bad Data (%s)\n", imageData -> filename);
        fclose(imageData -> f);
        freeAll(imageData);
        return 1;
    } // check write
    return 0;
}

// writing imageData to a file
int writeImage(Imagedata * imageData) {
    int current = 0;
    int current2 = 0;
    int check = 0;
    // writing out
    // iterate though the array and print out pixel values
    for (current = 0; current < imageData -> height; current++) {
        for (current2 = 0; current2 < imageData -> width; current2++) {
            if (current + 1 == imageData -> height && current2 + 1 == imageData -> width) {
                check = fprintf(imageData -> f, "%u", imageData -> image[current][current2]);
            } else {
                //if we are at the end of a row ((current+1)%width == 0) then write a newline, otherwise a space.
                check = fprintf(imageData -> f, "%u%c", imageData -> image[current][current2], ((current2 + 1) % imageData -> width) ? ' ' : '\n');
            }
            if (check <= 0) {
                // check write
                printf("ERROR: Bad Output (%s)\n", imageData -> filename);
                fclose(imageData -> f);
                freeAll(imageData);
                return 1;
            }
        }
    }
    return 0;
}