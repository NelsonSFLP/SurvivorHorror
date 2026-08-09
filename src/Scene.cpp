#include "Scene.h"

Scene::Scene() : texFloor(0), texWall(0), texWood(0), isDrawerOpen(false), currentDrawerZ(0.0f), isInspectingNote(false) {
    // 2 Drawers in the Main Room (Sala Principal: Z from -2 to 12)
    drawers[0] = { -10.0f, 5.0f,  90.0f, false, 0.0f, false }; // Placed against the West wall
    drawers[1] = {  10.0f, 5.0f, -90.0f, false, 0.0f, false }; // Placed against the East wall

    // 1 Drawer in Secondary Room 1 (Quarto Esquerdo: X=-12 to -2, Z=-12 to -2)
    drawers[2] = { -10.0f, -10.0f, 90.0f, false, 0.0f, false }; 

    // 1 Drawer in Secondary Room 2 (Quarto Direito: X=2 to 12, Z=-12 to -2)
    drawers[3] = {  10.0f, -10.0f, -90.0f, false, 0.0f, false }; 

    // 1 Drawer in Secondary Room 3 (Corredor Central: X=-2 to 2, Z=-12 to -2)
    drawers[4] = {   0.0f, -10.0f,   0.0f, false, 0.0f, false }; // Placed at the very back wall
    
    // Chest located in the main room
    chestX = 3.0f; 
    chestZ = 11.0f; 
    chestYaw = 180.0f; // Facing into the room
    chestLidAngle = 0.0f;
    isChestUnlocked = false;
    currentCode = "";
    isChestKeypadActive = false;
    correctCode = "1234"; // Placeholder solution!
}

void Scene::updatePhysics(float deltaTime) {
    // Smoothly animate the drawer sliding open or closed
    for (int i = 0; i < 5; i++) {
        if (drawers[i].isOpen) {
            drawers[i].currentZOffset += 2.0f * deltaTime;
            if (drawers[i].currentZOffset > 0.5f) drawers[i].currentZOffset = 0.5f;
        } else {
            drawers[i].currentZOffset -= 2.0f * deltaTime;
            if (drawers[i].currentZOffset < 0.0f) drawers[i].currentZOffset = 0.0f;
        }
    }

    // Smoothly animate the chest open
    if (isChestUnlocked) {
        chestLidAngle -= 45.0f * deltaTime; // Swing open
        if (chestLidAngle < -110.0f) chestLidAngle = -110.0f;
    }
    cursedSigil.update(deltaTime);
    debris.update(deltaTime);
}

void Scene::shoot(const Ray& cameraRay) {
    float hitDistance = 0.0f;
    
    // Define the desk's physical boundaries
    float deskMinX = 2.0f - 0.8f; float deskMaxX = 2.0f + 0.8f;
    float deskMinY = 0.8f - 0.1f; float deskMaxY = 0.8f + 0.1f; 
    float deskMinZ = -3.5f - 0.4f; float deskMaxZ = -3.5f + 0.4f;

    AABB deskBox;
    deskBox.min[0] = deskMinX; deskBox.max[0] = deskMaxX;
    deskBox.min[1] = deskMinY; deskBox.max[1] = deskMaxY;
    deskBox.min[2] = deskMinZ; deskBox.max[2] = deskMaxZ;

    // Check if the shotgun blast hit the desk
    if (checkRayAABBIntersection(cameraRay, deskBox, hitDistance)) {
        // Calculate the exact 3D impact coordinate in the world
        float hitX = cameraRay.origin[0] + cameraRay.direction[0] * hitDistance;
        float hitY = cameraRay.origin[1] + cameraRay.direction[1] * hitDistance;
        float hitZ = cameraRay.origin[2] + cameraRay.direction[2] * hitDistance;

        // Spawn 30 wooden splinters at the impact site!
        debris.spawnExplosion(hitX, hitY, hitZ, texWood);
        
        std::cout << "[SYSTEM] SHOT FIRED! Desk hit at distance: " << hitDistance << "m" << std::endl;
    }
}

bool Scene::isWalkable(float targetX, float targetZ) const {
    float playerRadius = 0.3f; 

    // Forest boundary (20m)
    if (targetX - playerRadius < -20.0f || targetX + playerRadius > 20.0f) return false;
    if (targetZ - playerRadius < -20.0f || targetZ + playerRadius > 20.0f) return false;

    // A Lambda helper that checks collision exactly like drawThickWall parameters
    auto hitWall = [&](float cx, float cz, float w, float d) {
        float minX = (cx - w/2.0f) - playerRadius; float maxX = (cx + w/2.0f) + playerRadius;
        float minZ = (cz - d/2.0f) - playerRadius; float maxZ = (cz + d/2.0f) + playerRadius;
        return (targetX >= minX && targetX <= maxX && targetZ >= minZ && targetZ <= maxZ);
    };

    // Check all Walls (Including windows, since you can't walk through glass!)
    if (hitWall(-10.0f, -12.0f, 4.0f, 0.5f)) return false; // South 1
    if (hitWall(-7.0f, -12.0f, 2.0f, 0.5f)) return false;  // South Window
    if (hitWall(3.0f, -12.0f, 18.0f, 0.5f)) return false;  // South 2

    if (hitWall(-6.5f, 12.0f, 11.0f, 0.5f)) return false;  // North 1
    if (hitWall(3.0f, 12.0f, 4.0f, 0.5f)) return false;    // North 2 
    if (hitWall(6.0f, 12.0f, 2.0f, 0.5f)) return false;    // North Window 
    if (hitWall(9.5f, 12.0f, 5.0f, 0.5f)) return false;    // North 3 

    if (hitWall(-12.0f, -4.0f, 0.5f, 16.0f)) return false; // West 1
    if (hitWall(-12.0f, 5.0f, 0.5f, 2.0f)) return false;   // West Window
    if (hitWall(-12.0f, 9.0f, 0.5f, 6.0f)) return false;   // West 2

    if (hitWall(12.0f, -10.0f, 0.5f, 4.0f)) return false;  // East 1
    if (hitWall(12.0f, -7.0f, 0.5f, 2.0f)) return false;   // East Window
    if (hitWall(12.0f, 3.0f, 0.5f, 18.0f)) return false;   // East 2

    if (hitWall(-7.0f, -2.0f, 10.0f, 0.5f)) return false;  // Div H 1
    if (hitWall(7.0f, -2.0f, 10.0f, 0.5f)) return false;   // Div H 2

    if (hitWall(-2.0f, -10.0f, 0.5f, 4.0f)) return false;  // Div V L 1
    if (hitWall(-2.0f, -4.0f, 0.5f, 4.0f)) return false;   // Div V L 2

    if (hitWall(2.0f, -10.0f, 0.5f, 4.0f)) return false;   // Div V R 1
    if (hitWall(2.0f, -4.0f, 0.5f, 4.0f)) return false;    // Div V R 2

    // Check collision for ALL 5 Desks
    for (int i = 0; i < 5; i++) {
        float w = 1.6f;
        float d = 0.8f;
        // Swap width and depth if the desk is rotated against the side walls
        if (drawers[i].yaw == 90.0f || drawers[i].yaw == -90.0f) {
            w = 0.8f;
            d = 1.6f;
        }
        if (hitWall(drawers[i].x, drawers[i].z, w, d)) return false;
    }

    if (hitWall(chestX, chestZ, 0.8f, 0.5f)) return false;
    return true; // Safe to walk!
}

