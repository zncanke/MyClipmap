#include "RawFile.h"

void RawFile::loadRawFile(const char* path) {
    FILE *fp = nullptr;
    fp = fopen(path, "rb");
    if (fp == nullptr) {
        perror("Cannot open raw file.\n");
        exit(1);
    }
    fread(heightMap, 1, DATASIZE * DATASIZE, fp);
    int result = ferror(fp);
    if (result) {
        perror("Load file failed.\n");
        exit(1);
    }
    fclose(fp);
}

unsigned char RawFile::getHeight(int x, int z) {
    int tx = x % (DATASIZE + 1);
    int tz = z % (DATASIZE + 1);
    if (!heightMap)
        return 0;
    return heightMap[tx * DATASIZE + tz];
}

RawFile::RawFile() {
    heightMap = nullptr;
    heightMap = (unsigned char*)malloc(DATASIZE * DATASIZE);
    if (!heightMap) {
        perror("Constructor failed.\n");
        exit(1);
    }
}

RawFile::~RawFile() {
    free(heightMap);
}

