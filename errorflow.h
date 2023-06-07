#include <stdlib.h>

#include <stdio.h>

#include <unistd.h>

#include <string.h>

#include "imagedata.h"

#include "errorCheck.h"

#include "make2dArr.h"

#include "utilities.h"


int checkErrorNoArr(Imagedata * imageData, unsigned short magic);

int checkMallocAndData(Imagedata * imageData, Imagedata * imageData2, char magic[3], int compressed);

int checkAndWrite(Imagedata * imageData, char magic[3], int compressed);

int checkIdenticalAll(Imagedata * imageData1, Imagedata * imageData2);