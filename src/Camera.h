#ifndef CAMERA_H
#define CAMERA_H

#include "Utils.h"

class Camera {
public:
    // Camera state
    float x, y, z;
    float yaw, pitch;
    float moveSpeed;
    float mouseSensitivity;

    // Constructor with standard human eye-height defaults
    Camera(float startX = 0.0f, float startY = 1.7f, float startZ = 5.0f);

    // Input processing methods
    void processKeyboard(GLFWwindow* window, float deltaTime);
    void processMouse(float xoffset, float yoffset);

    // Generates and applies the view matrix via gluLookAt
    void applyViewMatrix();
};

#endif // CAMERA_H