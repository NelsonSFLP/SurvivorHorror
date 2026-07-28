#ifndef TEXTURE_MANAGER_H
#define TEXTURE_MANAGER_H

#include "Utils.h"
#include <map>
#include <string>

class TextureManager {
private:
    // Caches loaded texture IDs by filepath to prevent duplicate GPU VRAM allocations 
    std::map<std::string, GLuint> textureCache;

public:
    TextureManager();
    ~TextureManager();

    // Loads an image from disk using stb_image, uploads it to VRAM, and returns the OpenGL texture ID 
    // If the image was already loaded previously, returns the cached ID immediately 
    GLuint loadTexture(const std::string& filepath);

    // Binds a specific texture ID to the active OpenGL texture unit 
    void bindTexture(GLuint textureID);

    // Clears all cached textures from GPU VRAM
    void clear();
};

#endif // TEXTURE_MANAGER_H