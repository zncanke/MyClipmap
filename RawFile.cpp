#include "RawFile.h"

void RawFile::loadRawFile(const char* path) {
    FILE *fp = nullptr;
    fp = fopen(path, "rb");
    if (fp == nullptr) {
        perror("Cannot open raw file.\n");
        exit(1);
    }
    fread(rawData, 1, DATASIZE * DATASIZE, fp);
    int result = ferror(fp);
    if (result) {
        perror("Load file failed.\n");
        exit(1);
    }
    fclose(fp);
}

unsigned char RawFile::getRaw(int x, int z) {
    int tx = x % (DATASIZE + 1);
    int tz = z % (DATASIZE + 1);
    if (!rawData)
        return 0;
    return rawData[tx * DATASIZE + tz];
}

RawFile::RawFile() : flag(false) {
    rawData = nullptr;
    rawData = (unsigned char*)malloc(DATASIZE * DATASIZE);
    if (!rawData) {
        perror("Constructor failed.\n");
        exit(1);
    }
}

RawFile::~RawFile() {
    if (rawData)
        free(rawData);
    if (heightMap)
        free(heightMap);
}

void RawFile::generateHeightMap() {
    if (flag) return;
    flag = true;
    heightMap = nullptr;
    heightMap = (float*)malloc(DATASIZE * DATASIZE * sizeof(float));
    if (!heightMap) {
        perror("heightMap malloc failed.\n");
        exit(1);
    }
    for (int i = 0; i < DATASIZE * DATASIZE; i++) {
        heightMap[i] = (float)rawData[i] / 200.0f;
    }
}

float* RawFile::getHeightMap() {
    return heightMap;
}

unsigned char *RawFile::getRaw() {
    return rawData;
}


