//
// Created by ivanr on 5.6.2025 г..
//

#ifndef DRAWINGCOMPONENT_H
#define DRAWINGCOMPONENT_H

#include "GameConfig.h"
#include "IComponent.h"
#include <string_view>

struct TextComponent : public IComponent {
    std::string_view text;
    int fontSize;
    Color color;
};

struct TextComponentPro : public IComponent {
    Font font;
    std::string_view text;
    int fontSize;
    float spacing;
    Color tint;
};

struct TextComponentPixelPerfect : public TextComponentPro {};

struct CameraComponent : public IComponent {
    Camera2D camera;
};

struct RectangleComponent : public IComponent {
    Rectangle rectangle{0,0,1,1};
    Color color{RED};
};

struct SpriteComponent : public IComponent {
    const Texture* texture = nullptr;  // Changed from reference to pointer
    Vector2 size{1,1};
    Vector2 origin{0,0};
    Color tint{WHITE};
};

struct PixelPerfectCameraComponent : public IComponent {
    Camera2D worldSpaceCamera;  // Game world camera
    Camera2D screenSpaceCamera; // Smoothing camera
    RenderTexture2D target;     // Render texture for the game world

    // Source and destination rectangles for rendering
    Rectangle sourceRec;
    Rectangle destRec;

    void OnCreate() override {
        // Initialize cameras with default settings
        worldSpaceCamera = Camera2D{};
        screenSpaceCamera = Camera2D{};

        // Initialize the render texture
        target = LoadRenderTexture(VIRTUAL_WIDTH, VIRTUAL_HEIGHT);

        // Set up source rectangle (flipped for OpenGL)
        sourceRec = {
            0.0f,
            0.0f,
            static_cast<float>(target.texture.width),
            -static_cast<float>(target.texture.height)
        };

        // Initial destination rectangle will be updated by RenderSystem
        destRec = { 0.0f, 0.0f, static_cast<float>(VIRTUAL_WIDTH), static_cast<float>(VIRTUAL_HEIGHT) };
    }

    void OnDestroy() override {
        UnloadRenderTexture(target);
    }
};

#endif //DRAWINGCOMPONENT_H
