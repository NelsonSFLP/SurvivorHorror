#include "Camera.h"
#include "Scene.h"

Camera::Camera(float startX, float startY, float startZ) 
    : x(startX), y(startY), z(startZ), 
      yaw(-90.0f), pitch(0.0f), 
      moveSpeed(3.0f), mouseSensitivity(0.02f) {}

void Camera::processKeyboard(GLFWwindow* window, float deltaTime, const Scene& scene) {
    float radYaw = toRadians(yaw);
    float radPitch = toRadians(pitch);

    float forwardX = cos(radYaw) * cos(radPitch);
    float forwardZ = sin(radYaw) * cos(radPitch);
    
    float rightX = -sin(radYaw);
    float rightZ = cos(radYaw);

    float velocity = moveSpeed * deltaTime;

    // 1. Tally up the intended movement for this frame
    float moveX = 0.0f;
    float moveZ = 0.0f;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        moveX += forwardX * velocity;
        moveZ += forwardZ * velocity;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        moveX -= forwardX * velocity;
        moveZ -= forwardZ * velocity;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        moveX -= rightX * velocity;
        moveZ -= rightZ * velocity;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        moveX += rightX * velocity;
        moveZ += rightZ * velocity;
    }

    // 2. Resolve Collision independently on X and Z for smooth wall sliding!
    if (scene.isWalkable(this->x + moveX, this->z)) {
        this->x += moveX;
    }
    if (scene.isWalkable(this->x, this->z + moveZ)) {
        this->z += moveZ;
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

void Camera::getPosition(float& outX, float& outY, float& outZ) const {
    outX = this->x;
    outY = this->y;
    outZ = this->z;
}

void Camera::getForwardVector(float& dx, float& dy, float& dz) const {
    // We reuse the exact same trigonometric math from your applyViewMatrix() method!
    float radYaw = toRadians(yaw);
    float radPitch = toRadians(pitch);
    
    // Calculate the normalized directional vector (where the camera is looking)
    dx = cos(radYaw) * cos(radPitch);
    dy = sin(radPitch);
    dz = sin(radYaw) * cos(radPitch);
}

void Camera::reset() {
    x = 0.0f;
    y = 1.7f;
    z = 15.0f; // Return to the forest spawn point!
    yaw = -90.0f;
    pitch = 0.0f;
}