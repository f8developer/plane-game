#ifndef COLLISIONSYSTEM_H
#define COLLISIONSYSTEM_H

#include <entt/entt.hpp>
#include "components/BasicComponent.h"
#include "components/PlayerComponent.h"
#include "components/DrawingComponent.h"
#include "GameConfig.h"

class CollisionSystem {
public:
    static void Update(entt::registry& registry, float deltaTime) {
        // Get all bullets, enemies, and player
        auto bulletView = registry.view<TransformComponent, BulletComponent, SpriteComponent>();
        auto enemyView = registry.view<TransformComponent, EnemyComponent, SpriteComponent>();
        auto playerView = registry.view<TransformComponent, PlayerComponent, SpriteComponent>();

        // Pre-calculate player rectangle if player exists
        Rectangle playerRect = {0};
        bool playerExists = false;
        for (auto playerEntity : playerView) {
            const auto& playerTransform = playerView.get<TransformComponent>(playerEntity);
            const auto& playerSprite = playerView.get<SpriteComponent>(playerEntity);
            
            playerRect = {
                playerTransform.position.x - playerSprite.size.x / 2.0f,
                playerTransform.position.y - playerSprite.size.y / 2.0f,
                playerSprite.size.x,
                playerSprite.size.y
            };
            playerExists = true;
            break;
        }

        // Check each bullet against appropriate targets
        for (auto bulletEntity : bulletView) {
            const auto& bulletTransform = bulletView.get<TransformComponent>(bulletEntity);
            const auto& bulletComp = bulletView.get<BulletComponent>(bulletEntity);
            const auto& bulletSprite = bulletView.get<SpriteComponent>(bulletEntity);

            // Create bullet AABB once
            const Rectangle bulletRect = {
                bulletTransform.position.x - bulletSprite.size.x / 2.0f,
                bulletTransform.position.y - bulletSprite.size.y / 2.0f,
                bulletSprite.size.x,
                bulletSprite.size.y
            };

            if (bulletComp.type == BulletType::Player) {
                // Player bullets check against enemies
                for (auto enemyEntity : enemyView) {
                    const auto& enemyTransform = enemyView.get<TransformComponent>(enemyEntity);
                    auto& enemyComp = enemyView.get<EnemyComponent>(enemyEntity);
                    const auto& enemySprite = enemyView.get<SpriteComponent>(enemyEntity);

                    // Create enemy AABB
                    const Rectangle enemyRect = {
                        enemyTransform.position.x - enemySprite.size.x / 2.0f,
                        enemyTransform.position.y - enemySprite.size.y / 2.0f,
                        enemySprite.size.x,
                        enemySprite.size.y
                    };

                    // Check for collision
                    if (CheckCollisionRecs(bulletRect, enemyRect)) {
                        // Apply damage to enemy
                        enemyComp.health -= bulletComp.bulletDamage;

                        // Remove bullet
                        registry.destroy(bulletEntity);

                        // Remove enemy if health is depleted
                        if (enemyComp.health <= 0) {
                            registry.destroy(enemyEntity);
                        }

                        // Break out of enemy loop since bullet is destroyed
                        break;
                    }
                }
            } else if (playerExists) {
                // Enemy bullets check against player
                if (CheckCollisionRecs(bulletRect, playerRect)) {
                    auto& playerComp = playerView.get<PlayerComponent>(playerView.front());
                    
                    // Remove a life from player
                    playerComp.lives--;

                    // Remove bullet
                    registry.destroy(bulletEntity);

                    // Remove player if no lives left
                    if (playerComp.lives <= 0) {
                        registry.destroy(playerView.front());
                        // TODO: Handle game over
                    }
                }
            }
        }
    }

private:
    static bool CheckCollisionRecs(const Rectangle& rec1, const Rectangle& rec2) {
        return (rec1.x < rec2.x + rec2.width &&
                rec1.x + rec1.width > rec2.x &&
                rec1.y < rec2.y + rec2.height &&
                rec1.y + rec1.height > rec2.y);
    }
};

#endif //COLLISIONSYSTEM_H 