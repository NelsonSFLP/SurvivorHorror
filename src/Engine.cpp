#include "Engine.h"

Engine::Engine(int width, int height, const char* title)
    : window(nullptr), width(width), height(height), title(title),
      camera(0.0f, 1.7f, 5.0f),
      deltaTime(0.0f), lastFrame(0.0f),
      lastMouseX(width / 2.0), lastMouseY(height / 2.0), firstMouse(true) {}

Engine::~Engine() {
    if (window) {
        glfwDestroyWindow(window);
    }
    glfwTerminate();
    std::cout << "[SYSTEM] Engine terminated cleanly." << std::endl;
}

void Engine::handleInteraction() {
    Ray ray;
    camera.getPosition(ray.origin[0], ray.origin[1], ray.origin[2]);
    camera.getForwardVector(ray.direction[0], ray.direction[1], ray.direction[2]);

    scene.tryInteract(ray);
}

// Triggers precisely once per key press, preventing machine-gun-style rapid fire interactions
static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    // Handle Interaction ('E' key)
    if (key == GLFW_KEY_E && action == GLFW_PRESS) {
        Engine* engine = static_cast<Engine*>(glfwGetWindowUserPointer(window));
        if (engine) {
            engine->handleInteraction();
        }
    }

    // Handle Window Close ('ESC' key)
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}

bool Engine::init() {
    if (!glfwInit()) {
        std::cerr << "[ERROR] Failed to initialize GLFW." << std::endl;
        return false;
    }

    // Enforce OpenGL 2.1 fixed-function profile
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);

    window = glfwCreateWindow(width, height, title, NULL, NULL);
    if (!window) {
        std::cerr << "[ERROR] Failed to create GLFW window." << std::endl;
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(window);

    // --- THE C++ CAPTURE TRICK ---
    // Attach 'this' class instance directly to the GLFW window memory
    glfwSetWindowUserPointer(window, this);

    // Register static callbacks
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    glfwSetCursorPosCallback(window, mouseCallback);
    glfwSetWindowFocusCallback(window, windowFocusCallback);

    // Trap cursor and enable raw mouse input
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    if (glfwRawMouseMotionSupported()) {
        glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
        std::cout << "[SYSTEM] Raw hardware mouse motion enabled." << std::endl;
    }

    // Initialize lighting and initial projection matrix
    scene.initLighting();
    scene.loadAssets();
    framebufferSizeCallback(window, width, height);
    glClearColor(0.1f, 0.1f, 0.15f, 1.0f);

    // Sync initial mouse coordinates from OS to prevent startup camera jump
    glfwGetCursorPos(window, &lastMouseX, &lastMouseY);
    firstMouse = false;

    return true;
}

void Engine::processInput() {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    camera.processKeyboard(window, deltaTime);
}

int Engine::run() {
    // --- CORE GAME LOOP ---
    while (!glfwWindowShouldClose(window)) {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        glfwPollEvents();
        processInput();

        // Render Pipeline
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        // Mouse callback
        glfwSetKeyCallback(window, keyCallback);

        // 1. Bind tactical spotlight in Eye Space (before camera transformation)
        scene.setupTacticalFlashlight();

        // 2. Apply camera transformations (gluLookAt)
        camera.applyViewMatrix();

        // 3. Render world geometry
        scene.render(); 

        // 4. Physics update
        scene.updatePhysics(deltaTime);

        //overlay for interactive objects
        scene.renderOverlay();

        glfwSwapBuffers(window);
    }

    return 0;
}

// --- STATIC CALLBACK IMPLEMENTATIONS ---
void Engine::framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    if (height == 0) height = 1;
    
    // Retrieve class instance from window memory
    Engine* engine = static_cast<Engine*>(glfwGetWindowUserPointer(window));
    if (engine) {
        engine->width = width;
        engine->height = height;
    }

    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    
    float aspectRatio = static_cast<float>(width) / static_cast<float>(height);
    gluPerspective(45.0f, aspectRatio, 0.1f, 100.0f);
    glMatrixMode(GL_MODELVIEW);
}

void Engine::mouseCallback(GLFWwindow* window, double xpos, double ypos) {
    Engine* engine = static_cast<Engine*>(glfwGetWindowUserPointer(window));
    if (!engine) return;

    if (engine->firstMouse) {
        engine->lastMouseX = xpos;
        engine->lastMouseY = ypos;
        engine->firstMouse = false;
    }

    float xoffset = static_cast<float>(xpos - engine->lastMouseX);
    float yoffset = static_cast<float>(engine->lastMouseY - ypos);

    engine->lastMouseX = xpos;
    engine->lastMouseY = ypos;

    engine->camera.processMouse(xoffset, yoffset);
}

void Engine::windowFocusCallback(GLFWwindow* window, int focused) {
    if (focused) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }
}