bool Scene::tryInteract(const Ray& cameraRay) {
    // Put away active note
    for (int i = 0; i < 5; i++) {
        if (drawers[i].isNoteInspected) {
            drawers[i].isNoteInspected = false;
            return true;
        }
    }

    float closestHit = 9999.0f;
    int hitIndex = -1;
    bool hitNote = false; 

    for (int i = 0; i < 5; i++) {
        // DESK COLLISION (Toggles Open/Close)
        AABB deskBox = { drawers[i].x - 0.8f, 0.0f, drawers[i].z - 0.8f,
                         drawers[i].x + 0.8f, 1.2f, drawers[i].z + 0.8f };
        
        float tEntry;
        if (checkRayAABBIntersection(cameraRay, deskBox, tEntry)) {
            if (tEntry > 0.0f && tEntry < 3.0f && tEntry < closestHit) {
                closestHit = tEntry;
                hitIndex = i;
                hitNote = false;
            }
        }

        // NOTE COLLISION (Only accessible when drawer is slid open!)
        if (drawers[i].isOpen && drawers[i].currentZOffset > 0.3f) {
            // Calculate physical location of the extended drawer based on desk rotation
            float radYaw = drawers[i].yaw * (3.14159265f / 180.0f);
            float dirX = sin(radYaw);
            float dirZ = cos(radYaw);
            
            float noteX = drawers[i].x + (dirX * drawers[i].currentZOffset);
            float noteZ = drawers[i].z + (dirZ * drawers[i].currentZOffset);

            // Small AABB just for the extended drawer
            AABB noteBox = { noteX - 0.6f, 0.2f, noteZ - 0.6f,
                             noteX + 0.6f, 1.2f, noteZ + 0.6f };

            if (checkRayAABBIntersection(cameraRay, noteBox, tEntry)) {
                if (tEntry > 0.0f && tEntry < 3.0f && tEntry < closestHit) {
                    closestHit = tEntry;
                    hitIndex = i;
                    hitNote = true; 
                }
            }
        }
    }

    justPickedUpShotgun = false;

    // CHEST COLLISION
    // Create a generous 1x1 meter hitbox around the chest
    AABB chestBox = { chestX - 0.5f, 0.0f, chestZ - 0.5f,
                      chestX + 0.5f, 1.0f, chestZ + 0.5f };
                      
    if (!isChestUnlocked) {
        AABB chestBox = { chestX - 0.5f, 0.0f, chestZ - 0.5f,
                          chestX + 0.5f, 1.0f, chestZ + 0.5f };
                          
        float tEntryChest;
        if (checkRayAABBIntersection(cameraRay, chestBox, tEntryChest)) {
            if (tEntryChest > 0.0f && tEntryChest < 3.0f && tEntryChest < closestHit) {
                closestHit = tEntryChest;
                hitIndex = 999; // Special ID for the chest
                hitNote = false;
            }
        }
    }

    // SHOTGUN COLLISION (Only accessible if chest is open and weapon isn't taken)
    if (isChestUnlocked && chestLidAngle < -45.0f && !isShotgunCollected) {
        AABB shotgunBox = { chestX - 0.5f, 0.0f, chestZ - 0.5f,
                            chestX + 0.5f, 0.8f, chestZ + 0.5f };
                            
        float tEntryShotgun;
        if (checkRayAABBIntersection(cameraRay, shotgunBox, tEntryShotgun)) {
            if (tEntryShotgun > 0.0f && tEntryShotgun < 3.0f && tEntryShotgun < closestHit) {
                closestHit = tEntryShotgun;
                hitIndex = 888; // Special ID for the Shotgun
                hitNote = false;
            }
        }
    }

    // Process the closest interaction
    if (hitIndex != -1) {
        if (hitIndex == 888) {
            isShotgunCollected = true;
            justPickedUpShotgun = true; // Signal the engine!
        } else if (hitIndex == 999) {
            if (!isChestUnlocked) isChestKeypadActive = true;
        } else if (hitNote) {
            drawers[hitIndex].isNoteInspected = true;
        } else {
            drawers[hitIndex].isOpen = !drawers[hitIndex].isOpen;
        }
        return true;
    }

    return false;
}

void Scene::drawNote() {
    // A simple aged paper sheet
    glDisable(GL_TEXTURE_2D); 
    glColor3f(0.9f, 0.9f, 0.8f); // Slightly yellow/aged white

    glBegin(GL_QUADS);
        glNormal3f(0.0f, 1.0f, 0.0f);
        glVertex3f(-0.15f, 0.0f,  0.2f);
        glVertex3f( 0.15f, 0.0f,  0.2f);
        glVertex3f( 0.15f, 0.0f, -0.2f);
        glVertex3f(-0.15f, 0.0f, -0.2f);
    glEnd();
    
    glColor3f(1.0f, 1.0f, 1.0f); // Reset base color
}

void Scene::renderOverlay() {
    for (int i = 0; i < 5; i++) {
        if (drawers[i].isNoteInspected) {
            // Wipe depth buffer so it overlays everything in the world
            glClear(GL_DEPTH_BUFFER_BIT); 
            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();

            // Disable lighting and textures so the note is bright and readable
            glDisable(GL_LIGHTING);
            glDisable(GL_TEXTURE_2D);

            glPushMatrix();
                // 1. Move it 0.5 meters directly in front of the camera lens
                glTranslatef(0.0f, 0.0f, -0.5f);

                // 2. THE GEOMETRY FIX: Tilt the paper 90 degrees upward so it faces the screen!
                glRotatef(90.0f, 1.0f, 0.0f, 0.0f);

                // 3. Draw the physical sheet of paper
                glScalef(0.4f, 0.02f, 0.5f);
                glColor3f(0.8f, 0.8f, 0.7f); // Aged paper color
                drawSolidCube(1.0f);
            glPopMatrix();

            // Restore engine states for the next frame
            glEnable(GL_TEXTURE_2D);
            glEnable(GL_LIGHTING);
            glColor3f(1.0f, 1.0f, 1.0f);

            // If you kept the Bézier sigil from earlier, you can call it here!
            cursedSigil.render();

            return; // Only draw one note at a time
        }
    }
}

void Scene::initLighting() {
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glShadeModel(GL_SMOOTH);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    setupCabinLights();

    // Enable Color Material so glColor calls interact with our flashlight
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

    // Global ambient set to pitch black for horror atmosphere
    GLfloat globalAmbient[] = { 0.02f, 0.02f, 0.02f, 1.0f };
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globalAmbient);

    std::cout << "[SYSTEM] Scene & Lighting State Initialized." << std::endl;
}

void Scene::loadAssets(){
    //loads the image files from the disk to GPU VRAM
    texFloor = texManager.loadTexture("floor.jpg");
    texWall = texManager.loadTexture("wall.jpg");
    texWood = texManager.loadTexture("wood.jpg");
    texGrass = texManager.loadTexture("grass.jpg");
    texDirt = texManager.loadTexture("dirt.jpg");
    texSky = texManager.loadTexture("sky.jpg");
    std::cout << "[SYSTEM] textures loaded successfully." << std::endl;
}

void Scene::setupTacticalFlashlight() {

    GLfloat lightPos[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    GLfloat lightDir[] = { 0.0f, 0.0f, -1.0f };

    GLfloat diffuseLight[]  = { 0.9f, 0.9f, 0.85f, 1.0f };
    GLfloat specularLight[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat ambientLight[]  = { 0.0f, 0.0f, 0.0f, 1.0f };

    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, lightDir);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);

    glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 35.0f);
    glLightf(GL_LIGHT0, GL_SPOT_EXPONENT, 8.0f);

    glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 1.0f);
    glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.02f);
    glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.005f);
}

