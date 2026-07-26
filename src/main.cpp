#include "Engine.h"
#include <iostream>

int main() {
    // Instantiate the engine with standard 720p resolution
    Engine engine(1280, 720, "Survival Horror - OpenGL 2.1 Modular Engine");
    
    // Initialize systems and hardware contexts
    if (!engine.init()) {
        std::cerr << "[ERROR] Fatal: Engine failed to initialize." << std::endl;
        return -1;
    }

    // Hand over thread execution to the game loop
    return engine.run();
}