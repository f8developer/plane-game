//
// Created by ivanr on 5.6.2025 г..
//

#ifndef PLAYERCOMPONENT_H
#define PLAYERCOMPONENT_H

#include "IComponent.h"
#include "GameConfig.h"

enum class BulletType {
    Player,
    Enemy
};

struct PlayerComponent : public IComponent {
    i8 lives = PLAYER_DEFAULT_LIVES;
    const u8 maxLives = PLAYER_MAX_LIVES;

    u8 bullets = PLAYER_DEFAULT_BULLETS;
    u8 bulletDamage = PLAYER_DEFAULT_BULLET_DAMAGE;
    u8 bulletSpeed = PLAYER_DEFAULT_BULLET_SPEED;
    u8 bulletSize = PLAYER_DEFAULT_BULLET_SIZE;

    float moveSpeed = PLAYER_MOVE_SPEED;

    u64 score = 0;
};

struct BulletComponent : public IComponent {
    u8 bulletDamage = PLAYER_DEFAULT_BULLET_DAMAGE;
    u8 bulletSpeed = PLAYER_DEFAULT_BULLET_SPEED;
    u8 bulletSize = PLAYER_DEFAULT_BULLET_SIZE;
    float angle = 0.0f;  // Angle in degrees
    BulletType type = BulletType::Player;

    u64 score = 0;
};

struct EnemyComponent : public IComponent {
    i8 health = ENEMY_DEFAULT_HEALTH;  // Default health
    float moveSpeed = ENEMY_MOVE_SPEED;  // Slower than player
};

#endif //PLAYERCOMPONENT_H
