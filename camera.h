#pragma once
#include <vector>
#include <GL/glew.h>
#include <glm/glm.hpp>
using namespace glm;

class Camera {
private:
	vec3 pos;
	vec3 dir;
	vec3 up;
	vec3 horizontal;
	vec3 speed;
	float pitch;
	float yaw;
	float rotateCoe;
	bool modified;
public:
	Camera();
	~Camera();
	void setPos(float x, float y, float z);
	vec3 getPos() const;
	vec3 getUp() const;
	vec3 getHorizontal() const;
	vec3 getDir() const;
	void move(bool keys[], float deltaTime, int width, int height);
	void rotate(float xOff, float yOff);
	float getPitch() const;
	float getYaw() const;
	void revise();
};
