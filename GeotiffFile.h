//
// Created by zhengchuankun on 8/31/16.
//

#ifndef MYCLIPMAP_GEOTIFFFILE_H
#define MYCLIPMAP_GEOTIFFFILE_H

#include "global.h"

using namespace std;

class GeotiffFile {
private:
    unsigned short* buffer;
    int tot;
    int width, height;
    float* heightMap;
public:
    float *getHeightMap() const;
    int getHeight() const;
    int getWidth() const;
    GeotiffFile();
    virtual ~GeotiffFile();
    void loadTiffFile(const char* filePath);
};


#endif //MYCLIPMAP_GEOTIFFFILE_H
