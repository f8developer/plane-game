#ifndef BULLETSYSTEM_H
#define BULLETSYSTEM_H

#include <entt/entt.hpp>
#include "components/BasicComponent.h"
#include "components/PlayerComponent.h"
#include "components/DrawingComponent.h"
#include "KeyManager.h"
#include "GameConfig.h"
#include "AssetManager.h"
#include <cmath>
#include "raylib.h"

// Convert degrees to radians (using a different name to avoid conflict with raylib)
#define DEGREES_TO_RADIANS DEG2RAD

class BulletSystem {
public:
    static void Initialize(i32 sceneId) {
        // Load bullet texture once and store it in AssetManager
        if (!bulletTexture) {
            AssetManager::AddSceneTexture("bullet", "bomber_one.png", sceneId);
            bulletTexture = &AssetManager::GetTexture("bullet");
        }
    }

    static void Update(entt::registry& registry, float deltaTime) {
        // Pre-calculate delta time factor
        const float deltaFactor = deltaTime * 60.0f;
        
        // Handle bullet spawning
        auto playerView = registry.view<TransformComponent, PlayerComponent, SpriteComponent>();
        for (auto entity : playerView) {
            const auto& transform = playerView.get<TransformComponent>(entity);
            const auto& player = playerView.get<PlayerComponent>(entity);
            
            if (key_manager::IsKeyPressed(KEY_SPACE)) {
                SpawnBullet(registry, transform.position, player, 0.0f);
            }
        }

        // Handle bullet movement and cleanup
        auto bulletView = registry.view<TransformComponent, BulletComponent, SpriteComponent>();
        for (auto entity : bulletView) {
            auto& transform = bulletView.get<TransformComponent>(entity);
            const auto& bullet = bulletView.get<BulletComponent>(entity);
            auto& sprite = bulletView.get<SpriteComponent>(entity);

            // Calculate movement based on angle (pre-calculate sin/cos)
            const float radAngle = bullet.angle * DEGREES_TO_RADIANS;
            const float sinAngle = std::sin(radAngle);
            const float cosAngle = std::cos(radAngle);
            
            // Calculate movement
            const float moveX = sinAngle * static_cast<float>(bullet.bulletSpeed) * deltaFactor;
            const float moveY = -cosAngle * static_cast<float>(bullet.bulletSpeed) * deltaFactor;

            // Move bullet
            transform.position.x += moveX;
            transform.position.y += moveY;

            // Update sprite size based on bullet size
            const float width = BULLET_BASE_WIDTH * static_cast<float>(bullet.bulletSize);
            const float height = BULLET_BASE_HEIGHT * static_cast<float>(bullet.bulletSize);
            sprite.size = Vector2{width, height};

            // Set bullet color based on type
            sprite.tint = (bullet.type == BulletType::Player) ? YELLOW : RED;

            // Remove bullet if it goes off screen (using early exit)
            if (transform.position.y < -10.0f || 
                transform.position.y > VIRTUAL_HEIGHT + 10.0f ||
                transform.position.x < -10.0f ||
                transform.position.x > VIRTUAL_WIDTH + 10.0f) {
                registry.destroy(entity);
            }
        }
    }

    // Helper function to spawn enemy bullets
    static void SpawnEnemyBullet(entt::registry& registry, const Vector2& position, float angle) {
        PlayerComponent enemyBulletProps;
        enemyBulletProps.bulletSpeed = ENEMY_BULLET_SPEED;  // Enemy bullets are slower
        enemyBulletProps.bulletSize = PLAYER_DEFAULT_BULLET_SIZE;
        enemyBulletProps.bulletDamage = ENEMY_BULLET_DAMAGE;  // Enemy bullets do less damage
        
        SpawnBullet(registry, position, enemyBulletProps, angle, BulletType::Enemy);
    }

private:
    static void SpawnBullet(entt::registry& registry, const Vector2& position, const PlayerComponent& props, float angle, BulletType type = BulletType::Player) {
        auto bullet = registry.create();

        // Add transform component
        auto& transform = registry.emplace<TransformComponent>(bullet);
        transform.position = position;  // Start at spawn position
        transform.rotation = angle;     // Set the rotation for rendering

        // Add bullet component with properties
        auto& bulletComp = registry.emplace<BulletComponent>(bullet);
        bulletComp.bulletDamage = props.bulletDamage;
        bulletComp.bulletSpeed = props.bulletSpeed;
        bulletComp.bulletSize = props.bulletSize;
        bulletComp.angle = angle;
        bulletComp.type = type;

        // Calculate bullet dimensions based on size
        float width = BULLET_BASE_WIDTH * static_cast<float>(props.bulletSize);
        float height = BULLET_BASE_HEIGHT * static_cast<float>(props.bulletSize);

        // Add visual representation using sprite
        auto& sprite = registry.emplace<SpriteComponent>(bullet);
        sprite.size = Vector2{width, height};
        sprite.tint = (type == BulletType::Player) ? YELLOW : RED;
        sprite.texture = bulletTexture;  // No need to dereference since we're using a pointer now
    }

    static const Texture* bulletTexture;  // Static pointer to bullet texture
};

#endif //BULLETSYSTEM_H 