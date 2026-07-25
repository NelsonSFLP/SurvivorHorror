// --- PLATFORM SPECIFIC HEADERS ---
#ifdef _WIN32
#include <windows.h> // Required on Windows to define APIENTRY and CALLBACK macros for GLU
#endif

#include <GLFW/glfw3.h>
#include <GL/glu.h> 
#include <iostream>
#include <cmath>

// --- WINDOW & TIME STATE ---
int windowWidth = 1280;
int windowHeight = 720;
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// --- CAMERA STATE VARIABLES ---
float camX = 0.0f, camY = 1.7f, camZ = 5.0f; // Standard human eye height (1.7m)
float yaw = -90.0f;   // Looking down the -Z axis initially
float pitch = 0.0f;
float moveSpeed = 3.0f; // Speed is now measured in units per second (scaled by deltaTime)
float mouseSensitivity = 0.02f;

// --- INPUT TRACKING ---
double lastMouseX = windowWidth / 2.0;
double lastMouseY = windowHeight / 2.0;
bool firstMouse = true;

//Convert degrees to radians
float toRadians(float degrees) {
    return degrees * (M_PI / 180.0f);
}

// Initializes fundamental OpenGL 2.1 state machine settings 
void initGL() {
    // Set the clear color to a pitch-black/midnight blue horror atmosphere
    glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
    
    // Enable Depth Testing so closer triangles occlude further ones 
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    
    // Enable smooth shading (legacy Gouraud shading baseline) 
    glShadeModel(GL_SMOOTH);
    
    std::cout << "[SYSTEM] OpenGL 2.1 Context Initialized." << std::endl;
}

// --- GLFW MOUSE LOOK CALLBACK ---
// Fires only when the OS registers physical mouse movement
void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
    // Calculate raw displacement from the last frame's recorded position 
    float xoffset = static_cast<float>(xpos - lastMouseX);
    float yoffset = static_cast<float>(lastMouseY - ypos); // Inverted Y for 3D coordinate system

    // Update tracking variables immediately
    lastMouseX = xpos;
    lastMouseY = ypos;

    // Apply sensitivity scaling to the displacement 
    xoffset *= mouseSensitivity;
    yoffset *= mouseSensitivity;

    // Accumulate Euler angles 
    yaw += xoffset;
    pitch += yoffset;

    // Clamp pitch to prevent camera inversion (-89 to +89 degrees) 
    if (pitch > 89.0f) pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;
}

void processInput(GLFWwindow* window) {
    // ESC key closes the application
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // 1. Calculate Forward Vector from yaw and pitch using spherical coordinates
    float radYaw = toRadians(yaw);
    float radPitch = toRadians(pitch);
    
    float forwardX = cos(radYaw) * cos(radPitch);
    float forwardY = sin(radPitch);
    float forwardZ = sin(radYaw) * cos(radPitch);
    
    // 2. Calculate Right Vector via Cross Product with World Up (0, 1, 0)
    float rightX = -sin(radYaw);
    float rightZ = cos(radYaw);

    // 3. Scale movement velocity by deltaTime for frame-rate independence
    float velocity = moveSpeed * deltaTime;

    // WASD Movement (Restricted to the X-Z floor plane by ignoring Y components) 
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        camX += forwardX * velocity;
        camZ += forwardZ * velocity;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        camX -= forwardX * velocity;
        camZ -= forwardZ * velocity;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        camX -= rightX * velocity;
        camZ -= rightZ * velocity;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        camX += rightX * velocity;
        camZ += rightZ * velocity;
    }
}

void drawSolidCube(float size) {
    float half = size / 2.0f;
    glBegin(GL_QUADS);
        // Front Face
        glNormal3f(0.0f, 0.0f, 1.0f);
        glVertex3f(-half, -half,  half);
        glVertex3f( half, -half,  half);
        glVertex3f( half,  half,  half);
        glVertex3f(-half,  half,  half);
        // Back Face
        glNormal3f(0.0f, 0.0f, -1.0f);
        glVertex3f(-half, -half, -half);
        glVertex3f(-half,  half, -half);
        glVertex3f( half,  half, -half);
        glVertex3f( half, -half, -half);
        // Top Face
        glNormal3f(0.0f, 1.0f, 0.0f);
        glVertex3f(-half,  half, -half);
        glVertex3f(-half,  half,  half);
        glVertex3f( half,  half,  half);
        glVertex3f( half,  half, -half);
        // Bottom Face
        glNormal3f(0.0f, -1.0f, 0.0f);
        glVertex3f(-half, -half, -half);
        glVertex3f( half, -half, -half);
        glVertex3f( half, -half,  half);
        glVertex3f(-half, -half,  half);
        // Right Face
        glNormal3f(1.0f, 0.0f, 0.0f);
        glVertex3f( half, -half, -half);
        glVertex3f( half,  half, -half);
        glVertex3f( half,  half,  half);
        glVertex3f( half, -half,  half);
        // Left Face
        glNormal3f(-1.0f, 0.0f, 0.0f);
        glVertex3f(-half, -half, -half);
        glVertex3f(-half, -half,  half);
        glVertex3f(-half,  half,  half);
        glVertex3f(-half,  half, -half);
    glEnd();
}