float Scene::getTerrainHeight(float x, float z) const {
    // 1. Keep the 40x40m transition foundation completely flat
    if (x >= -20.0f && x <= 20.0f && z >= -20.0f && z <= 20.0f) return 0.0f;
    
    // 2. Keep the 6m dirt road flat
    if (x > -3.5f && x < 3.5f && z >= 20.0f) return 0.0f;

    // 3. Generate organic rolling hills
    float height = sin(x * 0.3f) * cos(z * 0.3f) * 0.8f;
    height += sin(x * 0.7f + z * 0.5f) * 0.3f; 

    // 4. THE BLEND ZONE: Smoothly ramp the hills down to 0 over 5 meters near the foundation!
    float blend = 1.0f;
    float distX = 5.0f, distZ = 5.0f;

    if (x > 20.0f && x < 25.0f) distX = x - 20.0f;
    else if (x < -20.0f && x > -25.0f) distX = -x - 20.0f;

    if (z > 20.0f && z < 25.0f) distZ = z - 20.0f;
    else if (z < -20.0f && z > -25.0f) distZ = -z - 20.0f;

    if (distX < 5.0f || distZ < 5.0f) {
        float minDist = (distX < distZ) ? distX : distZ;
        blend = minDist / 5.0f; // Multiplier scales smoothly from 0.0 to 1.0
    }

    return height * blend;
}

void Scene::drawLowPolyCone(float radius, float height) {
    int slices = 8;
    const float PI = 3.14159265f;
    float angleStep = 2.0f * PI / (float)slices;
    
    glBegin(GL_TRIANGLES);
    for (int i = 0; i < slices; i++) {
        float a1 = i * angleStep;
        float a2 = (i + 1) * angleStep;
        
        float x1 = cos(a1) * radius, z1 = sin(a1) * radius;
        float x2 = cos(a2) * radius, z2 = sin(a2) * radius;
        
        // Calculate slanted surface normal for Gouraud shading
        float nx = (cos(a1) + cos(a2)) / 2.0f;
        float nz = (sin(a1) + sin(a2)) / 2.0f;
        float ny = radius / height; 
        float len = sqrt(nx*nx + ny*ny + nz*nz);
        glNormal3f(nx/len, ny/len, nz/len);
        
        glTexCoord2f(0.5f, 1.0f); glVertex3f(0.0f, height, 0.0f);     // Top Tip
        glTexCoord2f(1.0f, 0.0f); glVertex3f(x2, 0.0f, z2);           // Bottom Right
        glTexCoord2f(0.0f, 0.0f); glVertex3f(x1, 0.0f, z1);           // Bottom Left
    }
    glEnd();
}

void Scene::drawTree(float x, float z, int treeType) {
    // The road exclusion
    if (x > -4.0f && x < 4.0f && z >= 20.0f) return;

    float y = getTerrainHeight(x, z); 
    
    glPushMatrix();
        glTranslatef(x, y, z);
        
        // The Main Trunk (Shared by all trees)
        texManager.bindTexture(texWood);
        glPushMatrix();
            glRotatef(90.0f, 1.0f, 0.0f, 0.0f); 
            drawLowPolyCylinder(0.3f, 3.0f);
        glPopMatrix();
        
        // The Leaves / Branches
        if (treeType == 1) {
            texManager.bindTexture(texGrass);
            glPushMatrix(); glTranslatef(0.0f, 1.5f, 0.0f); drawLowPolyCone(1.5f, 2.5f); glPopMatrix();
            glPushMatrix(); glTranslatef(0.0f, 2.5f, 0.0f); drawLowPolyCone(1.2f, 2.0f); glPopMatrix();
            glPushMatrix(); glTranslatef(0.0f, 3.5f, 0.0f); drawLowPolyCone(0.8f, 1.5f); glPopMatrix();
        } else if (treeType == 2) {
            texManager.bindTexture(texGrass);
            glPushMatrix(); glTranslatef(0.0f, 1.0f, 0.0f); drawLowPolyCone(2.0f, 3.0f); glPopMatrix();
            glPushMatrix(); glTranslatef(0.0f, 2.0f, 0.0f); drawLowPolyCone(1.5f, 2.5f); glPopMatrix();
        } else if (treeType == 3) {
            // THE DEAD TREE (Fixed Branch Attachments)
            texManager.bindTexture(texWood);
            
            // Jagged Top Extension (Attaches exactly to the top of the trunk at Y = 1.5)
            glPushMatrix(); 
                glTranslatef(0.0f, 1.5f, 0.0f); 
                glRotatef(15.0f, 0.0f, 0.0f, 1.0f); // Lean right
                glRotatef(90.0f, 1.0f, 0.0f, 0.0f); // Stand upright
                glTranslatef(0.0f, 0.0f, 0.75f);    // <-- SHIFT BASE TO TRUNK SURFACE
                drawLowPolyCylinder(0.2f, 1.5f); 
            glPopMatrix();

            // Branch 1 (Sticking out to the left)
            glPushMatrix(); 
                glTranslatef(0.0f, 0.5f, 0.0f); 
                glRotatef(-60.0f, 0.0f, 0.0f, 1.0f); 
                glRotatef(90.0f, 1.0f, 0.0f, 0.0f); 
                glTranslatef(0.0f, 0.0f, 0.75f);    // <-- SHIFT BASE TO TRUNK SURFACE
                drawLowPolyCylinder(0.1f, 1.5f); 
            glPopMatrix();

            // Branch 2 (Sticking out forward-right)
            glPushMatrix(); 
                glTranslatef(0.0f, 0.0f, 0.0f); 
                glRotatef(45.0f, 1.0f, 0.0f, 0.0f); 
                glRotatef(45.0f, 0.0f, 0.0f, 1.0f); 
                glRotatef(90.0f, 1.0f, 0.0f, 0.0f); 
                glTranslatef(0.0f, 0.0f, 0.6f);     // <-- SHIFT BASE TO TRUNK SURFACE
                drawLowPolyCylinder(0.08f, 1.2f); 
            glPopMatrix();
        }
    glPopMatrix();
}

void Scene::drawForestTile(float startX, float startZ) {
    // Places 10 distinct trees scattered across the 20x20 meter tile area
    drawTree(startX + 2.0f,  startZ + 3.0f,  1); // Tall Pine
    drawTree(startX + 15.0f, startZ + 4.0f,  2); // Short Pine
    drawTree(startX + 8.0f,  startZ + 8.0f,  1); // Tall Pine
    drawTree(startX + 4.0f,  startZ + 14.0f, 3); // Dead Tree
    drawTree(startX + 12.0f, startZ + 12.0f, 2); // Short Pine
    drawTree(startX + 18.0f, startZ + 16.0f, 1); // Tall Pine
    drawTree(startX + 6.0f,  startZ + 18.0f, 2); // Short Pine
    drawTree(startX + 16.0f, startZ + 9.0f,  3); // Dead Tree
    drawTree(startX + 1.0f,  startZ + 10.0f, 1); // Tall Pine
    drawTree(startX + 10.0f, startZ + 19.0f, 1); // Tall Pine
}

void Scene::drawThickWall(float x, float z, float widthX, float depthZ) {
    float radius = 0.2f; // 20cm radius
    float diameter = radius * 2.0f; // 0.4m thickness
    float wallHeight = 3.2f; // Adjusted to exactly 8 logs
    int numLogs = (int)(wallHeight / diameter); // 8 logs

    // Determine the direction the wall is running
    bool alongX = (widthX > depthZ);
    float length = alongX ? widthX : depthZ;

    glPushMatrix();
        glTranslatef(x, 0.0f, z);
        
        // If the wall runs left-to-right, rotate the Z-aligned cylinders by 90 degrees
        if (alongX) glRotatef(90.0f, 0.0f, 1.0f, 0.0f);

        // A log cabin exclusively uses wood!
        texManager.bindTexture(texWood); 

        // Stack the logs vertically
        for (int i = 0; i < numLogs; i++) {
            glPushMatrix();
                // Offset Y so the first log sits flush against the floor (Y = 0.2m)
                glTranslatef(0.0f, radius + (i * diameter), 0.0f);
                drawLowPolyCylinder(radius, length);
            glPopMatrix();
        }
    glPopMatrix();
}

