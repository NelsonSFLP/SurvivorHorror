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

// Infinite line in 3D space
struct Ray {
    float origin[3];
    float direction[3];
};

//3d bounding box
struct AABB {
    float min[3];
    float max [3];
};

//Slab method for detection of intersections 
inline bool checkRayAABBIntersection(const Ray& ray, const AABB& box, float& hitDistance) {
    //Setting limits
    float tmin = -INFINITY, tmax = INFINITY;
    //Checking intersection
    for (int i =0; i < 3; i++){
        if (std::abs(ray.direction[i]) < 0.00001f) {
            if(ray.origin[i] < box.min[i] || ray.origin[i] > box.max[i]) return false;
            continue;
        }
        float t1 = (box.min[i] - ray.origin[i]) / ray.direction[i];
        float t2 = (box.max[i] - ray.origin[i]) / ray.direction[i];

        if (t1 > t2) std::swap(t1, t2);

        tmin = std::max(tmin, t1);
        tmax = std::min(tmax, t2);

        if (tmin > tmax) return false;
    }

    hitDistance = tmin;
    return tmin >= 0.0f;
}


#endif // UTILS_H