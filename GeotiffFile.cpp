//
// Created by zhengchuankun on 8/31/16.
//

#include "GeotiffFile.h"

void GeotiffFile::loadTiffFile(const char *filePath) {
    TIFF *tif = XTIFFOpen(filePath, "r");
    if (!tif) {
        perror("Open tif file failed.\n");
        exit(1);
    }

    TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &width);
    TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &height);
//    printf("%d %d\n", width, height);

    int samplePerPixel, bitsPerSample;
    TIFFGetField(tif, TIFFTAG_SAMPLESPERPIXEL, &samplePerPixel);
    TIFFGetField(tif, TIFFTAG_BITSPERSAMPLE, &bitsPerSample);
//    printf("%d %d\n", samplePerPixel, bitsPerSample);

    int stripSize = TIFFStripSize(tif);
    int stripNum = TIFFNumberOfStrips(tif);
//    printf("%d\n", stripSize);

    int bytePerPixel = samplePerPixel * bitsPerSample / 8;

    if (buffer != nullptr)
        free(buffer);
    if ((buffer = (unsigned short*)malloc(stripSize * stripNum * bytePerPixel)) == nullptr) {
        perror("Malloc buffer failed.\n");
        exit(1);
    }

    int offset = 0;
    for (int i = 0; i < stripNum; i++) {
        int result;
        if ((result = TIFFReadEncodedStrip(tif, i, buffer + offset, stripSize)) == -1) {
            perror("Read strip error.\n");
            exit(1);
        }
        offset += result;
    }

    tot = stripNum * stripSize;

    XTIFFClose(tif);

    if (heightMap != nullptr)
        free(heightMap);
    if ((heightMap = (float*)malloc(stripSize * stripNum * sizeof(float))) == nullptr) {
        perror("malloc heightMap failed.\n");
        exit(1);
    }

    int count = 0;
    freopen(testOutput, "w", stdout);
    for (int i = 0; i < tot; i++) {
        heightMap[i] = (float)buffer[i] / 6000.0;
        if (heightMap[i] > 0) {
//            count++;
            printf("%d %d : %f\n", i / 3601, i % 3601, heightMap[i]);
        }
    }
    freopen("CON", "w", stdout);
//    printf("%d\n", count);
}

GeotiffFile::GeotiffFile() {
    tot = 0;
    buffer = nullptr;
    heightMap = nullptr;
}

GeotiffFile::~GeotiffFile() {
    if (buffer) {
        free(buffer);
    }
    if (heightMap) {
        free(heightMap);
    }
}

int GeotiffFile::getWidth() const {
    return width;
}

int GeotiffFile::getHeight() const {
    return height;
}

float *GeotiffFile::getHeightMap() const {
    return heightMap;
}
