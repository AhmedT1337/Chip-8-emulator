#pragma once

#include <cstdint>
#include <raylib.h>

class Platform
{
public:
    Platform(char const* title, int windowWidth, int windowHeight, int textureWidth, int textureHeight);
    ~Platform();
    void Update(void const* buffer, int pitch);
    bool ProcessInput(uint8_t* keys);

private:
    int texWidth;
    int texHeight;
    Image screenImage;
    Texture2D texture;
};