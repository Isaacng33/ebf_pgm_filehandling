#include <stdio.h>

#include <stdlib.h>

#include <string.h>

#include <math.h>

#include "imagedata.h"


unsigned char readBit(unsigned char inputValue, int whichBit) {
    unsigned char bitMask = 1 << whichBit;
    return (inputValue & bitMask) != 0;
}

unsigned char writeBit(unsigned char inputValue, int whichBit, int bitValue) {
    unsigned char bitMask = 1 << whichBit;
    unsigned char inverserMask = ~bitMask;
    inputValue &= inverserMask;

    unsigned char shiftedBit = bitValue << whichBit;
    inputValue |= shiftedBit;
    return inputValue;
}

int readBinary(Imagedata * imageData) {
    int current = 0;
    int current2 = 0;
    unsigned int buffer;
    fread( & buffer, 1, 1, imageData -> f);

    // read in each grey value from the file
    for (current = 0; current < imageData -> height; current++) {
        for (current2 = 0; current2 < imageData -> width; current2++) {
            // reading in
            int check = fread( & imageData -> image[current][current2], 1, 1, imageData -> f);
            // validate that we have captured 1 pixel value
            if (check == 0 || imageData -> image[current][current2] > 31 || imageData -> image[current][current2] < 0) { // check inputted data
                // ensure that allocated data is freed before exit.
                printf("ERROR: Bad Data (%s)\n", imageData -> filename);
                fclose(imageData -> f);
                free(imageData -> image);
                free(imageData);
                return 1;
            } // check inputted data
        } // reading in
    }
    return 0;
}

int writeBinary(Imagedata * imageData) {
    int current = 0;
    int current2 = 0;
    // writing out
    // iterate though the array and print out pixel values
    for (current = 0; current < imageData -> height; current++) {
        for (current2 = 0; current2 < imageData -> width; current2++) {
            int check = fwrite( & imageData -> image[current][current2], 1, 1, imageData -> f);
            if (check == 0) {
                // check write
                printf("ERROR: Bad Output (%s)\n", imageData -> filename);
                fclose(imageData -> f);
                free(imageData -> image);
                free(imageData);
                return 1;
            }
        }
    }
    return 0;
}

/*
 Reading in Compressed Binary where each value is packed into 5 bits
 Stores it in Imagedata as uncompressed 8 bit binary
*/

void readBinaryCompressed(Imagedata * imageData) {
    // reads in unwanted newline
    unsigned char buffer;
    fread( & buffer, 1, 1, imageData -> f);

    int bufferCount = 0;
    unsigned char uncompressedPoint;
    unsigned char point;

    // reads first compressed byte
    fread( & buffer, 1, 1, imageData -> f);
    for (int current = 0; current < imageData -> height; current++) {
        for (int current2 = 0; current2 < imageData -> width; current2++) {
            // loop to read in 5 bit
            for (int i = 0; i < 5; i++) {
                if (bufferCount == 8) {
                    // Read in a new byte
                    fread( & buffer, 1, 1, imageData -> f);
                    bufferCount = 0;
                }
                // read each wanted bit
                point = readBit(buffer, 7 - bufferCount);
                // update the uncompressed bit to wanted bit value
                uncompressedPoint = writeBit(uncompressedPoint, 4 - i, point);
                bufferCount++;
            }
            // store it in the 2d array
            imageData -> image[current][current2] = uncompressedPoint;
        }
    }
}

/*
 Reading in uncompressed Binary 
 Writes out compress binary where each value is packed into 5 bits
*/

void writeBinaryCompressed(Imagedata * imageData) {
    unsigned char buffer;
    unsigned char byte;
    int buffCount = 0;

    for (int current = 0; current < imageData -> height; current++) {
        for (int current2 = 0; current2 < imageData -> width; current2++) {
            // Store uncompressed into buffer
            buffer = imageData -> image[current][current2];
            // we need to write out 5 each time because 5 makes a point
            for (int i = 0; i < 5; i++) {
                if (buffCount == 8) {
                    // once we have a byte in buffer, write it out
                    fwrite( & byte, 1, 1, imageData -> f);
                    buffCount = 0;
                }
                // update each bit in the byte
                byte = writeBit(byte, 7 - buffCount, readBit(buffer, 4 - i));
                buffCount++;
            }
        }
    }
    // write out remaining bye
    fwrite( & byte, 1, 1, imageData -> f);
}


