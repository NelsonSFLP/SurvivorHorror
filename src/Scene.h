#ifndef SCENE_H
#define SCENE_H

#include "Utils.h"

class Scene {
private:
    // Helper methods for procedural geometry
    void drawSolidCube(float size);
    void drawAbandonedRoom();
    void drawDeskWithDrawer(float drawerOffsetZ);

public:
    Scene();
    
    // Initializes fixed-function lighting state (called once at startup)
    void initLighting();
    
    // Configures GL_LIGHT0 in Eye Space (must be called BEFORE applyViewMatrix)
    void setupTacticalFlashlight();
    
    // Renders all world geometry (must be called AFTER applyViewMatrix)
    void render(float drawerOffsetZ = 0.2f);
};

#endif // SCENE_H