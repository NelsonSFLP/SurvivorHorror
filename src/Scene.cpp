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
        glVertex3f(-half, -half,  half);
        glVertex3f( half, -half,  half);
        glVertex3f( half,  half,  half);
        glVertex3f(-half,  half,  half);
        // Back Face
        glNormal3f(0.0f, 0.0f, -1.0f);
        glVertex3f(-half, -half, -half);
        glVertex3f(-half,  half, -half);
        glVertex3f( half,  half, -half);
        glVertex3f( half, -half, -half);
        // Top Face
        glNormal3f(0.0f, 1.0f, 0.0f);
        glVertex3f(-half,  half, -half);
        glVertex3f(-half,  half,  half);
        glVertex3f( half,  half,  half);
        glVertex3f( half,  half, -half);
        // Bottom Face
        glNormal3f(0.0f, -1.0f, 0.0f);
        glVertex3f(-half, -half, -half);
        glVertex3f( half, -half, -half);
        glVertex3f( half, -half,  half);
        glVertex3f(-half, -half,  half);
        // Right Face
        glNormal3f(1.0f, 0.0f, 0.0f);
        glVertex3f( half, -half, -half);
        glVertex3f( half,  half, -half);
        glVertex3f( half,  half,  half);
        glVertex3f( half, -half,  half);
        // Left Face
        glNormal3f(-1.0f, 0.0f, 0.0f);
        glVertex3f(-half, -half, -half);
        glVertex3f(-half, -half,  half);
        glVertex3f(-half,  half,  half);
        glVertex3f(-half,  half, -half);
    glEnd();
}

void Scene::drawAbandonedRoom() {
    float halfWidth = 5.0f;
    float height = 4.0f;
    float halfLength = 5.0f;
    float step = 0.5f;

    glBegin(GL_QUADS);
        // Floor
        glColor3f(0.2f, 0.18f, 0.15f);
        glNormal3f(0.0f, 1.0f, 0.0f);
        for (float x = -halfWidth; x < halfWidth; x += step) {
            for (float z = -halfLength; z < halfLength; z += step) {
                glVertex3f(x, 0.0f, z + step);
                glVertex3f(x + step, 0.0f, z + step);
                glVertex3f(x + step, 0.0f, z);
                glVertex3f(x, 0.0f, z);
            }
        }
        // Ceiling
        glColor3f(0.15f, 0.15f, 0.15f);
        glNormal3f(0.0f, -1.0f, 0.0f);
        for (float x = -halfWidth; x < halfWidth; x += step) {
            for (float z = -halfLength; z < halfLength; z += step) {
                glVertex3f(x, height, z);
                glVertex3f(x + step, height, z);
                glVertex3f(x + step, height, z + step);
                glVertex3f(x, height, z + step);
            }
        }
        // Back Wall
        glColor3f(0.25f, 0.25f, 0.26f);
        glNormal3f(0.0f, 0.0f, 1.0f);
        for (float x = -halfWidth; x < halfWidth; x += step) {
            for (float y = 0.0f; y < height; y += step) {
                glVertex3f(x, y, -halfLength);
                glVertex3f(x + step, y, -halfLength);
                glVertex3f(x + step, y + step, -halfLength);
                glVertex3f(x, y + step, -halfLength);
            }
        }
        // Front Wall
        glNormal3f(0.0f, 0.0f, -1.0f);
        for (float x = -halfWidth; x < halfWidth; x += step) {
            for (float y = 0.0f; y < height; y += step) {
                glVertex3f(x, y + step, halfLength);
                glVertex3f(x + step, y + step, halfLength);
                glVertex3f(x + step, y, halfLength);
                glVertex3f(x, y, halfLength);
            }
        }
        // Left Wall
        glNormal3f(1.0f, 0.0f, 0.0f);
        for (float z = -halfLength; z < halfLength; z += step) {
            for (float y = 0.0f; y < height; y += step) {
                glVertex3f(-halfWidth, y, z + step);
                glVertex3f(-halfWidth, y, z);
                glVertex3f(-halfWidth, y + step, z);
                glVertex3f(-halfWidth, y + step, z + step);
            }
        }
        // Right Wall
        glNormal3f(-1.0f, 0.0f, 0.0f);
        for (float z = -halfLength; z < halfLength; z += step) {
            for (float y = 0.0f; y < height; y += step) {
                glVertex3f(halfWidth, y + step, z + step);
                glVertex3f(halfWidth, y + step, z);
                glVertex3f(halfWidth, y, z);
                glVertex3f(halfWidth, y, z + step);
            }
        }
    glEnd();
}

void Scene::drawDeskWithDrawer(float drawerOffsetZ) {
    glPushMatrix();
        glTranslatef(2.0f, 0.8f, -3.5f);
        
        // Desk Frame
        glColor3f(0.3f, 0.2f, 0.1f);
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
            glColor3f(0.25f, 0.15f, 0.08f);
            glScalef(0.8f, 0.25f, 0.7f);
            drawSolidCube(1.0f);
        glPopMatrix();
    glPopMatrix();
}

// --- MISSING WRAPPER FUNCTION ---
void Scene::render(float drawerOffsetZ) {
    drawAbandonedRoom();
    drawDeskWithDrawer(drawerOffsetZ);
}