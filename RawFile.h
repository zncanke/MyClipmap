#ifndef MYCLIPMAP_RAWFILE_H
#define MYCLIPMAP_RAWFILE_H

#include "global.h"
using namespace std;

const int DATASIZE = 513;

class RawFile {
private:
    unsigned char *rawData;
    float *heightMap;
    bool flag;
public:
    RawFile();
    virtual ~RawFile();
    void loadRawFile(const char* path);
    unsigned char getRaw(int x, int z);
    void generateHeightMap();
    float* getHeightMap();
    unsigned char* getRaw();
};

#endif //MYCLIPMAP_RAWFILE_H
