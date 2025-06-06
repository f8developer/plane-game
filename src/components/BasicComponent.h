//
// Created by ivanr on 5.6.2025 г..
//

#ifndef BASICCOMPONENT_H
#define BASICCOMPONENT_H

#include "raylib.h"
#include "IComponent.h"

struct TransformComponent : public IComponent {
    Vector2 position;
    float rotation = 0.0f;  // Rotation in degrees
};

#endif //BASICCOMPONENT_H
