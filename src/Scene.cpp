#include "Scene.h"

Scene::Scene() : texFloor(0), texWall(0), texWood(0), isDrawerOpen(false), currentDrawerZ(0.0f), isInspectingNote(false) {}

void Scene::updatePhysics(float deltaTime) {
    // Smoothly animate the drawer sliding open (to 0.5f) or closed (to 0.0f)
    float targetZ = isDrawerOpen ? 0.5f : 0.0f;
    currentDrawerZ += (targetZ - currentDrawerZ) * 5.0f * deltaTime;
}

bool Scene::tryInteract(const Ray& cameraRay) {
    // 1. If we are currently reading the note, 'E' puts it away
    if (isInspectingNote) {
        isInspectingNote = false;
        std::cout << "[SYSTEM] Put note away." << std::endl;
        return true;
    }

    float hitDistance = 0.0f;

    // 2. If the drawer is open, check if we clicked the note inside it
    if (isDrawerOpen) {
        float noteWorldX = 2.0f;
        float noteWorldY = 0.8f - 0.2f - 0.1f; // Desk height - drawer offset - note offset
        float noteWorldZ = -3.5f + currentDrawerZ;

        AABB noteBox;
        noteBox.min[0] = noteWorldX - 0.2f; noteBox.max[0] = noteWorldX + 0.2f;
        noteBox.min[1] = noteWorldY - 0.05f; noteBox.max[1] = noteWorldY + 0.05f;
        noteBox.min[2] = noteWorldZ - 0.3f; noteBox.max[2] = noteWorldZ + 0.3f;

        if (checkRayAABBIntersection(cameraRay, noteBox, hitDistance) && hitDistance <= 3.0f) {
            isInspectingNote = true;
            std::cout << "[SYSTEM] Inspecting Note!" << std::endl;
            return true; // Stop here so we don't also accidentally close the drawer!
        }
    }

    // 3. Fallback: Check if we clicked the drawer itself
    float worldCenterX = 2.0f;
    float worldCenterY = 0.8f - 0.2f;
    float worldCenterZ = -3.5f + currentDrawerZ;

    AABB drawerBox;
    drawerBox.min[0] = worldCenterX - 0.4f; drawerBox.max[0] = worldCenterX + 0.4f;
    drawerBox.min[1] = worldCenterY - 0.125f; drawerBox.max[1] = worldCenterY + 0.125f;
    drawerBox.min[2] = worldCenterZ - 0.35f; drawerBox.max[2] = worldCenterZ + 0.35f;

    if (checkRayAABBIntersection(cameraRay, drawerBox, hitDistance) && hitDistance <= 3.0f) {
        isDrawerOpen = !isDrawerOpen;
        std::cout << "[SYSTEM] Drawer Interacted! Distance: " << hitDistance << "m" << std::endl;
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
    if (!isInspectingNote) return;

    // THE MAGIC TRICK: Wipe the depth buffer so the note renders over everything
    glClear(GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
        // Reset to Eye Space (Camera coordinates)
        glLoadIdentity(); 

        // Move 0.6 meters directly in front of the camera lens, lowered slightly
        glTranslatef(0.0f, -0.1f, -0.6f);

        // Tilt the paper up 60 degrees to face the reader
        glRotatef(60.0f, 1.0f, 0.0f, 0.0f);
        
        // Add a slight natural tilt (like holding it in one hand)
        glRotatef(-5.0f, 0.0f, 0.0f, 1.0f);

        drawNote();
    glPopMatrix();
}

void Scene::initLighting() {
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glShadeModel(GL_SMOOTH);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

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

void Scene::drawAbandonedRoom() {
    float halfWidth = 5.0f;
    float height = 4.0f;
    float halfLength = 5.0f;
    float step = 0.5f;
    float tileScale = 2.0f;

    glEnable(GL_TEXTURE_2D);
    glColor3f(1.0f, 1.0f, 1.0f);

    // --- Floor ---
    texManager.bindTexture(texFloor);
    glBegin(GL_QUADS);    
    glNormal3f(0.0f, 1.0f, 0.0f);
    for (float x = -halfWidth; x < halfWidth; x += step) {
        for (float z = -halfLength; z < halfLength; z += step) {
            glTexCoord2f(x / tileScale, (z + step) / tileScale);           glVertex3f(x, 0.0f, z + step);
            glTexCoord2f((x + step) / tileScale, (z + step) / tileScale);  glVertex3f(x + step, 0.0f, z + step);
            glTexCoord2f((x + step) / tileScale, z / tileScale);           glVertex3f(x + step, 0.0f, z);
            glTexCoord2f(x / tileScale, z / tileScale);                    glVertex3f(x, 0.0f, z);
        }
    }

    // --- CEILING & WALLS ---
    texManager.bindTexture(texWall);
    glBegin(GL_QUADS);

    //celling
    glNormal3f(0.0f, -1.0f, 0.0f);
    for (float x = -halfWidth; x < halfWidth; x += step) {
        for (float z = -halfLength; z < halfLength; z += step) {
            glTexCoord2f(x / tileScale, z / tileScale);                    glVertex3f(x, height, z);
            glTexCoord2f((x + step) / tileScale, z / tileScale);           glVertex3f(x + step, height, z);
            glTexCoord2f((x + step) / tileScale, (z + step) / tileScale);  glVertex3f(x + step, height, z + step);
            glTexCoord2f(x / tileScale, (z + step) / tileScale);           glVertex3f(x, height, z + step);
        }
    }

    // Back Wall
    glNormal3f(0.0f, 0.0f, 1.0f);
    for (float x = -halfWidth; x < halfWidth; x += step) {
        for (float y = 0.0f; y < height; y += step) {
            glTexCoord2f(x / tileScale, y / tileScale);                    glVertex3f(x, y, -halfLength);
            glTexCoord2f((x + step) / tileScale, y / tileScale);           glVertex3f(x + step, y, -halfLength);
            glTexCoord2f((x + step) / tileScale, (y + step) / tileScale);  glVertex3f(x + step, y + step, -halfLength);
            glTexCoord2f(x / tileScale, (y + step) / tileScale);           glVertex3f(x, y + step, -halfLength);
        }
    }

    // Front Wall
    glNormal3f(0.0f, 0.0f, -1.0f);
    for (float x = -halfWidth; x < halfWidth; x += step) {
        for (float y = 0.0f; y < height; y += step) {
            glTexCoord2f(x / tileScale, (y + step) / tileScale);           glVertex3f(x, y + step, halfLength);
            glTexCoord2f((x + step) / tileScale, (y + step) / tileScale);  glVertex3f(x + step, y + step, halfLength);
            glTexCoord2f((x + step) / tileScale, y / tileScale);           glVertex3f(x + step, y, halfLength);
            glTexCoord2f(x / tileScale, y / tileScale);                    glVertex3f(x, y, halfLength);
        }
    }

    // Left Wall
    glNormal3f(1.0f, 0.0f, 0.0f);
    for (float z = -halfLength; z < halfLength; z += step) {
        for (float y = 0.0f; y < height; y += step) {
            glTexCoord2f((z + step) / tileScale, y / tileScale);           glVertex3f(-halfWidth, y, z + step);
            glTexCoord2f(z / tileScale, y / tileScale);                    glVertex3f(-halfWidth, y, z);
            glTexCoord2f(z / tileScale, (y + step) / tileScale);           glVertex3f(-halfWidth, y + step, z);
            glTexCoord2f((z + step) / tileScale, (y + step) / tileScale);  glVertex3f(-halfWidth, y + step, z + step);
        }
    }

    // Right Wall
    glNormal3f(-1.0f, 0.0f, 0.0f);
    for (float z = -halfLength; z < halfLength; z += step) {
        for (float y = 0.0f; y < height; y += step) {
            glTexCoord2f((z + step) / tileScale, (y + step) / tileScale);  glVertex3f(halfWidth, y + step, z + step);
            glTexCoord2f(z / tileScale, (y + step) / tileScale);           glVertex3f(halfWidth, y + step, z);
            glTexCoord2f(z / tileScale, y / tileScale);                    glVertex3f(halfWidth, y, z);
            glTexCoord2f((z + step) / tileScale, y / tileScale);           glVertex3f(halfWidth, y, z + step);
        }
    }
    glEnd();

    glDisable(GL_TEXTURE_2D);
}

void Scene::drawDeskWithDrawer() {
    glEnable(GL_TEXTURE_2D);
    texManager.bindTexture(texWood);
    glColor3f(1.0f, 1.0f, 1.0f);
    
    glPushMatrix();
        glTranslatef(2.0f, 0.8f, -3.5f);
        
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
            glTranslatef(0.0f, -0.2f, currentDrawerZ);
            glScalef(0.8f, 0.25f, 0.7f);
            
            // --- NEW DRAWER GEOMETRY (5 Panels) ---
            // Bottom panel
            glPushMatrix();
                glTranslatef(0.0f, -0.45f, 0.0f);
                glScalef(1.0f, 0.1f, 1.0f);
                drawSolidCube(1.0f);
            glPopMatrix();
            
            // Front panel
            glPushMatrix();
                glTranslatef(0.0f, 0.0f, 0.45f);
                glScalef(1.0f, 1.0f, 0.1f);
                drawSolidCube(1.0f);
            glPopMatrix();
            
            // Back panel
            glPushMatrix();
                glTranslatef(0.0f, 0.0f, -0.45f);
                glScalef(1.0f, 1.0f, 0.1f);
                drawSolidCube(1.0f);
            glPopMatrix();
            
            // Left panel
            glPushMatrix();
                glTranslatef(-0.45f, 0.0f, 0.0f);
                glScalef(0.1f, 1.0f, 1.0f);
                drawSolidCube(1.0f);
            glPopMatrix();
            
            // Right panel
            glPushMatrix();
                glTranslatef(0.45f, 0.0f, 0.0f);
                glScalef(0.1f, 1.0f, 1.0f);
                drawSolidCube(1.0f);
            glPopMatrix();
            // --------------------------------------
            
            if (!isInspectingNote) {
                glPushMatrix();
                    // Counteract the drawer's scale to keep the note normal size
                    glScalef(1.0f / 0.8f, 1.0f / 0.25f, 1.0f / 0.7f);
                    // Place it flat on the bottom of the drawer interior
                    glTranslatef(0.0f, -0.09f, 0.0f); 
                    drawNote();
                glPopMatrix();
            }
        glPopMatrix();
    glPopMatrix();

    glDisable(GL_TEXTURE_2D);
}

// --- MISSING WRAPPER FUNCTION ---
void Scene::render() {
    drawAbandonedRoom();
    drawDeskWithDrawer();
}