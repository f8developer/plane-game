#ifndef TRANSITIONSYSTEM_H
#define TRANSITIONSYSTEM_H

#include <entt/entt.hpp>
#include "components/BasicComponent.h"
#include "components/PlayerComponent.h"
#include "components/DrawingComponent.h"
#include "GameConfig.h"
#include "raylib.h"
#include "systems/SystemManager.h"

class TransitionSystem {
public:
    static void Update(entt::registry& registry, float deltaTime);
    static void SetSystemManager(SystemManager* manager);
    static void SetSystemIDs(SystemManager::SystemID movementID, SystemManager::SystemID bulletID, SystemManager::SystemID transitionID);
    static void Initialize(entt::registry& registry);

private:
    static void UpdateLanding(TransformComponent& transform, TransitionComponent& transition, 
                            SpriteComponent& sprite, float deltaTime);
    static void UpdateTakeoff(entt::registry& registry, TransformComponent& transform, TransitionComponent& transition,
                            SpriteComponent& sprite, float deltaTime);
    static void UpdateBoatAnimation(entt::registry& registry, float deltaTime);
    static void SpawnShip(entt::registry& registry);

    static SystemManager* systemManager;
    static SystemManager::SystemID movementSystemID;
    static SystemManager::SystemID bulletSystemID;
    static SystemManager::SystemID transitionSystemID;
    static entt::entity shipEntity;
};

#endif //TRANSITIONSYSTEM_H 