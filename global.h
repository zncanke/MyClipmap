#pragma once
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <tiff.h>
#include <geotiff.h>
#include <tiffio.h>
#include <geotiffio.h>
#include <xtiffio.h>

#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <cstring>

#ifdef WIN32
#define vertexShaderPath "vertex.txt"
#define fragmentShaderPath "fragment.txt"
#define rawFilePath "data.raw"
#define mypointsPath "mypoints.txt"
#define tiffFilePath "ASTGTM2_N00E006_dem.tif"
#define testOutput "testOutput.txt"
#else
#define vertexShaderPath "/Users/willl/MyClipmap/vertex.txt"
#define fragmentShaderPath "/Users/willl/MyClipmap/fragment.txt"
#define rawFilePath "/Users/willl/MyClipmap/data.raw"
#define mypointsPath "/Users/willl/MyClipmap/mypoints.txt"
#define tiffFilePath "/Users/willl/MyClipmap/ASTGTM2_N00E006_dem.tif"
#define testOutput "/Users/willl/MyClipmap/testOutput.txt"
#endif