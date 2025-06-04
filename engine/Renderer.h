//
// Created by ivanr on 2.6.2025 г..
//

#ifndef ENGINE_RENDERER_H
#define ENGINE_RENDERER_H

#include "Defines.h"
#include "raylib.h"

namespace render{
    void BeginDraw();
    void Clear();
    void EndDraw();

    DLLEX void DrawTexture(const Texture2D* texture, int x, int y, Color color);
    DLLEX void DrawText(const str& text, int x, int y, int fontSize, Color color);
    DLLEX int MeasureText(const str& text, int fontSize);
    DLLEX int GetScreenWidth();
    DLLEX int GetScreenHeight();
}



#endif //ENGINE_RENDERER_H