void Scene::drawLowPolyCylinder(float radius, float length) {
    int slices = 8; // Low poly count to protect framerate!
    const float PI = 3.14159265f;
    float angleStep = 2.0f * PI / (float)slices;

    glBegin(GL_QUADS);
    for (int i = 0; i < slices; i++) {
        float a1 = i * angleStep;
        float a2 = (i + 1) * angleStep;

        // Calculate surface normals pointing strictly outward for the flashlight
        float n1x = cos(a1), n1y = sin(a1);
        float n2x = cos(a2), n2y = sin(a2);

        // UVs: U wraps around the log, V stretches along the length to tile the wood texture
        float u1 = (float)i / slices;
        float u2 = (float)(i + 1) / slices;
        float vMax = length; 

        // Vertex 1 (Bottom-left)
        glNormal3f(n1x, n1y, 0.0f);
        glTexCoord2f(u1, 0.0f);
        glVertex3f(n1x * radius, n1y * radius, -length / 2.0f);

        // Vertex 2 (Bottom-right)
        glNormal3f(n2x, n2y, 0.0f);
        glTexCoord2f(u2, 0.0f);
        glVertex3f(n2x * radius, n2y * radius, -length / 2.0f);

        // Vertex 3 (Top-right)
        glNormal3f(n2x, n2y, 0.0f);
        glTexCoord2f(u2, vMax);
        glVertex3f(n2x * radius, n2y * radius, length / 2.0f);

        // Vertex 4 (Top-left)
        glNormal3f(n1x, n1y, 0.0f);
        glTexCoord2f(u1, vMax);
        glVertex3f(n1x * radius, n1y * radius, length / 2.0f);
    }
    glEnd();

    // Draw End Caps to close the cylinders (prevents seeing inside them at doorways)
    glBegin(GL_POLYGON);
    glNormal3f(0.0f, 0.0f, -1.0f);
    for (int i = slices - 1; i >= 0; i--) {
        float a = i * angleStep;
        glTexCoord2f(cos(a)*0.5f + 0.5f, sin(a)*0.5f + 0.5f);
        glVertex3f(cos(a) * radius, sin(a) * radius, -length / 2.0f);
    }
    glEnd();

    glBegin(GL_POLYGON);
    glNormal3f(0.0f, 0.0f, 1.0f);
    for (int i = 0; i < slices; i++) {
        float a = i * angleStep;
        glTexCoord2f(cos(a)*0.5f + 0.5f, sin(a)*0.5f + 0.5f);
        glVertex3f(cos(a) * radius, sin(a) * radius, length / 2.0f);
    }
    glEnd();
}

void Scene::drawWindowWall(float x, float z, float widthX, float depthZ, float sillHeight, float lintelHeight) {
    float radius = 0.2f;
    float diameter = radius * 2.0f;
    float wallHeight = 3.2f; // Match the 8-log solid walls

    // Snap requested heights to the nearest discrete log (multiples of 0.4m)
    int numSillLogs = (int)(sillHeight / diameter + 0.5f);
    int numLintelLogs = (int)(lintelHeight / diameter + 0.5f);

    float actualSillHeight = numSillLogs * diameter;
    float actualLintelHeight = numLintelLogs * diameter;
    float actualGlassHeight = wallHeight - actualSillHeight - actualLintelHeight;

    bool alongX = (widthX > depthZ);
    float length = alongX ? widthX : depthZ;

    glPushMatrix();
        glTranslatef(x, 0.0f, z);
        if (alongX) glRotatef(90.0f, 0.0f, 1.0f, 0.0f);

        texManager.bindTexture(texWood);

        // 1. Bottom Sill (Stacked Logs)
        for (int i = 0; i < numSillLogs; i++) {
            glPushMatrix();
                glTranslatef(0.0f, radius + (i * diameter), 0.0f);
                drawLowPolyCylinder(radius, length);
            glPopMatrix();
        }

        // 2. Top Lintel (Stacked Logs)
        float lintelStartY = wallHeight - actualLintelHeight;
        for (int i = 0; i < numLintelLogs; i++) {
            glPushMatrix();
                glTranslatef(0.0f, lintelStartY + radius + (i * diameter), 0.0f);
                drawLowPolyCylinder(radius, length);
            glPopMatrix();
        }
    glPopMatrix();

    // 3. Middle Glass Pane
    if (actualGlassHeight > 0.0f) {
        glPushMatrix();
            // Position the glass vertically between the log courses
            glTranslatef(x, actualSillHeight + (actualGlassHeight / 2.0f), z);
            
            // Shrink the glass thickness so it recesses beautifully into the center of the logs
            if (alongX) depthZ = 0.05f; else widthX = 0.05f;
            glScalef(widthX, actualGlassHeight, depthZ);

            glDisable(GL_TEXTURE_2D);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glColor4f(0.2f, 0.4f, 0.5f, 0.5f); // Translucent blue glass

            drawSolidCube(1.0f);

            glDisable(GL_BLEND);
            glEnable(GL_TEXTURE_2D);
            glColor3f(1.0f, 1.0f, 1.0f);
        glPopMatrix();
    }
}

void Scene::drawSolidCube(float size) {
    float half = size / 2.0f;
    glBegin(GL_QUADS);
        // Front Face
        glNormal3f(0.0f, 0.0f, 1.0f);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-half, -half,  half);
        glTexCoord2f(1.0f, 0.0f); glVertex3f( half, -half,  half);
        glTexCoord2f(1.0f, 1.0f); glVertex3f( half,  half,  half);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(-half,  half,  half);
        // Back Face
        glNormal3f(0.0f, 0.0f, -1.0f);
        glTexCoord2f(0.0f, 0.0f); glVertex3f( half, -half, -half);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(-half, -half, -half);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(-half,  half, -half);
        glTexCoord2f(0.0f, 1.0f); glVertex3f( half,  half, -half);
        // Top Face
        glNormal3f(0.0f, 1.0f, 0.0f);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-half,  half,  half);
        glTexCoord2f(1.0f, 0.0f); glVertex3f( half,  half,  half);
        glTexCoord2f(1.0f, 1.0f); glVertex3f( half,  half, -half);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(-half,  half, -half);
        // Bottom Face
        glNormal3f(0.0f, -1.0f, 0.0f);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-half, -half, -half);
        glTexCoord2f(1.0f, 0.0f); glVertex3f( half, -half, -half);
        glTexCoord2f(1.0f, 1.0f); glVertex3f( half, -half,  half);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(-half, -half,  half);
        // Right Face
        glNormal3f(1.0f, 0.0f, 0.0f);
        glTexCoord2f(0.0f, 0.0f); glVertex3f( half, -half,  half);
        glTexCoord2f(1.0f, 0.0f); glVertex3f( half, -half, -half);
        glTexCoord2f(1.0f, 1.0f); glVertex3f( half,  half, -half);
        glTexCoord2f(0.0f, 1.0f); glVertex3f( half,  half,  half);
        // Left Face
        glNormal3f(-1.0f, 0.0f, 0.0f);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-half, -half, -half);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(-half, -half,  half);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(-half,  half,  half);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(-half,  half, -half);
    glEnd();
}

