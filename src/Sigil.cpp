#include "Sigil.h"

Sigil::Sigil() : animationTimer(0.0f), isAnimating(false) {}

void Sigil::startAnimation() {
    isAnimating = true;
    animationTimer = 0.0f; // Start at the beginning of the curve
}

void Sigil::reset() {
    isAnimating = false;
    animationTimer = 0.0f;
}

void Sigil::update(float deltaTime) {
    if (isAnimating && animationTimer < 1.0f) {
        animationTimer += deltaTime * 0.4f; // Takes 2.5 seconds to fully draw
        if (animationTimer > 1.0f) {
            animationTimer = 1.0f;
        }
    }
}

void Sigil::render() const {
    if (!isAnimating) return;

    // 4 Control points for a Cubic Bezier curve (an occult 'S' shape)
    // Coords map to the X-Z plane of the paper, with Y slightly raised (0.001f)
    GLfloat ctrlpoints[4][3] = {
        { -0.05f, 0.001f,  0.08f }, // P0: Top left start
        {  0.15f, 0.001f,  0.08f }, // P1: First magnetic pull (right)
        { -0.15f, 0.001f, -0.08f }, // P2: Second magnetic pull (left)
        {  0.05f, 0.001f, -0.08f }  // P3: Bottom right end
    };

    // Disable texturing and lighting so it renders as pure, glowing blood
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_LIGHTING);
    
    // Thicken the line and color it deep red
    glLineWidth(4.0f);
    glColor3f(0.6f, 0.0f, 0.0f);

    // 1. Enable the 1D Evaluator for 3D vertices
    glEnable(GL_MAP1_VERTEX_3);
    
    // 2. Load the control points into the GPU
    // Parameters: Target, u1, u2, stride (3 floats per point), order (4 points), array pointer
    glMap1f(GL_MAP1_VERTEX_3, 0.0f, 1.0f, 3, 4, &ctrlpoints[0][0]);

    // 3. Draw the curve dynamically based on the animation timer
    glBegin(GL_LINE_STRIP);
        int numSegments = 60; // 60 line segments for a perfectly smooth curve
        for (int i = 0; i <= numSegments; i++) {
            float t = (float)i / (float)numSegments;
            
            // Only draw up to our current animation progress
            if (t <= animationTimer) {
                // The GPU calculates the exact coordinate for time 't'!
                glEvalCoord1f(t); 
            }
        }
    glEnd();

    // 4. Clean up state so we don't break the rest of the engine
    glDisable(GL_MAP1_VERTEX_3);
    glEnable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);
    glColor3f(1.0f, 1.0f, 1.0f); 
    glLineWidth(1.0f);
}