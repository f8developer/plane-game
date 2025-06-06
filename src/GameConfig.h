#ifndef GAMECONFIG_H
#define GAMECONFIG_H

// ------------------------------------------------------
// WINDOW RELATED

// Virtual resolution for pixel-perfect rendering
#define VIRTUAL_WIDTH 224
#define VIRTUAL_HEIGHT 256

// TODO: Make it so it's compatible with as most devices as possible.
// Screen resolution
#define GAME_WIDTH 800
#define GAME_HEIGHT 600

// Game title
#define GAME_TITLE "A Plane Game"

// ------------------------------------------------------

// Player configuration
#define PLAYER_DEFAULT_LIVES 3
#define PLAYER_MAX_LIVES 4
#define PLAYER_DEFAULT_BULLETS 2
#define PLAYER_DEFAULT_BULLET_DAMAGE 10
#define PLAYER_DEFAULT_BULLET_SPEED 10
#define PLAYER_DEFAULT_BULLET_SIZE 1
#define PLAYER_MOVE_SPEED 10.0f
#define PLAYER_SPRITE_SIZE 32

// Enemy configuration
#define ENEMY_DEFAULT_HEALTH 15
#define ENEMY_MOVE_SPEED 5.0f
#define ENEMY_SPRITE_SIZE 24
#define ENEMY_BULLET_SPEED 5
#define ENEMY_BULLET_DAMAGE 5

// Bullet configuration
#define BULLET_BASE_WIDTH 3.0f
#define BULLET_BASE_HEIGHT 5.0f

// UI configuration
#define UI_TITLE_FONT_SIZE 26
#define UI_START_TEXT_FONT_SIZE 18
#define UI_ENTER_TEXT_FONT_SIZE 14
#define UI_DEFAULT_FONT_SIZE 20

// Engine configuration
#define ENGINE_FIXED_TIME_STEP 0.02f
#define ENGINE_MAX_QUEUED_TASKS 1000
#define ENGINE_MAX_ACCUMULATOR 0.25f
#define ENGINE_NUM_WORKER_THREADS 4

#endif //GAMECONFIG_H 