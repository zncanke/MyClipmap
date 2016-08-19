#include "RawFile.h"

void RawFile::loadRawFile(const string path) {
    FILE *fp = nullptr;
    fp = fopen(path.c_str(), "rb");
    if (fp == nullptr) {
        perror("Cannot open raw file.\n");
        exit(1);
    }
    fread(heightMap, 1, SIZE*SIZE, fp);
    int result = ferror(fp);
    if (result) {
        perror("Load file failed.\n");
        exit(1);
    }
    fclose(fp);
}

unsigned char RawFile::getHeight(int x, int z) {
    int tx = x % (SIZE + 1);
    int tz = z % (SIZE + 1);
    if (!heightMap)
        return 0;
    return heightMap[tx * SIZE + tz];
}

RawFile::RawFile() {
    heightMap = nullptr;
    heightMap = (unsigned char*)malloc(SIZE * SIZE);
    if (!heightMap) {
        perror("Constructor failed.\n");
        exit(1);
    }
}

RawFile::~RawFile() {
    free(heightMap);
}