// function to compress ebc and write to .ebcc files
void writeBlockedCompressed(Imagedata * imageData) {
    // Algorithm
    // Since we have all values in 2d array we can loop thru 3x3 with +3 counter
    // have a variable to add up all the digits in the block and calculate average
    // write out the digit
    unsigned int average; // = sum divide counter
    unsigned char buffer;
    unsigned char byte;
    int buffCount = 0;
    int increment = 3;
    int increment2 = 3;

    // Iterating through the 2d array using 3x3 block
    for (int current = 0; current < imageData -> height; current+=increment) {
        for (int current2 = 0; current2 < imageData -> width; current2+=increment2) {
            int sum = 0; // to store the sum of 3x3 block
            int counter = 0; // counts how many grey value we have added
            
            // to calculate if it has a full block
            // adds 3 each iteration to check if it exeeds the dimensions
            // if it exceeds return remaining blocks
            int blockHeight = (current + 3 <= imageData -> height) ? 3 : imageData -> height - current;
            int blockWidth = (current2 + 3 <= imageData -> width) ? 3 : imageData -> width - current2;

            // To iterate within the correct 3x3 block 
            for (int row = current; row < current + blockHeight; row++) {
                for (int column = current2; column < current2 + blockWidth; column++) {
                    sum += imageData -> image[row][column];
                    counter++;
                }
            }
            average = round(sum / counter); // found averaged for that block
            
            // write average out in packed binary
            buffer = average;
            // we need to write out 5 each time because 5 makes a point
            for (int i = 0; i < 5; i++) {
                if (buffCount == 8) {
                    // once we have a byte in buffer, write it out
                    fwrite( & byte, 1, 1, imageData -> f);
                    buffCount = 0;
                }
                // update each bit in the byte
                byte = writeBit(byte, 7 - buffCount, readBit(buffer, 4 - i));
                buffCount++;
            }
            // error checking to check for remaining blocks smaller than 3x3
            if (imageData -> height - (current + 3) < 0) {
                increment = (current + 3) - imageData -> height;
            }
            if (imageData -> width - (current2 + 3) < 0) {
                increment2 = (current2 + 3) - imageData -> width;
            }
        }
    }
    // write out remaining bye
    fwrite( & byte, 1, 1, imageData -> f);
}

void readBlockCompressed(Imagedata * imageData,Imagedata * imageData2) {
    // Now that the data is stored in a 2d array form
    // Every grey value in this 2d array equals to a 3x3 block
    // We loop through once 
    int increment = 3;
    int increment2 = 3;
    unsigned char buffer;

    for (int current = 0; current < imageData -> height; current++) {
        for (int current2 = 0; current2 < imageData -> width; current2++) {
            buffer = imageData -> image[current][current2];
            // double for loop to get write out the 3x3 array at imagedata 2
            for (int row = 0; row < imageData2 -> height; row+=increment) {
                for (int column = 0; column < imageData2 -> width; column+=increment2) {
                    // to calculate if it has a full block
                    // adds 3 each iteration to check if it exeeds the dimensions
                    // if it exceeds return remaining blocks
                    int blockHeight = (row + 3 <= imageData -> height) ? 3 : imageData -> height - row;
                    int blockWidth = (column + 3 <= imageData -> width) ? 3 : imageData -> width - column;
                    // To iterate within the correct 3x3 block and store the average value to 3x3
                    for (int row2 = row; row2 < row + blockHeight; row2++) {
                        for (int column2 = column; column2 < column + blockWidth; column2++) {
                            imageData2 -> image[row2][column2] = buffer;
                        }
                    }
                    
                    // error checking to check for remaining blocks smaller than 3x3
                    if (imageData -> height - (row + 3) < 0) {
                        increment = (row + 3) - imageData -> height;
                    }
                    if (imageData -> width - (column + 3) < 0) {
                        increment2 = (column + 3) - imageData -> width;
                    }
                }
            }
        }
    }
}
