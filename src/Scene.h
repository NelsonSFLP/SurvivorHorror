#ifndef SCENE_H
#define SCENE_H

#include "Utils.h"
#include "TextureManager.h"
#include "Debris.h"
#include "Sigil.h"

class Scene {
private:
    // texture loader and VRAM cache
    TextureManager texManager;
    
    // Texture IDs
    GLuint texFloor, texWall, texWood; 

    // Helper methods for procedural geometry
    void drawSolidCube(float size);
    void drawAbandonedRoom();
    void drawDeskWithDrawer();

    //drawer manipulation 
    bool isDrawerOpen;
    float currentDrawerZ; // Smooth animation offset

    //note inspection
    bool isInspectingNote;
    void drawNote();

    // Debri pool
    DebrisSystem debris;

    // Sigil using bezier curves
    Sigil cursedSigil;

public:
    Scene();
    
    // Initializes fixed-function lighting state (called once at startup)
    void initLighting();

    // Loads the image files into VRAM in the start
    void loadAssets();
    
    // Configures GL_LIGHT0 in Eye Space 
    void setupTacticalFlashlight();
    
    // Renders all world geometry 
    void render();

    // Interation functions
    void updatePhysics(float deltaTime);
    bool tryInteract(const Ray& cameraRay);
    void shoot(const Ray& cameraRay);

    // Overlay for interactive objects
    void renderOverlay();

    // Checking player walkable zone
    bool isWalkable(float targetX, float targetZ) const;
};

#endif // SCENE_H