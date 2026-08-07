#include "Debris.h"

DebrisSystem::DebrisSystem() {
    // Initialize all particles as dead
    for (int i = 0; i < MAX_PARTICLES; ++i) {
        pool[i].active = false;
    }
}

void DebrisSystem::spawnExplosion(float hitX, float hitY, float hitZ, GLuint texID) {
    int particlesToSpawn = 30; // 30 splinters per shotgun blast
    
    for (int i = 0; i < MAX_PARTICLES && particlesToSpawn > 0; ++i) {
        if (!pool[i].active) {
            pool[i].active = true;
            pool[i].x = hitX;
            pool[i].y = hitY;
            pool[i].z = hitZ;
            
            // Random burst velocity (Explode outward on X/Z, burst upward on Y)
            pool[i].vx = ((rand() % 100) / 50.0f - 1.0f) * 2.0f;
            pool[i].vy = ((rand() % 100) / 100.0f) * 3.0f + 1.0f;
            pool[i].vz = ((rand() % 100) / 50.0f - 1.0f) * 2.0f;
            
            pool[i].life = 2.0f; // Despawn after 2 seconds
            pool[i].textureID = texID;
            
            // Randomize UV offsets so it looks like a torn fragment of the object
            pool[i].uOffset = (rand() % 100) / 100.0f;
            pool[i].vOffset = (rand() % 100) / 100.0f;
            
            particlesToSpawn--;
        }
    }
}

void DebrisSystem::update(float deltaTime) {
    float gravity = -9.8f; // Earth gravity on the Y-axis
    
    for (int i = 0; i < MAX_PARTICLES; ++i) {
        if (pool[i].active) {
            // 1. Euler Integration for physics
            pool[i].vy += gravity * deltaTime;
            pool[i].x += pool[i].vx * deltaTime;
            pool[i].y += pool[i].vy * deltaTime;
            pool[i].z += pool[i].vz * deltaTime;
            
            // 2. Floor collision (The floor is at Y = 0.0f)
            if (pool[i].y < 0.05f) {
                pool[i].y = 0.05f;
                pool[i].vy = 0.0f;  // Stop falling
                pool[i].vx *= 0.8f; // Apply floor friction to slide to a halt
                pool[i].vz *= 0.8f;
            }
            
            // 3. Lifecycle management
            pool[i].life -= deltaTime;
            if (pool[i].life <= 0.0f) {
                pool[i].active = false;
            }
        }
    }
}

void DebrisSystem::render() const {
    glEnable(GL_TEXTURE_2D);
    glColor3f(1.0f, 1.0f, 1.0f); // Reset color to allow texture colors to show
    
    for (int i = 0; i < MAX_PARTICLES; ++i) {
        if (pool[i].active) {
            glBindTexture(GL_TEXTURE_2D, pool[i].textureID);
            
            glPushMatrix();
                glTranslatef(pool[i].x, pool[i].y, pool[i].z);
                
                // Spin dynamically based on remaining life
                glRotatef(pool[i].life * 360.0f, 1.0f, 1.0f, 0.0f);
                
                // Draw a 0.1m splinter facing the camera
                glBegin(GL_QUADS);
                    glNormal3f(0.0f, 0.0f, 1.0f); // Catch flashlight illumination
                    
                    glTexCoord2f(pool[i].uOffset, pool[i].vOffset); 
                    glVertex3f(-0.05f, -0.05f, 0.0f);
                    
                    glTexCoord2f(pool[i].uOffset + 0.1f, pool[i].vOffset); 
                    glVertex3f(0.05f, -0.05f, 0.0f);
                    
                    glTexCoord2f(pool[i].uOffset + 0.1f, pool[i].vOffset + 0.1f); 
                    glVertex3f(0.05f, 0.05f, 0.0f);
                    
                    glTexCoord2f(pool[i].uOffset, pool[i].vOffset + 0.1f); 
                    glVertex3f(-0.05f, 0.05f, 0.0f);
                glEnd();
            glPopMatrix();
        }
    }
}