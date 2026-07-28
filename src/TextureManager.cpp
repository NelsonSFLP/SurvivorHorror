#include "TextureManager.h"

// STB_IMAGE_IMPLEMENTATION tells the precompiler to include the actual implementation code 
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

TextureManager::TextureManager() {}

TextureManager::~TextureManager() {
    clear();
}

GLuint TextureManager::loadTexture(const std::string& filepath) {
    // 1. Check if the texture is already in our cache
    auto it = textureCache.find(filepath);
    if (it != textureCache.end()) {
        return it->second; // Return existing GPU ID instantly 
    }

    // 2. Flip image vertically during loading because OpenGL expects UV origin (0,0) at the bottom-left 
    stbi_set_flip_vertically_on_load(true);

    int width, height, nrChannels;
    // Read raw image bytes from CPU disk 
    unsigned char* data = stbi_load(filepath.c_str(), &width, &height, &nrChannels, 0);

    if (!data) {
        std::cerr << "[ERROR] TextureManager failed to load image at: " << filepath << std::endl;
        return 0;
    }

    // 3. Request a unique numerical handle from OpenGL and bind it 
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Determine correct OpenGL color format based on channel count 
    GLenum format = GL_RGB;
    if (nrChannels == 4) format = GL_RGBA;
    else if (nrChannels == 1) format = GL_LUMINANCE;

    // 4. Send pixel bytes from CPU RAM into GPU VRAM 
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);

    // --- TEXTURE PARAMETERS ---
    // Set wrapping to GL_REPEAT so we can tile textures across large 10-meter walls 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // Set linear filtering for smooth pixel interpolation in horror atmosphere 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // 5. Free the CPU RAM copy now that the GPU has its own copy in VRAM
    stbi_image_free(data);

    // Store the generated ID in our cache map 
    textureCache[filepath] = textureID;

    std::cout << "[SYSTEM] Successfully loaded texture: " << filepath 
              << " (" << width << "x" << height << " | Channels: " << nrChannels << ")" << std::endl;

    return textureID;
}

void TextureManager::bindTexture(GLuint textureID) {
    // Make the specified texture handle the active state on Texture Unit 0 
    glBindTexture(GL_TEXTURE_2D, textureID);
}

void TextureManager::clear() {
    // Delete all OpenGL texture objects from VRAM to prevent memory leaks
    for (auto& pair : textureCache) {
        glDeleteTextures(1, &pair.second);
    }
    textureCache.clear();
    std::cout << "[SYSTEM] TextureManager cache cleared from GPU VRAM." << std::endl;
}