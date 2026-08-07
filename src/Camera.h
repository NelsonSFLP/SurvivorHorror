#ifndef CAMERA_H
#define CAMERA_H

#include "Utils.h"

class Scene;

class Camera {
public:
    // Camera state
    float x, y, z;
    float yaw, pitch;
    float moveSpeed;
    float mouseSensitivity;

    // Constructor with standard human eye-height defaults
    Camera(float startX = 0.0f, float startY = 1.7f, float startZ = 3.0f);

    // Input processing methods
    void processKeyboard(GLFWwindow* window, float deltaTime, const Scene& scene);
    void processMouse(float xoffset, float yoffset);

    // Generates and applies the view matrix via gluLookAt
    void applyViewMatrix();

    //Aquires positions and foward looking vectors
    void getPosition(float& x, float& y, float& z) const;
    void getForwardVector(float& dx, float& dy, float& dz) const;
};

#endif // CAMERA_H