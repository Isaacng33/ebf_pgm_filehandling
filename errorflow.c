#include <stdlib.h>

#include <stdio.h>

#include <unistd.h>

#include <string.h>

#include "imagedata.h"

#include "errorCheck.h"

#include "make2dArr.h"

#include "utilities.h"

#define SUCCESS 0
#define BAD_ARGS 1
#define BAD_FILE 2
#define BAD_MAGIC_NUMBER 3
#define BAD_DIM 4
#define BAD_MALLOC 5
#define BAD_DATA 6
#define BAD_OUTPUT 7
#define MAX_DIMENSION 262144
#define MIN_DIMENSION 1

int checkErrorNoArr(Imagedata * imageData, unsigned short magic) {
    // check file opened successfully
    if (checkFile(imageData) == 1 || checkBadRead(imageData) == 1) {
        return BAD_FILE;
    }

    // check magic number
    if (checkMagicNum(imageData, magic) == 1) {
        return BAD_MAGIC_NUMBER;
    }

    // Checking to make sure dimension is valid
    if (checkDimensions(imageData) == 1) {
        return BAD_DIM;
    }
    return 100;
}

int checkMallocAndData(Imagedata * imageData, Imagedata * imageData2, char magic[3], int compressed) {
    if (arrCheck(imageData) == 1) {
        return BAD_MALLOC;
    }

    // checking for invalid grey value
    if (strcmp(magic, "eb") == 0) {
        if (checkBadData(imageData) == 1) {
            return BAD_DATA;
        }
    } else if (strcmp(magic, "eu") == 0) {
        if (readBinary(imageData) == 1) {
            return BAD_DATA;
        }
    } else if (strcmp(magic, "ec") == 0) {
        if (compressed == 0) { // normal ebc read data
            readBinaryCompressed(imageData);
        } else if (compressed == 1) { // reading ebcc data
            readBinaryCompressed(imageData);
            readBlockCompressed(imageData, imageData2);
        }
    }
    return 100;
}

int checkAndWrite(Imagedata * imageData, char magic[3], int compressed) { // 1 for ebc compressed, 0 for other file
    // validate that the file has been opened correctly
    if ((checkFile(imageData) == 1 || checkBadWrite(imageData) == 1)) { // validate output files
        return BAD_FILE;
    }

    // writing header to output file
    if (compressed == 1) {
        if (writeHeader(imageData, "EC") == 1) {
            return BAD_DATA;
        }
    } else {
        if (writeHeader(imageData, magic) == 1) {
            return BAD_DATA;
        }
    }

    // check for file types
    if (strcmp(magic, "eb") == 0) {
        if (writeImage(imageData) == 1) {
            return BAD_OUTPUT;
        }
    } else if (strcmp(magic, "eu") == 0) {
        if (writeBinary(imageData) == 1) {
            return BAD_OUTPUT;
        }
    } else if (strcmp(magic, "ec") == 0) {
        if (compressed == 0) { // if 0 means normal ebc
            writeBinaryCompressed(imageData);
        } else if (compressed == 1) {
            writeBlockedCompressed(imageData);
        }
       
    }
    // writing image data to output file
    return 100;
}

int checkIdenticalAll(Imagedata * imageData1, Imagedata * imageData2) {
    if (checkIdenticalMGV(imageData1, imageData2) == 0) {
        if (checkIdenticalDim(imageData1, imageData2) == 0) {
            if (checkIdentical(imageData1, imageData2) == 0) {
                return 1;
            }
        }
    }
    return SUCCESS;
}