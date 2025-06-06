//
// Created by ivanr on 2.6.2025 г..
//

#ifndef ENGINE_RENDERER_H
#define ENGINE_RENDERER_H

#include "Defines.h"
#include "raylib.h"
#include <vector>
#include <variant>
#include <string>

namespace render {
    // Batch rendering structures
    struct TextureCommand {
        const Texture2D* texture;
        int x, y;
        Color color;
    };

    struct TextCommand {
        std::string text;
        int x, y;
        int fontSize;
        Color color;
    };

    struct RectangleCommand {
        Rectangle rec;
        Vector2 origin;
        float rotation;
        Color color;
    };

    struct TextureProCommand {
        const Texture2D* texture;
        Rectangle source;
        Rectangle dest;
        Vector2 origin;
        float rotation;
        Color tint;
    };

    struct TextureRecCommand {
        const Texture2D* texture;
        Rectangle source;
        Vector2 position;
        Color tint;
    };

    using DrawCommand = std::variant<
        TextureCommand,
        TextCommand,
        RectangleCommand,
        TextureProCommand,
        TextureRecCommand
    >;

    // State management
    struct RenderState {
        Color currentColor = WHITE;
        float currentRotation = 0.0f;
        Vector2 currentOrigin = {0, 0};
        Font currentFont = {0};
        int currentFontSize = 10;
    };

    // Initialize and shutdown
    DLLEX void Initialize();
    DLLEX void Shutdown();

    // Batch rendering
    DLLEX void BeginBatch();
    DLLEX void EndBatch();
    DLLEX void FlushBatch();

    // State management
    DLLEX void SetColor(Color color);
    DLLEX void SetRotation(float rotation);
    DLLEX void SetOrigin(Vector2 origin);
    DLLEX void SetFont(Font font, int fontSize);

    // Drawing functions
    DLLEX void BeginDraw();
    DLLEX void Clear();
    DLLEX void EndDraw();

    DLLEX void DrawTexture(const Texture2D* texture, int x, int y, Color color);
    DLLEX void DrawTextureV(const Texture2D* texture, Vector2 position, Color color);
    DLLEX void DrawText(const str& text, int x, int y, int fontSize, Color color);
    DLLEX void DrawTextPro(Font font, const char* text, Vector2 position, Vector2 origin, float rotation, float fontSize, float spacing, Color tint);
    DLLEX int MeasureText(const str& text, int fontSize);
    DLLEX int GetScreenWidth();
    DLLEX int GetScreenHeight();
    DLLEX int GetFPS();
    DLLEX void DrawFPS(int x, int y);
    DLLEX void DrawRectanglePro(Rectangle rec, Vector2 origin, float rotation, Color color);
    DLLEX void DrawTexturePro(const Texture2D& texture, Rectangle source, Rectangle dest, Vector2 origin, float rotation, Color tint);
    DLLEX void DrawTextureRec(const Texture2D& texture, Rectangle source, Vector2 position, Color tint);
}

#endif //ENGINE_RENDERER_H
