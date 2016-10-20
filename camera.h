#pragma once
#include <vector>
#include <GL/glew.h>
#include <glm/glm.hpp>
using namespace glm;

class Camera {
private:
	vec3 cameraPos;
	vec3 cameraDir;
	vec3 cameraUp;
	vec3 cameraHor;
public:
	Camera();
	~Camera();
};
