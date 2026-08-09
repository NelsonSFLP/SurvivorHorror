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
    GLuint texFloor, texWall, texWood, texGrass, texDirt, texSky; 

    // Helper methods for procedural geometry
    float getTerrainHeight(float x, float z) const;
    void drawLowPolyCone(float radius, float height);
    void drawTree(float x, float z, int treeType);
    void drawForestTile(float startX, float startZ);
    void drawSolidCube(float size);
    void drawThickWall(float x, float z, float widthX, float depthZ);
    void drawLowPolyCylinder(float radius, float length);
    void drawWindowWall(float x, float z, float widthX, float depthZ, float sillHeight = 1.0f, float lintelHeight = 1.0f);
    void drawEnvironment();
    void drawDeskWithDrawer();
    void drawSkybox(float camX, float camY, float camZ);

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

    // Initializes dynamic lights
    void setupCabinLights();   
    void positionCabinLights();

    // Loads the image files into VRAM in the start
    void loadAssets();
    
    // Configures GL_LIGHT0 in Eye Space 
    void setupTacticalFlashlight();
    
    // Renders all world geometry 
    void render(float camX, float camY, float camZ);

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