#include "camera.h"
#include <GLFW/glfw3.h>

Camera::Camera() {
    pos.x = pos.y = pos.z = 0;
    dir.x = dir.y = 0;
    dir.z = -1;
    up.x = up.z = 0;
    up.y = 1;
    horizontal = normalize(cross(dir, up));
	speed.x = speed.z = 10;
	speed.y = 0;
	yaw = 0;
	pitch = 0;
	rotateCoe = 0.25;
	modified = false;
}

Camera::~Camera() {}

void Camera::setPos(float x, float y, float z) {
	pos.x = x;
	pos.y = y;
	pos.z = z;
}

vec3 Camera::getPos() const {
	return vec3(pos);
}

vec3 Camera::getUp() const {
	return vec3(up);
}

vec3 Camera::getHorizontal() const {
	return vec3(horizontal);
}

vec3 Camera::getDir() const {
	return vec3(dir);
}

void Camera::move(bool keys[], float deltaTime, int width, int height) {
	vec3 nPos(pos);
	speed *= deltaTime;
	if (keys[GLFW_KEY_W])
		pos += speed * dir;
	if (keys[GLFW_KEY_S])
		pos -= speed * dir;
	if (keys[GLFW_KEY_A])
		pos -= speed * horizontal;
	if (keys[GLFW_KEY_D])
		pos += speed * horizontal;
	if (keys[GLFW_KEY_SPACE])
		pos.y += speed.x;
	if (keys[GLFW_KEY_C])
		pos.y -= speed.x;
	speed /= deltaTime;
	if (pos.x > width || pos.x < 0 || pos.z > height || pos.z < 0)
		pos = nPos;
}

void Camera::rotate(float xpos, float ypos) {
	yaw = xpos;
	pitch -= ypos;
	modified = true;
}

float Camera::getPitch() const {
	return pitch;
}

float Camera::getYaw() const {
	return yaw;
}

void Camera::revise() {
	if (modified) {
		yaw /= 4;
		pitch = (pitch > 4 * 89) ? 4 * 89 : pitch;
		pitch = (pitch < -4 * 89) ? -4 * 89 : pitch;
		pitch /= 4;
		modified = false;

		yaw += 90;

		dir.x = cos(radians(pitch)) * cos(radians(yaw));
		dir.y = sin(radians(pitch));
		dir.z = cos(radians(pitch)) * sin(radians(yaw));

		up.x = -sin(radians(pitch)) * cos(radians(yaw));
		up.y = cos(radians(pitch));
		up.z = -sin(radians(pitch)) * sin(radians(yaw));

		horizontal = normalize(cross(dir, up));
		pitch *= 4;
	}
}




