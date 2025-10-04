#include "texture.h"

#include <stdio.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

GLuint LoadTexture(const char* filepath)
{
    int width, height, channels;
    unsigned char* data = stbi_load(filepath, &width, &height, &channels, 4);  // Force RGBA

    if (!data)
    {
        printf("Failed to load texture: %s\n", filepath);
        return 0;
    }

    GLuint textureID;
    glGenTextures(1, &textureID);  // Generate a texture ID
    glBindTexture(GL_TEXTURE_2D, textureID);  // Make it active

    // Upload pixel data to GPU
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

    // Set texture parameters (how it's sampled)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    stbi_image_free(data);  // Free CPU-side memory (GPU has it now)

    printf("Loaded texture: %s (%dx%d)\n", filepath, width, height);
    return textureID;
}

GLFWimage LoadImage(const char* filepath)
{
    GLFWimage image;
    int channels;

    // stbi_load returns pixel data in memory
    image.pixels = stbi_load(filepath, &image.width, &image.height, &channels, 4);

    if (!image.pixels)
    {
        printf("Failed to load image: %s\n", filepath);
        image.width = 0;
        image.height = 0;
    }

    return image;
}
