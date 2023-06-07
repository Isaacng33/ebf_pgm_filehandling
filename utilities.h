#include <stdio.h>

#include <stdlib.h>

#include <string.h>

#include <math.h>

#include "imagedata.h"

int readBinary(Imagedata * imageData);

int writeBinary(Imagedata * imageData);

void readBinaryCompressed(Imagedata * imageData);

void writeBinaryCompressed(Imagedata * imageData);

void writeBlockedCompressed(Imagedata * imageData);

void readBlockCompressed(Imagedata * imageData, Imagedata * imageData2);

unsigned char readBit(unsigned char inputValue, int whichBit);

unsigned char writeBit(unsigned char inputValue, int whichBit, int bitValue);