// Helper function to draw a reference grid so movement is visible
void drawFloorGrid() {
    glColor3f(0.3f, 0.3f, 0.3f);
    glBegin(GL_LINES); 
    for (int i = -10; i <= 10; i++) {
        glVertex3f((float)i, 0.0f, -10.0f);
        glVertex3f((float)i, 0.0f,  10.0f);
        glVertex3f(-10.0f, 0.0f, (float)i);
        glVertex3f( 10.0f, 0.0f, (float)i);
    }
    glEnd(); 
    
    // Draw reference pillar at origin (0, 0, 0) using hierarchical matrix stack 
    glColor3f(0.8f, 0.2f, 0.2f);
    glPushMatrix();
        glTranslatef(0.0f, 1.0f, 0.0f); 
        glScalef(0.5f, 2.0f, 0.5f);
        drawSolidCube(1.0f);
    glPopMatrix(); 
}

// Handles window resizing and rebuilds the projection matrix 
void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    if (height == 0) height = 1;
    windowWidth = width;
    windowHeight = height;
    
    glViewport(0, 0, width, height);
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    
    float aspectRatio = static_cast<float>(width) / static_cast<float>(height);
    gluPerspective(45.0f, aspectRatio, 0.1f, 100.0f);
    
    glMatrixMode(GL_MODELVIEW);
}

void windowFocusCallback(GLFWwindow* window, int focused) {
    if (focused) {
        // Force GLFW to trap and hide the cursor inside the window 
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }
}

int main() {
    // 1. Initialize GLFW library
    if (!glfwInit()) {
        std::cerr << "[ERROR] Failed to initialize GLFW." << std::endl;
        return -1;
    }

    // Enforce OpenGL 2.1 context profile 
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);

    // 2. Create Window
    GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "Survival Horror - OpenGL 2.1 Engine", NULL, NULL);
    if (!window) {
        std::cerr << "[ERROR] Failed to create GLFW window." << std::endl;
        glfwTerminate();
        return -1;
    }

    // Make OpenGL context current for this thread
    glfwMakeContextCurrent(window);

    // 3. Register GLFW Callbacks
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    glfwSetCursorPosCallback(window, mouseCallback);

    // 4. Capture and trap the hardware mouse cursor for true FPS look 
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Initialize OpenGL state
    initGL();
    
    // Manually trigger projection matrix setup for initial window size
    framebufferSizeCallback(window, windowWidth, windowHeight);

    // --- NEW: LINUX CURSOR & INPUT PATCHES ---
    // 1. Register focus callback to recover cursor lock if window focus is lost
    glfwSetWindowFocusCallback(window, windowFocusCallback);

    // 2. Enable Raw Mouse Motion to bypass OS acceleration and scaling jitter
    if (glfwRawMouseMotionSupported()) {
        glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
        std::cout << "[SYSTEM] Raw hardware mouse motion enabled." << std::endl;
    } else {
        std::cout << "[WARNING] Raw mouse motion not supported by this OS/WM." << std::endl;
    }

    // 3. Force initial cursor trap 
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // 4. Actively sync initial cursor position from the OS to prevent startup camera jump
    glfwGetCursorPos(window, &lastMouseX, &lastMouseY);
    firstMouse = false; // Bypass the firstMouse check since we just synced manually
    // --- THE CORE GAME LOOP --- 
    while (!glfwWindowShouldClose(window)) {
        // Calculate Delta Time ($\Delta t$)
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Input Polling & State Updates
        glfwPollEvents();
        processInput(window);

        // --- RENDER PIPELINE ---
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        // Calculate Target point for camera lens
        float radYaw = toRadians(yaw);
        float radPitch = toRadians(pitch);
        float targetX = camX + (cos(radYaw) * cos(radPitch));
        float targetY = camY + sin(radPitch);
        float targetZ = camZ + (sin(radYaw) * cos(radPitch));
        
        // Apply camera transformation 
        gluLookAt(camX, camY, camZ,
                  targetX, targetY, targetZ,
                  0.0f, 1.0f, 0.0f);
        
        // Draw scene geometry
        drawFloorGrid();

        // Buffer Swap 
        glfwSwapBuffers(window);
    }

    // Clean up GLFW resources before exiting
    glfwTerminate();
    return 0;
}