void Scene::drawEnvironment() {
    // 1. THE FOREST FLOOR (20x20 meters tiles)
    glEnable(GL_TEXTURE_2D);
    glColor3f(1.0f, 1.0f, 1.0f);

    float halfWorld = 60.0f; 
    float step = 1.0f;       
    float tileScale = 4.0f; 

    // A. Draw the Wavy Grass (Strictly Outside the 40x40m Foundation)
    texManager.bindTexture(texGrass);
    glBegin(GL_QUADS);    
    for (float x = -halfWorld; x < halfWorld; x += step) {
        bool isRoadColumn = (x >= -3.0f && x < 3.0f);
        // The exclusion zone is exactly mapped to the 40x40m tile space
        bool isInsideFoundation = (x >= -20.0f && x < 20.0f);
        
        for (float z = -halfWorld; z < halfWorld; z += step) {
            if (isRoadColumn && z >= 20.0f) continue;
            if (isInsideFoundation && z >= -20.0f && z < 20.0f) continue; 

            float h1 = getTerrainHeight(x, z + step);
            float h2 = getTerrainHeight(x + step, z + step);
            float h3 = getTerrainHeight(x + step, z);
            float h4 = getTerrainHeight(x, z);

            float nx = h1 - h3; float ny = 2.0f; float nz = h4 - h2;
            float len = sqrt(nx*nx + ny*ny + nz*nz);
            glNormal3f(nx/len, ny/len, nz/len);

            glTexCoord2f(x / tileScale, (z + step) / tileScale);           glVertex3f(x, h1, z + step);
            glTexCoord2f((x + step) / tileScale, (z + step) / tileScale);  glVertex3f(x + step, h2, z + step);
            glTexCoord2f((x + step) / tileScale, z / tileScale);           glVertex3f(x + step, h3, z);
            glTexCoord2f(x / tileScale, z / tileScale);                    glVertex3f(x, h4, z);
        }
    }
    glEnd();

    // B. Draw the Dirt Foundation Transition (40x40m outer, hollow 24x24m center)
    texManager.bindTexture(texDirt);
    glBegin(GL_QUADS);
    glNormal3f(0.0f, 1.0f, 0.0f);
    for (float x = -20.0f; x < 20.0f; x += step) {
        for (float z = -20.0f; z < 20.0f; z += step) {
            // Hollow out the center for the cabin's interior wooden floor (-12 to 12)
            if (x >= -12.0f && x < 12.0f && z >= -12.0f && z < 12.0f) continue;
            
            glTexCoord2f(x / 2.0f, (z + step) / 2.0f);           glVertex3f(x, 0.0f, z + step);
            glTexCoord2f((x + step) / 2.0f, (z + step) / 2.0f);  glVertex3f(x + step, 0.0f, z + step);
            glTexCoord2f((x + step) / 2.0f, z / 2.0f);           glVertex3f(x + step, 0.0f, z);
            glTexCoord2f(x / 2.0f, z / 2.0f);                    glVertex3f(x, 0.0f, z);
        }
    }
    glEnd();

    // C. Draw the Dirt Road (Extending from the Foundation edge at Z=20.0f)
    glBegin(GL_QUADS);
    for (float x = -3.0f; x < 3.0f; x += step) {
        for (float z = 20.0f; z < 80.0f; z += step) {
            float rH1 = cos((x / 3.0f) * 1.57f) * 0.1f;
            float rH2 = cos(((x + step) / 3.0f) * 1.57f) * 0.1f;

            glNormal3f(0.0f, 1.0f, 0.0f); 
            glTexCoord2f(x / 2.0f, (z + step) / 2.0f);           glVertex3f(x, rH1, z + step);
            glTexCoord2f((x + step) / 2.0f, (z + step) / 2.0f);  glVertex3f(x + step, rH2, z + step);
            glTexCoord2f((x + step) / 2.0f, z / 2.0f);           glVertex3f(x + step, rH2, z);
            glTexCoord2f(x / 2.0f, z / 2.0f);                    glVertex3f(x, rH1, z);
        }
    }
    glEnd();
    
    // D. Draw the Cabin's Interior Wooden Floor (X: -12 to 12, Z: -12 to 12)
    float floorStep = 0.5f; 
    texManager.bindTexture(texFloor);
    glBegin(GL_QUADS);
    glNormal3f(0.0f, 1.0f, 0.0f);
    for (float x = -12.0f; x < 12.0f; x += floorStep) {
        for (float z = -12.0f; z < 12.0f; z += floorStep) {
            glTexCoord2f(x / 2.0f, (z + floorStep) / 2.0f);                glVertex3f(x, 0.0f, z + floorStep);
            glTexCoord2f((x + floorStep) / 2.0f, (z + floorStep) / 2.0f);  glVertex3f(x + floorStep, 0.0f, z + floorStep);
            glTexCoord2f((x + floorStep) / 2.0f, z / 2.0f);                glVertex3f(x + floorStep, 0.0f, z);
            glTexCoord2f(x / 2.0f, z / 2.0f);                              glVertex3f(x, 0.0f, z);
        }
    }
    glEnd();

    // E. Tile the Forest Trees around the Cabin
    for (float tX = -40.0f; tX <= 40.0f; tX += 20.0f) {
        for (float tZ = -40.0f; tZ <= 80.0f; tZ += 20.0f) {
            // BULLETPROOF GRID EXCLUSION:
            // Skip the exact 4 tiles that occupy the 40x40m transition foundation
            if (tX >= -20.0f && tX < 20.0f && tZ >= -20.0f && tZ < 20.0f) continue;
            
            drawForestTile(tX, tZ);
        }
    }

    // ==========================================
    // 2. EXTERNAL WALLS (PERIMETER)
    // ==========================================

    texManager.bindTexture(texWall);
    
    // South Wall (Y=0 -> OpenGL Z = -12)
    drawThickWall(-10.0f, -12.0f, 4.0f, 0.5f);  // X=0 to 4
    drawWindowWall(-7.0f, -12.0f, 2.0f, 0.5f);  // Window X=4 to 6
    drawThickWall(3.0f, -12.0f, 18.0f, 0.5f);   // X=6 to 24

    // North Wall (Y=24, Z = 12) (Main Entrance)
    drawThickWall(-6.5f, 12.0f, 11.0f, 0.5f); // Left Wall (X = -12 to -1)
    // Door X=10 to 12 (Empty)
    drawThickWall(3.0f, 12.0f, 4.0f, 0.5f);   // Middle Wall (X = 1 to 5)
    drawWindowWall(6.0f, 12.0f, 2.0f, 0.5f, 0.875f, 0.875f); // Window (X = 5 to 7)
    drawThickWall(9.5f, 12.0f, 5.0f, 0.5f);   // Right Wall (X = 7 to 12)

    // West Wall (X=0 -> OpenGL X = -12)
    drawThickWall(-12.0f, -4.0f, 0.5f, 16.0f);  // Y=0 to 16
    drawWindowWall(-12.0f, 5.0f, 0.5f, 2.0f);   // Window Y=16 to 18
    drawThickWall(-12.0f, 9.0f, 0.5f, 6.0f);    // Y=18 to 24

    // East Wall (X=24 -> OpenGL X = 12)
    drawThickWall(12.0f, -10.0f, 0.5f, 4.0f);   // Y=0 to 4
    drawWindowWall(12.0f, -7.0f, 0.5f, 2.0f);   // Window Y=4 to 6
    drawThickWall(12.0f, 3.0f, 0.5f, 18.0f);    // Y=6 to 24

    // ==========================================
    // 3. INTERNAL WALLS (PARTITIONS)
    // ==========================================
    
    // Horizontal Partition (Y=10 -> OpenGL Z = -2)
    drawThickWall(-7.0f, -2.0f, 10.0f, 0.5f);   // Left Bedroom (X=0 to 10)
    // Central Hallway X=10 to 14 (Empty)
    drawThickWall(7.0f, -2.0f, 10.0f, 0.5f);    // Right Bedroom (X=14 to 24)

    // Left Vertical Partition (X=10 -> OpenGL X = -2)
    drawThickWall(-2.0f, -10.0f, 0.5f, 4.0f);   // Y=0 to 4
    // Door Y=4 to 6 (Empty)
    drawThickWall(-2.0f, -4.0f, 0.5f, 4.0f);    // Y=6 to 10

    // Right Vertical Partition (X=14 -> OpenGL X = 2)
    drawThickWall(2.0f, -10.0f, 0.5f, 4.0f);    // Y=0 to 4
    // Door Y=4 to 6 (Empty)
    drawThickWall(2.0f, -4.0f, 0.5f, 4.0f);     // Y=6 to 10

    // ==========================================
    // 4. THE CABIN ROOF (25x25 for a slight overhang)
    // ==========================================
    glPushMatrix();
        glTranslatef(0.0f, 3.2f, 0.0f); // Rest perfectly on top of the 8-log walls
        
        texManager.bindTexture(texWood); 

        float halfW = 12.5f; // 25m total width (gives a 0.5m overhang on the sides)
        float halfD = 12.5f; // 25m total depth
        float roofH = 4.0f;  // 2m tall triangular peak
        
        // Calculate the surface normals for the slanted roof using Pythagoras
        float normalLen = sqrt((roofH * roofH) + (halfW * halfW));
        float nx = roofH / normalLen; 
        float ny = halfW / normalLen;
        
        float tile = 2.0f; // Tile the texture every 2 meters
        float slantLen = normalLen; // The physical length of the slanted face

        glBegin(GL_QUADS);
            // CEILING (Faces downward to seal the interior from the void)
            glNormal3f(0.0f, -1.0f, 0.0f);
            glTexCoord2f(-halfW/tile, -halfD/tile); glVertex3f(-halfW, 0.0f, -halfD);
            glTexCoord2f( halfW/tile, -halfD/tile); glVertex3f( halfW, 0.0f, -halfD);
            glTexCoord2f( halfW/tile,  halfD/tile); glVertex3f( halfW, 0.0f,  halfD);
            glTexCoord2f(-halfW/tile,  halfD/tile); glVertex3f(-halfW, 0.0f,  halfD);
            
            // LEFT SLANTED ROOF (Facing -X / +Y)
            glNormal3f(-nx, ny, 0.0f);
            glTexCoord2f(-halfD/tile, 0.0f);          glVertex3f(-halfW, 0.0f, -halfD);
            glTexCoord2f( halfD/tile, 0.0f);          glVertex3f(-halfW, 0.0f,  halfD);
            glTexCoord2f( halfD/tile, slantLen/tile); glVertex3f(  0.0f, roofH, halfD);
            glTexCoord2f(-halfD/tile, slantLen/tile); glVertex3f(  0.0f, roofH,-halfD);
            
            // RIGHT SLANTED ROOF (Facing +X / +Y)
            glNormal3f(nx, ny, 0.0f);
            glTexCoord2f(-halfD/tile, 0.0f);          glVertex3f( halfW, 0.0f,  halfD);
            glTexCoord2f( halfD/tile, 0.0f);          glVertex3f( halfW, 0.0f, -halfD);
            glTexCoord2f( halfD/tile, slantLen/tile); glVertex3f(  0.0f, roofH,-halfD);
            glTexCoord2f(-halfD/tile, slantLen/tile); glVertex3f(  0.0f, roofH, halfD);
        glEnd();
        
        glBegin(GL_TRIANGLES);
            // FRONT GABLE TRIANGLE (Facing North / +Z)
            glNormal3f(0.0f, 0.0f, 1.0f);
            glTexCoord2f(-halfW/tile, 0.0f);       glVertex3f(-halfW, 0.0f, halfD);
            glTexCoord2f( halfW/tile, 0.0f);       glVertex3f( halfW, 0.0f, halfD);
            glTexCoord2f( 0.0f,       roofH/tile); glVertex3f(  0.0f, roofH, halfD);
            
            // BACK GABLE TRIANGLE (Facing South / -Z)
            glNormal3f(0.0f, 0.0f, -1.0f);
            glTexCoord2f( halfW/tile, 0.0f);       glVertex3f( halfW, 0.0f, -halfD);
            glTexCoord2f(-halfW/tile, 0.0f);       glVertex3f(-halfW, 0.0f, -halfD);
            glTexCoord2f( 0.0f,       roofH/tile); glVertex3f(  0.0f, roofH,-halfD);
        glEnd();
        
    glPopMatrix();
    
    glDisable(GL_TEXTURE_2D);
}

