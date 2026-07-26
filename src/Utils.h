#ifndef UTILS_H
#define UTILS_H

// Platform specific headers must be included before GLU on Windows
#ifdef _WIN32
#include <windows.h>
#endif

#include <GLFW/glfw3.h>
#include <GL/glu.h>
#include <iostream>
#include <cmath>

// Ensure M_PI is defined across all compilers
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Convert degrees to radians for spherical trigonometric calculations
inline float toRadians(float degrees) {
    return degrees * (static_cast<float>(M_PI) / 180.0f);
}

#endif // UTILS_H