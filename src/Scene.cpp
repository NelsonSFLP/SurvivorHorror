#include "Scene.h"

Scene::Scene() {}

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

void Scene::drawDeskWithDrawer(float drawerOffsetZ) {
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
            glTranslatef(0.0f, -0.2f, drawerOffsetZ); 
            glScalef(0.8f, 0.25f, 0.7f);
            drawSolidCube(1.0f);
        glPopMatrix();
    glPopMatrix();

    glDisable(GL_TEXTURE_2D);
}

// --- MISSING WRAPPER FUNCTION ---
void Scene::render(float drawerOffsetZ) {
    drawAbandonedRoom();
    drawDeskWithDrawer(drawerOffsetZ);
}