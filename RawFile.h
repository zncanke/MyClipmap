#ifndef MYCLIPMAP_RAWFILE_H
#define MYCLIPMAP_RAWFILE_H

#include "global.h"
using namespace std;

const int DATASIZE = 513;

class RawFile {
private:
    unsigned char *heightMap;
public:
    RawFile();
    virtual ~RawFile();
    void loadRawFile(const char* path);
    unsigned char getHeight(int x, int z);
};

#endif //MYCLIPMAP_RAWFILE_H
