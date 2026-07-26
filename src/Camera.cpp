#include "Camera.h"

Camera::Camera(float startX, float startY, float startZ) 
    : x(startX), y(startY), z(startZ), 
      yaw(-90.0f), pitch(0.0f), 
      moveSpeed(3.0f), mouseSensitivity(0.02f) {}

void Camera::processKeyboard(GLFWwindow* window, float deltaTime) {
    float radYaw = toRadians(yaw);
    float radPitch = toRadians(pitch);

    // Calculate forward movement vector on the X-Z floor plane
    float forwardX = cos(radYaw) * cos(radPitch);
    float forwardZ = sin(radYaw) * cos(radPitch);
    
    // Calculate right vector via cross product with World Up (0, 1, 0)
    float rightX = -sin(radYaw);
    float rightZ = cos(radYaw);

    float velocity = moveSpeed * deltaTime;

    // Apply movement restricted to X-Z plane (ignoring Y to prevent flying)
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        x += forwardX * velocity;
        z += forwardZ * velocity;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        x -= forwardX * velocity;
        z -= forwardZ * velocity;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        x -= rightX * velocity;
        z -= rightZ * velocity;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        x += rightX * velocity;
        z += rightZ * velocity;
    }
}

void Camera::processMouse(float xoffset, float yoffset) {
    xoffset *= mouseSensitivity;
    yoffset *= mouseSensitivity;

    yaw += xoffset;
    pitch += yoffset;

    // Clamp pitch to prevent screen inversion
    if (pitch > 89.0f) pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;
}

void Camera::applyViewMatrix() {
    float radYaw = toRadians(yaw);
    float radPitch = toRadians(pitch);

    // Spherical to Cartesian target calculation
    float targetX = x + (cos(radYaw) * cos(radPitch));
    float targetY = y + sin(radPitch);
    float targetZ = z + (sin(radYaw) * cos(radPitch));

    // Apply the transformation to OpenGL's Modelview stack
    gluLookAt(x, y, z,
              targetX, targetY, targetZ,
              0.0f, 1.0f, 0.0f);
}