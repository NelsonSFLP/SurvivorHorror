#ifndef ENGINE_H
#define ENGINE_H

#include "Utils.h"
#include "Camera.h"
#include "Scene.h"

class Engine {
private:
    // Window state
    GLFWwindow* window;
    int width;
    int height;
    const char* title;

    // Core systems
    Camera camera;
    Scene scene;

    // Time tracking
    float deltaTime;
    float lastFrame;

    // Mouse input tracking
    double lastMouseX;
    double lastMouseY;
    bool firstMouse;

    // Private helper methods
    void processInput();

    // --- STATIC GLFW CALLBACKS ---
    // These must be static to interface with GLFW's C-style API
    static void framebufferSizeCallback(GLFWwindow* window, int width, int height);
    static void mouseCallback(GLFWwindow* window, double xpos, double ypos);
    static void windowFocusCallback(GLFWwindow* window, int focused);

public:
    Engine(int width, int height, const char* title);
    ~Engine();

    // Initializes GLFW, OpenGL 2.1 context, and hardware callbacks
    bool init();

    // Executes the core game loop
    int run();
};

#endif // ENGINE_H