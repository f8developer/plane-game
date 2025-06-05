//
// Created by ivanr on 5.6.2025 г..
//

#ifndef BASICCOMPONENT_H
#define BASICCOMPONENT_H

#include "IComponent.h"

struct TransformComponent : public IComponent {
    Vector2 position;
};

#endif //BASICCOMPONENT_H
