#ifndef DEBRIS_H
#define DEBRIS_H

#include "Utils.h"
#include <GLFW/glfw3.h>
#include <cstdlib>

// A lightweight physics object
struct Particle {
    bool active;
    float x, y, z;
    float vx, vy, vz;
    float life;       // How long before the particle despawns
    GLuint textureID; // The inherited texture (wood, wallpaper, etc.)
    float uOffset, vOffset; // To grab a specific sliver of the UV map
};

class DebrisSystem {
private:
    static const int MAX_PARTICLES = 500;
    Particle pool[MAX_PARTICLES];

public:
    DebrisSystem();
    
    // Spawns a burst of particles at the impact site
    void spawnExplosion(float hitX, float hitY, float hitZ, GLuint texID);
    
    // Applies Euler integration for gravity and velocity
    void update(float deltaTime);
    
    // Renders the 3D splinters
    void render() const;
};

#endif // DEBRIS_H