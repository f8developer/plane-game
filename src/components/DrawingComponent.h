//
// Created by ivanr on 5.6.2025 г..
//

#ifndef DRAWINGCOMPONENT_H
#define DRAWINGCOMPONENT_H

#include "IComponent.h"

struct SpriteComponent : public IComponent {
    Texture* texture;
};

struct TextComponent : public IComponent {
    str text;
};

struct DrawingOffsetComponent: public IComponent {
    i32 positionX;
    i32 positionY;
};

struct CameraComponent : public IComponent {
    Camera2D camera;
};

#endif //DRAWINGCOMPONENT_H
