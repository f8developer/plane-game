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
#define UI_MAIN_MENU_FONT_SIZE 7
#define UI_DEFAULT_FONT_SIZE 14

// Engine configuration
#define ENGINE_FIXED_TIME_STEP 0.02f
#define ENGINE_MAX_QUEUED_TASKS 1000

// Common game assets
#define FONT_LANDER "fonts/Lander.ttf"
#define FONT_LANDER_BOLD "fonts/Lander Bold.ttf"

// ------------------------------------------------------

// Transition configuration

// Landing configuration
#define TRANSITION_LANDING_SPEED 1.0f
#define TRANSITION_LANDING_ARC_HEIGHT 50.0f
#define TRANSITION_LANDING_ARC_SMOOTHNESS 1.0f
#define TRANSITION_LANDING_ROTATION_SPEED 1.0f
#define TRANSITION_LANDING_FINAL_ROTATION 0.0f

// Takeoff configuration
#define TRANSITION_TAKEOFF_SPEED 1.0f
#define TRANSITION_TAKEOFF_ARC_HEIGHT 0.5f
#define TRANSITION_TAKEOFF_ARC_SMOOTHNESS 0.5f
#define TRANSITION_TAKEOFF_INITIAL_ROTATION 0.0f
#define TRANSITION_TAKEOFF_FINAL_ROTATION 0.0f

// Ship configuration
#define TRANSITION_LANDED_DURATION 2.0f
#define TRANSITION_SHIP_BOBBING_AMOUNT 2.0f
#define TRANSITION_SHIP_BOBBING_SPEED 2.0f
#define TRANSITION_SHIP_ROTATION_AMOUNT 2.0f
#define TRANSITION_SHIP_EXIT_SPEED 100.0f
#define TRANSITION_SHIP_LANDING_OFFSET 20.0f
#define TRANSITION_SHIP_BOBBING_PHASE_OFFSET 0.5f

// Animation easing configuration
#define TRANSITION_EASING_SMOOTHNESS 3.0f
#define TRANSITION_EASING_STRENGTH 2.0f

#endif //GAMECONFIG_H 