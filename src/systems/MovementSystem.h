#ifndef MOVEMENTSYSTEM_H
#define MOVEMENTSYSTEM_H

#include <entt/entt.hpp>

#include "KeyManager.h"
#include "components/BasicComponent.h"
#include "components/PlayerComponent.h"
#include "components/DrawingComponent.h"
#include "GameConfig.h"

// A simple movement system that updates positions based on velocity
class MovementSystem {
public:
    static void Update(entt::registry& registry, float deltaTime) {
        // Clamp delta time to avoid physics glitches
        deltaTime = std::min(deltaTime, ENGINE_FIXED_TIME_STEP);

        // Get the camera component to access virtual resolution
        auto cameraView = registry.view<PixelPerfectCameraComponent>();
        if (cameraView.empty()) return;  // No camera, no movement
        
        // Pre-calculate movement amount
        const float moveAmount = PLAYER_MOVE_SPEED * deltaTime;
        
        auto view = registry.view<TransformComponent, PlayerComponent, SpriteComponent>();
        for (auto entity : view) {
            auto& transform = view.get<TransformComponent>(entity);
            const auto& player = view.get<PlayerComponent>(entity);
            const auto& sprite = view.get<SpriteComponent>(entity);

            // Calculate actual movement speed
            const float actualMoveAmount = player.moveSpeed * moveAmount;

            // Handle horizontal movement
            if (key_manager::IsKeyDown(KEY_RIGHT) || key_manager::IsKeyDown(KEY_D)) {
                transform.position.x += actualMoveAmount;
            }
            if (key_manager::IsKeyDown(KEY_LEFT) || key_manager::IsKeyDown(KEY_A)) {
                transform.position.x -= actualMoveAmount;
            }

            // Handle vertical movement
            if (key_manager::IsKeyDown(KEY_UP) || key_manager::IsKeyDown(KEY_W)) {
                transform.position.y -= actualMoveAmount;
            }
            if (key_manager::IsKeyDown(KEY_DOWN) || key_manager::IsKeyDown(KEY_S)) {
                transform.position.y += actualMoveAmount;
            }

            // Calculate sprite boundaries
            const float halfWidth = sprite.size.x / 2.0f;
            const float halfHeight = sprite.size.y / 2.0f;

            // Keep player within virtual screen bounds with proper sprite centering
            transform.position.x = std::max(halfWidth, std::min(transform.position.x, VIRTUAL_WIDTH - halfWidth));
            transform.position.y = std::max(halfHeight, std::min(transform.position.y, VIRTUAL_HEIGHT - halfHeight));
        }
    }
};

#endif //MOVEMENTSYSTEM_H 