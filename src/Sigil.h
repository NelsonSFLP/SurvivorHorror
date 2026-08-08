#ifndef SIGIL_H
#define SIGIL_H

#include "Utils.h"
#include <GLFW/glfw3.h>

class Sigil {
private:
    float animationTimer;
    bool isAnimating;

public:
    Sigil();
    
    // Controls the playback of the blood-drawing animation
    void startAnimation();
    void reset();
    
    // Advances the animation timer
    void update(float deltaTime);
    
    // Submits the Bezier control points and evaluators to the GPU
    void render() const;
};

#endif // SIGIL_H