void Scene::drawPuzzleDrawer(PuzzleDrawer& drawer) {
    glEnable(GL_TEXTURE_2D);
    texManager.bindTexture(texWood);
    glColor3f(1.0f, 1.0f, 1.0f);

    glPushMatrix();
        glTranslatef(drawer.x, 0.0f, drawer.z);
        glRotatef(drawer.yaw, 0.0f, 1.0f, 0.0f); 
        glTranslatef(0.0f, 0.8f, 0.0f);

        // Desk Frame
        glPushMatrix();
            glScalef(1.6f, 0.1f, 0.8f);
            drawSolidCube(1.0f);
        glPopMatrix();

        // Desk Legs
        for(float x : {-0.75f, 0.75f}) {
            for(float z : {-0.35f, 0.35f}) {
                glPushMatrix();
                    glTranslatef(x, -0.4f, z);
                    glScalef(0.1f, 0.8f, 0.1f);
                    drawSolidCube(1.0f);
                glPopMatrix();
            }
        }

        // Interactive Drawer Child Node
        glPushMatrix();
            glTranslatef(0.0f, -0.2f, drawer.currentZOffset);
            
            // 5-PANEL HOLLOW DRAWER (Replaces the solid slab)
            glPushMatrix(); glTranslatef(0.0f, -0.1f, 0.0f); glScalef(0.7f, 0.05f, 0.7f); drawSolidCube(1.0f); glPopMatrix(); // Floor
            glPushMatrix(); glTranslatef(0.0f, 0.0f, 0.325f); glScalef(0.8f, 0.25f, 0.05f); drawSolidCube(1.0f); glPopMatrix(); // Front
            glPushMatrix(); glTranslatef(0.0f, 0.0f, -0.325f); glScalef(0.7f, 0.2f, 0.05f); drawSolidCube(1.0f); glPopMatrix(); // Back
            glPushMatrix(); glTranslatef(-0.35f, 0.0f, 0.0f); glScalef(0.05f, 0.2f, 0.6f); drawSolidCube(1.0f); glPopMatrix(); // Left
            glPushMatrix(); glTranslatef(0.35f, 0.0f, 0.0f); glScalef(0.05f, 0.2f, 0.6f); drawSolidCube(1.0f); glPopMatrix(); // Right

            // The Note
            if (!drawer.isNoteInspected) {
                glDisable(GL_TEXTURE_2D);
                glPushMatrix();
                    glTranslatef(0.0f, -0.07f, 0.0f); // Hover just above the new bottom floor
                    glScalef(0.4f, 0.02f, 0.5f);
                    glColor3f(0.8f, 0.8f, 0.7f); // Aged paper color
                    drawSolidCube(1.0f);
                    glColor3f(1.0f, 1.0f, 1.0f); // Reset color
                glPopMatrix();
                glEnable(GL_TEXTURE_2D);
            }
        glPopMatrix();
        
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
}

void Scene::drawSkybox(float camX, float camY, float camZ) {
    glPushMatrix();
        // 1. Lock the center of the skydome to the camera's exact position
        glTranslatef(camX, camY, camZ);
        
        // 2. Disable lighting and depth writing
        glDisable(GL_LIGHTING);
        glDepthMask(GL_FALSE); 
        
        glEnable(GL_TEXTURE_2D);
        texManager.bindTexture(texSky);
        glColor3f(1.0f, 1.0f, 1.0f); 
        
        // 3. Generate a 50m radius Skydome using Spherical Coordinates
        float radius = 50.0f; 
        int slices = 24; // Horizontal segments (longitude)
        int stacks = 16; // Vertical segments (latitude)
        const float PI = 3.14159265f;
        
        // Loop through the vertical stacks
        for (int i = 0; i < stacks; i++) {
            float phi1 = PI * (float)i / stacks;
            float phi2 = PI * (float)(i + 1) / stacks;
            
            glBegin(GL_QUAD_STRIP);
            // Loop through the horizontal slices to build the dome surface
            for (int j = 0; j <= slices; j++) {
                float theta = 2.0f * PI * (float)j / slices;
                
                // Map U smoothly from 0.0 to 1.0 around the sphere to eliminate tiling seams!
                float u = (float)j / slices;
                
                // Top Vertex
                float v1 = 1.0f - (float)i / stacks; // Map V from top to bottom
                float x1 = radius * sin(phi1) * cos(theta);
                float y1 = radius * cos(phi1);
                float z1 = radius * sin(phi1) * sin(theta);
                glTexCoord2f(u, v1);
                glVertex3f(x1, y1, z1);
                
                // Bottom Vertex
                float v2 = 1.0f - (float)(i + 1) / stacks;
                float x2 = radius * sin(phi2) * cos(theta);
                float y2 = radius * cos(phi2);
                float z2 = radius * sin(phi2) * sin(theta);
                glTexCoord2f(u, v2);
                glVertex3f(x2, y2, z2);
            }
            glEnd();
        }
        
        // 4. Restore states
        glDepthMask(GL_TRUE);
        glEnable(GL_LIGHTING);
    glPopMatrix();
}

