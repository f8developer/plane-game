#ifndef ICOMPONENT_H
#define ICOMPONENT_H

#include <entt/entt.hpp>
#include <string>

#include "Defines.h"

class IComponent {
public:
    virtual ~IComponent() = default;
    virtual void OnCreate() {}  // Called when component is created
    virtual void OnUpdate(float deltaTime) {}  // Called every frame
    virtual void OnDestroy() {} // Called when component is destroyed
};

#endif //ICOMPONENT_H