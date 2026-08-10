#include "Engine.h"
#include <iostream>
#include <cmath>
#include <string>

Engine::Engine(int width, int height, const char* title)
    : window(nullptr), width(width), height(height), title(title),
      camera(0.0f, 1.7f, 18.0f),
      deltaTime(0.0f), lastFrame(0.0f),
      lastMouseX(width / 2.0), lastMouseY(height / 2.0), firstMouse(true),
      currentState(GameState::AWAKENING),
      awakeningTimer(0.0f) {}

Engine::~Engine() {
    if (window) {
        glfwDestroyWindow(window);
    }
    glfwTerminate();
    std::cout << "[SYSTEM] Engine terminated cleanly." << std::endl;
}

void Engine::handleInteraction() {
    if (currentState == GameState::AWAKENING || currentState == GameState::KEYPAD) return;

    // 1. Get exact position
    float cX, cY, cZ; 
    camera.getPosition(cX, cY, cZ);
    
    // 2. Get exact forward direction using pass-by-reference!
    float dX, dY, dZ;
    camera.getForwardVector(dX, dY, dZ);

    // 3. Cast the ray
    Ray ray = { cX, cY, cZ, dX, dY, dZ };

    if (scene.tryInteract(ray)) {
        if (scene.isChestKeypadActive) {
            currentState = GameState::KEYPAD;
        } else if (scene.justPickedUpShotgun) {
            // STATE TRANSITION: The player has armed themselves!
            currentState = GameState::COMBAT;
            scene.spawnMonsters(); // SPAWN THE HORDE!
            std::cout << "[SYSTEM] SHOTGUN ACQUIRED. THE HUNT BEGINS." << std::endl;
        }
    }
}

void Engine::handleShooting() {
    // Only allow firing if the player is actively in COMBAT!
    if (currentState != GameState::COMBAT) return; 

    float cX, cY, cZ; camera.getPosition(cX, cY, cZ);
    float dX, dY, dZ; camera.getForwardVector(dX, dY, dZ);
    Ray ray = { cX, cY, cZ, dX, dY, dZ };

    // Trigger the debris system
    scene.shoot(ray); 

    // Check Win Condition
    if (scene.monstersKilled >= 5) {
        currentState = GameState::COMPLETED;
        endTimer = 0.0f;
        std::cout << "[SYSTEM] ALL ENTITIES ELIMINATED. YOU SURVIVED." << std::endl;
    }
}

void Engine::renderBlinkOverlay(int width, int height) {
    if (currentState != GameState::AWAKENING) return;

    float progress = awakeningTimer / 4.0f;
    
    // MATHEMATICAL BLINKING:
    // cos() creates the open/close blink rhythm.
    // (1.0f - progress) forces the blackness to fade away as time passes.
    float alpha = fabs(cos(awakeningTimer * 3.5f)) * (1.0f - progress);

    // Clamp alpha safely
    if (alpha < 0.0f) alpha = 0.0f;
    if (alpha > 1.0f) alpha = 1.0f;

    // Switch OpenGL to 2D Orthographic UI mode
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, width, height, 0, -1, 1); // 2D Screen Space

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    // Draw a pitch-black fullscreen quad that fades out
    glColor4f(0.0f, 0.0f, 0.0f, alpha);
    glBegin(GL_QUADS);
        glVertex2f(0.0f, 0.0f);
        glVertex2f((float)width, 0.0f);
        glVertex2f((float)width, (float)height);
        glVertex2f(0.0f, (float)height);
    glEnd();

    // Restore 3D Perspective mode
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);

    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
}

void Engine::updateAwakening(float deltaTime) {
    if (currentState != GameState::AWAKENING) return;

    awakeningTimer += deltaTime;
    float duration = 4.0f; // The sequence lasts 4 seconds
    float progress = awakeningTimer / duration;

    if (progress > 1.0f) {
        progress = 1.0f;
        currentState = GameState::EXPLORING; // Give control back to the player!
    }

    // Linearly interpolate the camera height from 0.2f (ground) to 1.7f (standing)
    camera.y = 0.2f + (progress * 1.5f);
}

static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    // Handle Shotgun Blast (Left Mouse Button)
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        Engine* engine = static_cast<Engine*>(glfwGetWindowUserPointer(window));
        if (engine) {
            engine->handleShooting();
        }
    }
}

