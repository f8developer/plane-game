#ifndef MOVEMENTSYSTEM_H
#define MOVEMENTSYSTEM_H

#include <entt/entt.hpp>

#include "KeyManager.h"
#include "components/BasicComponent.h"
#include "components/PlayerComponent.h"

// A simple movement system that updates positions based on velocity
class MovementSystem {
public:
    static void Update(entt::registry& registry, float deltaTime) {
        // Clamp delta time to avoid physics glitches
        deltaTime = std::min(deltaTime, 0.05f);

        const float moveAmount = 300.0f * deltaTime;  // 300 units per second

        auto view = registry.view<TransformComponent, PlayerComponent>();
        view.each([moveAmount](auto entity, TransformComponent& transform, PlayerComponent& player) {
            // Handle horizontal movement
            if (key_manager::IsKeyDown(KEY_RIGHT) || key_manager::IsKeyDown(KEY_D)) {
                transform.position.x += moveAmount;
            }
            if (key_manager::IsKeyDown(KEY_LEFT) || key_manager::IsKeyDown(KEY_A)) {
                transform.position.x -= moveAmount;
            }

            // Handle vertical movement
            if (key_manager::IsKeyDown(KEY_UP) || key_manager::IsKeyDown(KEY_W)) {
                transform.position.y -= moveAmount;
            }
            if (key_manager::IsKeyDown(KEY_DOWN) || key_manager::IsKeyDown(KEY_S)) {
                transform.position.y += moveAmount;
            }

            // Keep player within screen bounds
            const int screenWidth = render::GetScreenWidth();
            const int screenHeight = render::GetScreenHeight();
            
            // Assuming the player sprite is 32x32 pixels (adjust these values based on your actual sprite size)
            const float playerWidth = 32.0f;
            const float playerHeight = 32.0f;
            
            transform.position.x = std::max(0.0f, std::min(transform.position.x, static_cast<float>(screenWidth - playerWidth)));
            transform.position.y = std::max(0.0f, std::min(transform.position.y, static_cast<float>(screenHeight - playerHeight)));
        });
    }
};

#endif //MOVEMENTSYSTEM_H 