void Scene::drawChest() {
    glPushMatrix();
        glTranslatef(chestX, 0.0f, chestZ);
        glRotatef(chestYaw, 0.0f, 1.0f, 0.0f);

        // 1. The Hollow Chest Base (5 Panels)
        glEnable(GL_TEXTURE_2D);
        texManager.bindTexture(texWood);
        glColor3f(0.6f, 0.4f, 0.3f); 
        
        glPushMatrix();
            glTranslatef(0.0f, 0.25f, 0.0f); // Center of the base
            
            // Floor
            glPushMatrix(); glTranslatef(0.0f, -0.225f, 0.0f); glScalef(0.8f, 0.05f, 0.5f); drawSolidCube(1.0f); glPopMatrix();
            // Left Wall
            glPushMatrix(); glTranslatef(-0.375f, 0.0f, 0.0f); glScalef(0.05f, 0.5f, 0.5f); drawSolidCube(1.0f); glPopMatrix();
            // Right Wall
            glPushMatrix(); glTranslatef(0.375f, 0.0f, 0.0f); glScalef(0.05f, 0.5f, 0.5f); drawSolidCube(1.0f); glPopMatrix();
            // Front Wall
            glPushMatrix(); glTranslatef(0.0f, 0.0f, 0.225f); glScalef(0.7f, 0.5f, 0.05f); drawSolidCube(1.0f); glPopMatrix();
            // Back Wall
            glPushMatrix(); glTranslatef(0.0f, 0.0f, -0.225f); glScalef(0.7f, 0.5f, 0.05f); drawSolidCube(1.0f); glPopMatrix();
        glPopMatrix();

        // 2. The Hinged Lid
        glPushMatrix();
            glTranslatef(0.0f, 0.5f, -0.25f);
            glRotatef(chestLidAngle, 1.0f, 0.0f, 0.0f);
            glTranslatef(0.0f, 0.1f, 0.25f);
            glScalef(0.8f, 0.2f, 0.5f);
            drawSolidCube(1.0f);
        glPopMatrix();

        // 3. The Electronic Keypad
        glDisable(GL_TEXTURE_2D);
        glColor3f(0.2f, 0.2f, 0.2f); 
        glPushMatrix();
            glTranslatef(0.0f, 0.35f, 0.26f); 
            glScalef(0.15f, 0.2f, 0.05f);
            drawSolidCube(1.0f);
        glPopMatrix();
        
        // 4. The Contents (Only visible when unlocked)
        if (isChestUnlocked) {
            // A. The Pump-Action Shotgun
            if (!isShotgunCollected) {
                glPushMatrix();
                    glTranslatef(0.0f, 0.05f, 0.0f); // Rest flat on the chest floor
                    glRotatef(15.0f, 0.0f, 1.0f, 0.0f); // Angle it slightly for aesthetic presentation

                    // Grip & Shoulder Stock
                    glColor3f(0.25f, 0.12f, 0.08f); // Dark Wood
                    glPushMatrix(); 
                        glTranslatef(-0.15f, 0.02f, 0.0f); 
                        glRotatef(-15.0f, 0.0f, 0.0f, 1.0f); // Slant the stock downward
                        glScalef(0.18f, 0.06f, 0.04f); 
                        drawSolidCube(1.0f); 
                    glPopMatrix();

                    // Main Receiver
                    glColor3f(0.15f, 0.15f, 0.15f); // Gunmetal
                    glPushMatrix(); 
                        glTranslatef(0.0f, 0.04f, 0.0f); 
                        glScalef(0.15f, 0.07f, 0.045f); 
                        drawSolidCube(1.0f); 
                    glPopMatrix();

                    // Pump Handle
                    glColor3f(0.25f, 0.12f, 0.08f); // Wood
                    glPushMatrix(); 
                        glTranslatef(0.15f, 0.02f, 0.0f); 
                        glScalef(0.12f, 0.03f, 0.05f); 
                        drawSolidCube(1.0f); 
                    glPopMatrix();

                    // Barrel
                    glColor3f(0.1f, 0.1f, 0.1f); // Dark Metal
                    glPushMatrix(); 
                        glTranslatef(0.2f, 0.05f, 0.0f); 
                        glRotatef(90.0f, 0.0f, 0.0f, 1.0f); 
                        drawLowPolyCylinder(0.015f, 0.35f); 
                    glPopMatrix();
                glPopMatrix();
            }

            // B. The Blood Text on the inside of the Lid
            glDisable(GL_LIGHTING); 
            glColor3f(0.8f, 0.0f, 0.0f); 
            glLineWidth(4.0f);
            glPushMatrix();
                glTranslatef(0.0f, 0.5f, -0.25f);
                glRotatef(chestLidAngle, 1.0f, 0.0f, 0.0f);
                glTranslatef(0.0f, -0.01f, 0.25f); 
                
                // THE FIX: Scaling Z by -0.2f mathematically flips the top and bottom of the letters!
                glScalef(0.2f, 0.2f, -0.2f);
                
                // Explicit coordinates for "SURVIVE"
                glBegin(GL_LINES);
                    // S
                    glVertex3f(-1.8f, 0.0f, -0.5f); glVertex3f(-1.4f, 0.0f, -0.5f);
                    glVertex3f(-1.8f, 0.0f, -0.5f); glVertex3f(-1.8f, 0.0f, 0.0f);
                    glVertex3f(-1.8f, 0.0f, 0.0f);  glVertex3f(-1.4f, 0.0f, 0.0f);
                    glVertex3f(-1.4f, 0.0f, 0.0f);  glVertex3f(-1.4f, 0.0f, 0.5f);
                    glVertex3f(-1.4f, 0.0f, 0.5f);  glVertex3f(-1.8f, 0.0f, 0.5f);
                    // U
                    glVertex3f(-1.2f, 0.0f, -0.5f); glVertex3f(-1.2f, 0.0f, 0.5f);
                    glVertex3f(-1.2f, 0.0f, 0.5f);  glVertex3f(-0.8f, 0.0f, 0.5f);
                    glVertex3f(-0.8f, 0.0f, 0.5f);  glVertex3f(-0.8f, 0.0f, -0.5f);
                    // R
                    glVertex3f(-0.6f, 0.0f, -0.5f); glVertex3f(-0.6f, 0.0f, 0.5f);
                    glVertex3f(-0.6f, 0.0f, -0.5f); glVertex3f(-0.2f, 0.0f, -0.5f);
                    glVertex3f(-0.2f, 0.0f, -0.5f); glVertex3f(-0.2f, 0.0f, 0.0f);
                    glVertex3f(-0.2f, 0.0f, 0.0f);  glVertex3f(-0.6f, 0.0f, 0.0f);
                    glVertex3f(-0.6f, 0.0f, 0.0f);  glVertex3f(-0.2f, 0.0f, 0.5f);
                    // V
                    glVertex3f(0.0f, 0.0f, -0.5f);  glVertex3f(0.2f, 0.0f, 0.5f);
                    glVertex3f(0.2f, 0.0f, 0.5f);   glVertex3f(0.4f, 0.0f, -0.5f);
                    // I
                    glVertex3f(0.6f, 0.0f, -0.5f);  glVertex3f(0.8f, 0.0f, -0.5f);
                    glVertex3f(0.7f, 0.0f, -0.5f);  glVertex3f(0.7f, 0.0f, 0.5f);
                    glVertex3f(0.6f, 0.0f, 0.5f);   glVertex3f(0.8f, 0.0f, 0.5f);
                    // V
                    glVertex3f(1.0f, 0.0f, -0.5f);  glVertex3f(1.2f, 0.0f, 0.5f);
                    glVertex3f(1.2f, 0.0f, 0.5f);   glVertex3f(1.4f, 0.0f, -0.5f);
                    // E
                    glVertex3f(1.6f, 0.0f, -0.5f);  glVertex3f(1.6f, 0.0f, 0.5f);
                    glVertex3f(1.6f, 0.0f, -0.5f);  glVertex3f(2.0f, 0.0f, -0.5f);
                    glVertex3f(1.6f, 0.0f, 0.0f);   glVertex3f(1.9f, 0.0f, 0.0f);
                    glVertex3f(1.6f, 0.0f, 0.5f);   glVertex3f(2.0f, 0.0f, 0.5f);
                glEnd();
            glPopMatrix();
            glLineWidth(1.0f);
            glEnable(GL_LIGHTING);
        }

        glColor3f(1.0f, 1.0f, 1.0f);
        glEnable(GL_TEXTURE_2D);
    glPopMatrix();
}

