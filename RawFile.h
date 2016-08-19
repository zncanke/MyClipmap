#ifndef MYCLIPMAP_RAWFILE_H
#define MYCLIPMAP_RAWFILE_H

#include <cstdio>
#include <cstdlib>
#include <string>
#include <iostream>
using namespace std;

const int SIZE = 513;

class RawFile {
private:
    unsigned char *heightMap;
public:
    RawFile();
    virtual ~RawFile();
    void loadRawFile(const string path);
    unsigned char getHeight(int x, int z);
};

#endif //MYCLIPMAP_RAWFILE_H
