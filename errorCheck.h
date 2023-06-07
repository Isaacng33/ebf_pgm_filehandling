#pragma once

#include <stdlib.h>

#include <stdio.h>

#include <unistd.h>

#include "imagedata.h"

void freeAll(Imagedata * imageData);

void freeImage(Imagedata * imageData);

int checkArgCount(int argCount, char ** argVar);

int checkMagicNum(Imagedata * imageData, unsigned short magic);

int checkDimensions(Imagedata * imageData);

int checkFile(Imagedata * imageData);

int checkBadData(Imagedata * imageData);

int checkIdenticalMGV(Imagedata * imageData1, Imagedata * imageData2);

int checkIdenticalDim(Imagedata * imageData1, Imagedata * imageData2);

int checkIdentical(Imagedata * imageData1, Imagedata * imageData2);

int writeImage(Imagedata * imageData);

int writeHeader(Imagedata * imageData, char magic[3]);

int checkBadRead(Imagedata * imageData);

int checkBadWrite(Imagedata * imageData);