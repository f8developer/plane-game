//
// Created by ivanr on 2.6.2025 г..
//

#include "Renderer.h"
#include "raylib.h"

void Renderer::BeginDraw() {
    BeginDrawing();
}

void Renderer::Clear() {
    ClearBackground(RAYWHITE);
}

void Renderer::EndDraw() {
    EndDrawing();
}