#include "Platform.hpp"

Platform::Platform(char const* title, int windowWidth, int windowHeight, int textureWidth, int textureHeight)
    : texWidth(textureWidth), texHeight(textureHeight)
{
    InitWindow(windowWidth, windowHeight, title);
    SetTargetFPS(60);

    screenImage = GenImageColor(textureWidth, textureHeight, BLACK);
    
    texture = LoadTextureFromImage(screenImage);
}

Platform::~Platform()
{
    UnloadTexture(texture);
    UnloadImage(screenImage);
    CloseWindow();
}

void Platform::Update(void const* buffer, int pitch)
{
    UpdateTexture(texture, buffer);

    BeginDrawing();
    ClearBackground(BLACK);

    DrawTexturePro(
        texture,
        Rectangle{ 0.0f, 0.0f, (float)texWidth, (float)texHeight },
        Rectangle{ 0.0f, 0.0f, (float)GetScreenWidth(), (float)GetScreenHeight() },
        Vector2{ 0.0f, 0.0f },
        0.0f,
        WHITE
    );

    EndDrawing();
}

bool Platform::ProcessInput(uint8_t* keys)
{
    if (WindowShouldClose() || IsKeyDown(KEY_ESCAPE))
    {
        return true;
    }
    keys[0x0] = IsKeyDown(KEY_X) ? 1 : 0;
    keys[0x1] = IsKeyDown(KEY_ONE) ? 1 : 0;
    keys[0x2] = IsKeyDown(KEY_TWO) ? 1 : 0;
    keys[0x3] = IsKeyDown(KEY_THREE) ? 1 : 0;
    keys[0x4] = IsKeyDown(KEY_Q) ? 1 : 0;
    keys[0x5] = IsKeyDown(KEY_W) ? 1 : 0;
    keys[0x6] = IsKeyDown(KEY_E) ? 1 : 0;
    keys[0x7] = IsKeyDown(KEY_A) ? 1 : 0;
    keys[0x8] = IsKeyDown(KEY_S) ? 1 : 0;
    keys[0x9] = IsKeyDown(KEY_D) ? 1 : 0;
    keys[0xA] = IsKeyDown(KEY_Z) ? 1 : 0;
    keys[0xB] = IsKeyDown(KEY_C) ? 1 : 0;
    keys[0xC] = IsKeyDown(KEY_FOUR) ? 1 : 0;
    keys[0xD] = IsKeyDown(KEY_R) ? 1 : 0;
    keys[0xE] = IsKeyDown(KEY_F) ? 1 : 0;
    keys[0xF] = IsKeyDown(KEY_V) ? 1 : 0;

    return false;
}