static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    // 1. We MUST extract the engine pointer from the GLFW window!
    Engine* engine = static_cast<Engine*>(glfwGetWindowUserPointer(window));
    if (!engine) return;

    // 2. KEYPAD PUZZLE STATE LOGIC
    if (engine->currentState == GameState::KEYPAD) {
        if (action == GLFW_PRESS) {
            // Exit Keypad
            if (key == GLFW_KEY_ESCAPE || key == GLFW_KEY_E) {
                engine->scene.isChestKeypadActive = false;
                engine->currentState = GameState::EXPLORING;
                engine->scene.currentCode = ""; 
            }
            // Delete Number
            else if (key == GLFW_KEY_BACKSPACE && !engine->scene.currentCode.empty()) {
                engine->scene.currentCode.pop_back();
            }
            // Type Number (0-9)
            else if (engine->scene.currentCode.length() < 4) {
                if (key >= GLFW_KEY_0 && key <= GLFW_KEY_9) {
                    engine->scene.currentCode += std::to_string(key - GLFW_KEY_0);
                } else if (key >= GLFW_KEY_KP_0 && key <= GLFW_KEY_KP_9) {
                    engine->scene.currentCode += std::to_string(key - GLFW_KEY_KP_0);
                }
            }

            // Check Win Condition!
            if (engine->scene.currentCode.length() == 4) {
                if (engine->scene.currentCode == engine->scene.correctCode) {
                    engine->scene.isChestUnlocked = true;
                    engine->scene.isChestKeypadActive = false;
                    engine->currentState = GameState::EXPLORING;
                    std::cout << "[SYSTEM] CHEST UNLOCKED!" << std::endl;
                } else {
                    engine->scene.currentCode = "";
                    std::cout << "[SYSTEM] ERROR: Incorrect Code." << std::endl;
                }
            }
        }
        return; // BLOCK ALL OTHER INPUTS WHILE TYPING!
    }

    // 3. NORMAL EXPLORATION LOGIC
    if (action == GLFW_PRESS) {
        if (key == GLFW_KEY_ESCAPE) {
            glfwSetWindowShouldClose(window, true);
        } 
        // RESTORE THE INTERACTION TRIGGER!
        else if (key == GLFW_KEY_E) {
            engine->handleInteraction();
        }
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

    camera.processKeyboard(window, deltaTime, scene);
}

int Engine::run() {
    // 1. Hand the Engine pointer to GLFW so static callbacks can access it!
    glfwSetWindowUserPointer(window, this); 

    // 2. Register callbacks once BEFORE the loop starts
    glfwSetKeyCallback(window, keyCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);

    // --- CORE GAME LOOP ---
    while (!glfwWindowShouldClose(window)) {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        glfwPollEvents();

        // --- 1. UPDATE PHASE ---
        float px, py, pz; 
        camera.getPosition(px, py, pz);

        if (currentState == GameState::AWAKENING) {
            updateAwakening(deltaTime);
        } else if (currentState == GameState::KEYPAD || currentState == GameState::COMPLETED || currentState == GameState::GAME_OVER) {
            // Freeze WASD movement, but keep updating physics
            scene.updatePhysics(deltaTime, px, pz);
        } else {
            // Fully awake and exploring/combat
            processInput();
            camera.getPosition(px, py, pz);
            scene.updatePhysics(deltaTime, px, pz);
        }

        // Check Death Condition!
        if (currentState == GameState::COMBAT && scene.playerHealth <= 0.0f) {
            currentState = GameState::GAME_OVER;
            endTimer = 0.0f;
            std::cout << "[SYSTEM] YOU DIED." << std::endl;
        }

        // --- 2. RENDER PIPELINE ---
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        // Bind tactical spotlight in Eye Space (before camera transformation)
        scene.setupTacticalFlashlight();

        // Apply camera transformations (gluLookAt)
        camera.applyViewMatrix();

        // Extract camera coordinates for the Skybox rendering
        float cX, cY, cZ;
        camera.getPosition(cX, cY, cZ);

        // Render world geometry (Skybox, Cabin, Forest, etc.)
        scene.render(cX, cY, cZ);

        // Overlay for interactive objects (e.g., Reading notes)
        scene.renderOverlay();

        // Draw the first-person shotgun if we are in combat
        if (currentState == GameState::COMBAT) {
            scene.drawViewModel();
            scene.renderDamageOverlay(width, height); // Hook up the visual damage!
        }

        // --- 3. CINEMATIC OVERLAY ---
        // Dynamically get the window dimensions so the blink overlay covers the whole screen
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        renderBlinkOverlay(width, height);

        // Draw the digital keypad if active
        scene.renderKeypadUI(width, height);

        // Draw the End Screens
        if (currentState == GameState::COMPLETED || currentState == GameState::GAME_OVER) {
            endTimer += deltaTime;
            
            // Cleanly call the modular rendering function!
            scene.renderEndScreen(width, height, currentState == GameState::COMPLETED, endTimer);

            // Trigger the final event after 6 seconds
            if (endTimer > 6.0f) {
                if (currentState == GameState::COMPLETED) {
                    glfwSetWindowShouldClose(window, true);
                } else {
                    resetGame(); // Wake up in the forest again!
                }
            }
        }
        
        // Swap buffers and poll IO events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    return 0;
}

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

void Engine::resetGame() {
    currentState = GameState::AWAKENING;
    awakeningTimer = 0.0f;
    endTimer = 0.0f;
    scene.reset();
    camera.reset();
    std::cout << "[SYSTEM] GAME RESET." << std::endl;
}