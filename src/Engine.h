#ifndef ENGINE_H
#define ENGINE_H

#include "Utils.h"
#include "Camera.h"
#include "Scene.h"
#include <string.h>

// Define our core gameplay loop states
enum class GameState {
    AWAKENING,
    EXPLORING,
    KEYPAD,     
    COMBAT,
    COMPLETED
};

class Engine {
private:
    // Window state
    GLFWwindow* window;
    int width;
    int height;
    const char* title;

    // Time tracking
    float deltaTime;
    float lastFrame;

    // Mouse input tracking
    double lastMouseX;
    double lastMouseY;
    bool firstMouse;

    // Private helper methods
    void processInput();

    // Awakening State Helpers
    void updateAwakening(float deltaTime);
    void renderBlinkOverlay(int width, int height);

    // --- STATIC GLFW CALLBACKS ---
    // These must be static to interface with GLFW's API
    static void framebufferSizeCallback(GLFWwindow* window, int width, int height);
    static void mouseCallback(GLFWwindow* window, double xpos, double ypos);
    static void windowFocusCallback(GLFWwindow* window, int focused);


public:
    Engine(int width, int height, const char* title);
    ~Engine();

    // Core systems
    Camera camera;
    Scene scene;

    // Initializes GLFW, OpenGL 2.1 context, and hardware callbacks
    bool init();

    // Executes the core game loop
    int run();

    // Game State variables
    GameState currentState;
    float awakeningTimer;

    // Deals with player interactions 
    void handleInteraction();
    void handleShooting();
};

#endif // ENGINE_H