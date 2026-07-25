#include <GL/glut.h>
#include <iostream>

// Temporal configuration: 60 updates per second (1000ms / 60 ≈ 16ms) [cite: 41, 42]
const int TARGET_FPS = 60;
const int FRAME_DELAY = 1000 / TARGET_FPS;

// Window dimensions
int windowWidth = 1280;
int windowHeight = 720;

// Initializes fundamental OpenGL 2.1 state machine settings [cite: 13, 35]
void initGL() {
    // Set the clear color to a pitch-black/midnight blue horror atmosphere
    glClearColor(1.0f, 0.0f, 1.0f, 1.0f);
    
    // Enable Depth Testing so closer triangles occlude further ones 
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    
    // Enable smooth shading (legacy Gouraud shading baseline) [cite: 18]
    glShadeModel(GL_SMOOTH);
    
    std::cout << "[SYSTEM] OpenGL 2.1 Context Initialized." << std::endl;
}

// Fixed-timestep logic & physics update callback [cite: 41, 42]
void update(int value) {
    // TODO: In future steps, WASD movement, debris gravity, and Bézier timers go here [cite: 6, 10, 11]
    
    // Request a screen redraw from GLUT [cite: 43]
    glutPostRedisplay();
    
    // Re-register the timer to maintain our continuous 60Hz loop [cite: 42, 43]
    glutTimerFunc(FRAME_DELAY, update, 0);
}

// Rendering callback (Strictly consumes state, executes GPU draw commands) [cite: 44, 45]
void display() {
    // Clear both Color and Depth buffers before drawing the new frame 
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // Reset the Modelview matrix stack to identity [cite: 46]
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    // TODO: Camera view setup (gluLookAt) and room rendering will go here [cite: 6, 27, 45]
    
    // Swap the back and front buffers to display the rendered frame 
    glutSwapBuffers();
}

// Handles window resizing and maintains aspect ratio [cite: 20]
void reshape(int width, int height) {
    if (height == 0) height = 1; // Prevent divide by zero
    windowWidth = width;
    windowHeight = height;
    
    // Set the OpenGL viewport to cover the entire window
    glViewport(0, 0, width, height);
    
    // Switch to Projection matrix to adjust lens/perspective [cite: 20]
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    
    // Set up a standard 45-degree field of view perspective lens [cite: 20]
    float aspectRatio = (float)width / (float)height;
    gluPerspective(45.0f, aspectRatio, 0.1f, 100.0f);
    
    // Return to Modelview matrix mode for standard rendering [cite: 46]
    glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char** argv) {
    // Initialize GLUT and pass command line arguments
    glutInit(&argc, argv);
    
    // Request Double Buffer, RGB color, and Depth buffer 
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(windowWidth, windowHeight);
    glutCreateWindow("Survival Horror - OpenGL 2.1 Engine");
    
    initGL();
    
    // Wire up GLUT callbacks
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    
    // Start the fixed 60Hz update loop (fires first update immediately) [cite: 41, 42]
    glutTimerFunc(0, update, 0);
    
    // Hand over thread execution to the GLUT event loop [cite: 37, 38]
    glutMainLoop();
    return 0;
}