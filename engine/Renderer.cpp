//
// Created by ivanr on 2.6.2025 г..
//

#include "Renderer.h"
#include "raylib.h"

namespace render {
    void BeginDraw() {
        BeginDrawing();
    }

    void Clear() {
        ClearBackground(BLACK);
    }

    void EndDraw() {
        EndDrawing();
    }

    void DrawTexture(const Texture2D *texture, const int x, const int y, const Color color) {
        DrawTexture(*texture, x, y , color);
    }

    void DrawText(const str &text, const int x, const int y, const int fontSize, const Color color) {
        ::DrawText(text.c_str(), x, y, fontSize, color);
    }

    int MeasureText(const str &text, int fontSize) {
        return ::MeasureText(text.c_str(), fontSize);
    }

    int GetScreenWidth() {
        return ::GetScreenWidth();
    }

    int GetScreenHeight() {
        return ::GetScreenHeight();
    }
}
