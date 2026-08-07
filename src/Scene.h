#ifndef SCENE_H
#define SCENE_H

#include "Utils.h"
#include "TextureManager.h"

class Scene {
private:
    // texture loader and VRAM cache
    TextureManager texManager;
    
    // Texture IDs
    GLuint texFloor; 
    GLuint texWall;
    GLuint texWood;

    // Helper methods for procedural geometry
    void drawSolidCube(float size);
    void drawAbandonedRoom();
    void drawDeskWithDrawer(float drawerOffsetZ);

public:
    Scene();
    
    // Initializes fixed-function lighting state (called once at startup)
    void initLighting();

    //Loads the image files into VRAM in the start
    void loadAssets();
    
    // Configures GL_LIGHT0 in Eye Space 
    void setupTacticalFlashlight();
    
    // Renders all world geometry 
    void render(float drawerOffsetZ = 0.2f);
};

#endif // SCENE_H