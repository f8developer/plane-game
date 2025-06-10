//
// Created by ivanr on 2.6.2025 г..
//

#include "Renderer.h"

#include <cmath>

#include "raylib.h"
#include "Log.h"

namespace render {
    // Static member initialization
    static std::vector<DrawCommand> drawCommands;
    static bool isBatching = false;
    static Color backgroundColor = BLUE;  // Default background color

    void Initialize() {
        drawCommands.reserve(1000); // Pre-allocate space for commands
    }

    void Shutdown() {
        drawCommands.clear();
    }

    void SetBackgroundColor(Color color) {
        backgroundColor = color;
    }

    void BeginBatch() {
        if (isBatching) {
            ENGINE_LOG(LOG_WARNING, "BeginBatch called while already batching");
            return;
        }
        isBatching = true;
        drawCommands.clear();
    }

    void EndBatch() {
        if (!isBatching) {
            ENGINE_LOG(LOG_WARNING, "EndBatch called while not batching");
            return;
        }
        FlushBatch();
        isBatching = false;
    }

    void FlushBatch() {
        if (!isBatching || drawCommands.empty()) return;

        for (const auto& cmd : drawCommands) {
            std::visit([](const auto& command) {
                using T = std::decay_t<decltype(command)>;
                if constexpr (std::is_same_v<T, TextureCommand>) {
                    ::DrawTexture(*command.texture, command.x, command.y, command.color);
                }
                else if constexpr (std::is_same_v<T, TextCommand>) {
                    ::DrawText(command.text.c_str(), command.x, command.y, command.fontSize, command.color);
                }
                else if constexpr (std::is_same_v<T, RectangleCommand>) {
                    ::DrawRectanglePro(command.rec, command.origin, command.rotation, command.color);
                }
                else if constexpr (std::is_same_v<T, TextureProCommand>) {
                    ::DrawTexturePro(*command.texture, command.source, command.dest, 
                                   command.origin, command.rotation, command.tint);
                }
                else if constexpr (std::is_same_v<T, TextureRecCommand>) {
                    ::DrawTextureRec(*command.texture, command.source, command.position, command.tint);
                }
            }, cmd);
        }
        drawCommands.clear();
    }

    void BeginDraw() {
        BeginDrawing();
    }

    void Clear() {
        ClearBackground(backgroundColor);
    }

    void EndDraw() {
        if (isBatching) {
            FlushBatch();
        }
        EndDrawing();
    }

    void DrawTexture(const Texture2D* texture, int x, int y, Color color) {
        if (isBatching) {
            drawCommands.emplace_back(TextureCommand{texture, x, y, color});
        } else {
            ::DrawTexture(*texture, x, y, color);
        }
    }

    void DrawTextureV(const Texture2D* texture, Vector2 position, Color color) {
        DrawTexture(texture, static_cast<int>(position.x), static_cast<int>(position.y), color);
    }

    void DrawText(const str& text, int x, int y, int fontSize, Color color) {
        if (isBatching) {
            drawCommands.emplace_back(TextCommand{text, x, y, fontSize, color});
        } else {
            ::DrawText(text.c_str(), x, y, fontSize, color);
        }
    }

    void DrawTextPro(Font font, const char* text, Vector2 position, Vector2 origin, float rotation, float fontSize, float spacing, Color tint) {
        ::DrawTextPro(font, text, position, origin, rotation, fontSize, spacing, tint);
    }

    void DrawTextPixelPerfect(Font font, const char *text, Vector2 position, float fontSize, float spacing,
        Color tint) {
        // Force integer positions
        Vector2 pixelPos = {
            roundf(position.x),
            roundf(position.y)
        };

        // Force integer spacing
        float pixelSpacing = roundf(spacing);

        // Draw with exact positioning
        ::DrawTextEx(font, text, pixelPos, fontSize, pixelSpacing, tint);
    }

    int MeasureText(const str& text, int fontSize) {
        return ::MeasureText(text.c_str(), fontSize);
    }

    int GetScreenWidth() {
        return ::GetScreenWidth();
    }

    int GetScreenHeight() {
        return ::GetScreenHeight();
    }

    int GetFPS() {
        return ::GetFPS();
    }

    void DrawFPS(int x, int y) {
        ::DrawFPS(x, y);
    }

    void DrawRectanglePro(Rectangle rec, Vector2 origin, float rotation, Color color) {
        if (isBatching) {
            drawCommands.emplace_back(RectangleCommand{rec, origin, rotation, color});
        } else {
            ::DrawRectanglePro(rec, origin, rotation, color);
        }
    }

    void DrawTexturePro(const Texture2D& texture, Rectangle source, Rectangle dest, Vector2 origin, float rotation, Color tint) {
        if (isBatching) {
            drawCommands.emplace_back(TextureProCommand{&texture, source, dest, origin, rotation, tint});
        } else {
            ::DrawTexturePro(texture, source, dest, origin, rotation, tint);
        }
    }

    void DrawTextureRec(const Texture2D& texture, Rectangle source, Vector2 position, Color tint) {
        if (isBatching) {
            drawCommands.emplace_back(TextureRecCommand{&texture, source, position, tint});
        } else {
            ::DrawTextureRec(texture, source, position, tint);
        }
    }
}
