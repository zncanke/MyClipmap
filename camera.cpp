#include "camera.h"

Camera::Camera() {
    cameraPos.x = cameraPos.y = cameraPos.z = 0;
    cameraDir.x = cameraDir.y = 0;
    cameraDir.z = -1;
    cameraUp.x = cameraUp.z = 0;
    cameraUp.y = 1;
    cameraHor = normalize(cross(cameraDir, cameraUp));
}

Camera::~Camera() {}




