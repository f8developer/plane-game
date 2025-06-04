//
// Created by ivanr on 4.6.2025 г..
//

#include "KeyManager.h"

#include "raylib.h"

namespace key_manager {
    bool IsKeyPressed(const int& key) {
        return ::IsKeyPressed(key);
    }
}