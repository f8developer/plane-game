//
// Created by ivanr on 5.6.2025 г..
//

#ifndef PLAYERCOMPONENT_H
#define PLAYERCOMPONENT_H

#include "IComponent.h"

struct PlayerComponent : public IComponent {
    i8 lives = 3;
    const u8 maxLives = 4;

    u8 bullets = 1;
    const u8 maxBullets = 5;
    u8 bulletDamage = 10;
    const u8 maxBulletDamage = 50;
    u8 bulletSpeed = 10;
    const u8 maxBulletSpeed = 50;
    u8 bulletSize = 1;
    const u8 maxBulletSize = 5;

    u64 score = 0;
};

struct BulletComponent : public IComponent {
    u8 bulletDamage = 10;
    const u8 maxBulletDamage = 50;
    u8 bulletSpeed = 10;
    const u8 maxBulletSpeed = 50;
    u8 bulletSize = 1;
    const u8 maxBulletSize = 5;

    u64 score = 0;
};

#endif //PLAYERCOMPONENT_H