void Scene::drawViewModel() {
    if (!isShotgunCollected) return;

    // Clear depth so the gun never clips into walls!
    glClear(GL_DEPTH_BUFFER_BIT); 
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Position the gun in the bottom-right corner of the screen
    glTranslatef(0.15f, -0.15f, -0.4f); 
    glRotatef(5.0f, 0.0f, 1.0f, 0.0f); // Angle it slightly toward the center

    // Enable lighting for dynamic flashlight reflections on the metal
    glEnable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);

    // Reuse our 4-part shotgun geometry
    glPushMatrix();
        glScalef(0.6f, 0.6f, 0.6f); 

        // Grip & Shoulder Stock
        glColor3f(0.25f, 0.12f, 0.08f); 
        glPushMatrix(); glTranslatef(-0.15f, 0.02f, 0.0f); glRotatef(-15.0f, 0.0f, 0.0f, 1.0f); glScalef(0.18f, 0.06f, 0.04f); drawSolidCube(1.0f); glPopMatrix();

        // Main Receiver
        glColor3f(0.15f, 0.15f, 0.15f); 
        glPushMatrix(); glTranslatef(0.0f, 0.04f, 0.0f); glScalef(0.15f, 0.07f, 0.045f); drawSolidCube(1.0f); glPopMatrix();

        // Pump Handle
        glColor3f(0.25f, 0.12f, 0.08f); 
        glPushMatrix(); glTranslatef(0.15f, 0.02f, 0.0f); glScalef(0.12f, 0.03f, 0.05f); drawSolidCube(1.0f); glPopMatrix();

        // Barrel
        glColor3f(0.1f, 0.1f, 0.1f); 
        glPushMatrix(); glTranslatef(0.2f, 0.05f, 0.0f); glRotatef(90.0f, 0.0f, 0.0f, 1.0f); drawLowPolyCylinder(0.015f, 0.35f); glPopMatrix();
    glPopMatrix();
    
    glColor3f(1.0f, 1.0f, 1.0f);
    glEnable(GL_TEXTURE_2D);
}

void Scene::setupCabinLights() {
    // Warm, dim bulb colors
    GLfloat lightDiffuse[] = { 0.7f, 0.6f, 0.4f, 1.0f }; 
    GLfloat lightAmbient[] = { 0.05f, 0.05f, 0.05f, 1.0f }; // Very faint ambient glow
    
    // Configure lights 1 through 4
    for (int i = GL_LIGHT1; i <= GL_LIGHT4; ++i) {
        glLightfv(i, GL_DIFFUSE, lightDiffuse);
        glLightfv(i, GL_AMBIENT, lightAmbient);
        
        // Attenuation math: 1 / (kc + kl*d + kq*d^2)
        glLightf(i, GL_CONSTANT_ATTENUATION, 1.0f);
        glLightf(i, GL_LINEAR_ATTENUATION, 0.14f);
        glLightf(i, GL_QUADRATIC_ATTENUATION, 0.07f);
        
        glEnable(i);
    }
}

void Scene::positionCabinLights() {
    // Coordinates match the center of our 4 new rooms at a height of 2.5m (near the ceiling)
    GLfloat posLivingRoom[]   = {  0.0f, 2.5f,  5.0f, 1.0f }; // Front Right (GL_LIGHT1)
    GLfloat posLeftRoom[] = { -7.0f, 2.5f, -7.0f, 1.0f }; // Front Left  (GL_LIGHT2)
    GLfloat posRightRoom[]  = {  7.0f, 2.5f, -7.0f, 1.0f }; // Back Right  (GL_LIGHT3)
    GLfloat posHallway[]  = {  0.0f, 2.5f, -7.0f, 1.0f }; // Back Left   (GL_LIGHT4)
    
    glLightfv(GL_LIGHT1, GL_POSITION, posLivingRoom);
    glLightfv(GL_LIGHT2, GL_POSITION, posLeftRoom);
    glLightfv(GL_LIGHT3, GL_POSITION, posRightRoom);
    glLightfv(GL_LIGHT4, GL_POSITION, posHallway);
}

void Scene::renderKeypadUI(int width, int height) {
    if (!isChestKeypadActive) return;

    // Switch to 2D Ortho
    glDisable(GL_DEPTH_TEST); glDisable(GL_LIGHTING); glDisable(GL_TEXTURE_2D);
    glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
    glOrtho(0, width, height, 0, -1, 1);
    glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity();

    // Dark background panel
    float cx = width / 2.0f, cy = height / 2.0f;
    glColor4f(0.1f, 0.1f, 0.15f, 0.9f);
    glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBegin(GL_QUADS);
        glVertex2f(cx - 100, cy - 40); glVertex2f(cx + 100, cy - 40);
        glVertex2f(cx + 100, cy + 40); glVertex2f(cx - 100, cy + 40);
    glEnd();
    glDisable(GL_BLEND);

    // Draw the digital numbers as crude glowing lines
    glColor3f(0.0f, 1.0f, 0.2f); // Neon Green
    glLineWidth(3.0f);
    
    float startX = cx - 60.0f;
    for (int i = 0; i < 4; i++) {
        // Draw an underscore for empty slots
        if (i >= currentCode.length()) {
            glBegin(GL_LINES); glVertex2f(startX + (i * 35), cy + 15); glVertex2f(startX + 20 + (i * 35), cy + 15); glEnd();
        } else {
            // Draw a vertical tally mark for each typed number (Simulating an encrypted screen)
            glBegin(GL_LINES); glVertex2f(startX + 10 + (i * 35), cy - 15); glVertex2f(startX + 10 + (i * 35), cy + 15); glEnd();
        }
    }
    glLineWidth(1.0f);

    // Restore 3D
    glPopMatrix(); glMatrixMode(GL_PROJECTION); glPopMatrix(); glMatrixMode(GL_MODELVIEW);
    glEnable(GL_DEPTH_TEST); glEnable(GL_LIGHTING); glEnable(GL_TEXTURE_2D);
}

void Scene::render(float camX, float camY, float camZ) {
    drawSkybox(camX, camY, camZ); // 1. Draw the skybox first (at infinity)
    
    positionCabinLights();        // 2. Lock dynamic lights
    drawEnvironment();            // 3. Draw the 3D world geometry
    // 4. Draw interactive props
    for (int i = 0; i < 5; i++) {
        drawPuzzleDrawer(drawers[i]);
    }       
    drawChest();
    debris.render();              // 5. Draw shotgun physics
}