//
// Created by zhengchuankun on 8/31/16.
//

#include "geotiffFile.h"

void GeotiffFile::loadTiffFile(const char *filePath) {
    TIFF *tif = XTIFFOpen(filePath, "r");
    if (!tif) {
        perror("Open tif file failed.\n");
        exit(1);
    }

    TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &width);
    TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &height);
//    printf("%d %d\n", width, height);

    uint16 samplePerPixel, bitsPerSample;
    TIFFGetField(tif, TIFFTAG_SAMPLESPERPIXEL, &samplePerPixel);
    TIFFGetField(tif, TIFFTAG_BITSPERSAMPLE, &bitsPerSample);
//    printf("%d %d\n", samplePerPixel, bitsPerSample);

    int stripSize = TIFFStripSize(tif);
    int stripNum = TIFFNumberOfStrips(tif);
//    printf("%d\n", stripSize);

    int bytePerPixel = samplePerPixel * bitsPerSample / 8;

	//printf("%d\n", bytePerPixel);

    if (buffer != nullptr)
        free(buffer);
    if ((buffer = (unsigned short*)malloc(width * height * bytePerPixel)) == nullptr) {
        perror("Malloc buffer failed.\n");
        exit(1);
    }

    int offset = 0;
    for (int i = 0; i < stripNum; i++) {
        int result;
        if ((result = TIFFReadEncodedStrip(tif, i, (char*)buffer + offset, stripSize)) == -1) {
            perror("Read strip error.\n");
            exit(1);
        }
        offset += result;
    }

	tot = width * height;

    XTIFFClose(tif);

    if (heightMap != nullptr)
        free(heightMap);
    if ((heightMap = (float*)malloc(width * height * sizeof(float))) == nullptr) {
        perror("malloc heightMap failed.\n");
        exit(1);
    }

    int count = 0;

 //   freopen(testOutput, "w", stdout);
	//for (int i = 0; i < height; i++) {
	//	for (int j = 0; j < width; j++) {
	//		printf("%6.d", buffer[i*width + j]);
	//	}
	//	printf("\n");
	//}
	//freopen("CON", "w", stdout);
    for (int i = 0; i < tot; i++) {
		//printf("%d\n", buffer[i]);
        //heightMap[i] = (float)buffer[i] / 6000.0;
		heightMap[i] = (float)buffer[i] / 10.0;
		maxheight = maxheight < heightMap[i] ? heightMap[i] : maxheight;
        /*if (heightMap[i] > 1) {
            count++;
            printf("%d %d : %f\n", i / 3601, i % 3601, heightMap[i]);
        }*/
    }
    //printf("%d\n", count);
}

GeotiffFile::GeotiffFile() {
    tot = 0;
	maxheight = 0;
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

float GeotiffFile::getMaxheight() const {
	return maxheight;
}

int GeotiffFile::getHeight() const {
    return height;
}

float *GeotiffFile::getHeightMap() const {
    return heightMap;
}
