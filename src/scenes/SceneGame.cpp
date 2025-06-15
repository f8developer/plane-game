//
// Created by ivanr on 4.6.2025 г..
//

#include "SceneGame.h"
#include "GameConfig.h"
#include "systems/BulletSystem.h"
#include "systems/CollisionSystem.h"

SceneGame::~SceneGame() {
    Unload();
}

void SceneGame::Load() {
    // Register systems with the system manager
    systemManager.AddSystem(
        [](entt::registry& reg, float dt) { MovementSystem::Update(reg, dt); },
        SystemManager::UpdateType::Update
    );
    
    systemManager.AddSystem(
        [](entt::registry& reg, float dt) { BulletSystem::Update(reg, dt); },
        SystemManager::UpdateType::Update
    );

    systemManager.AddSystem(
        [](entt::registry& reg, float dt) { CollisionSystem::Update(reg, dt); },
        SystemManager::UpdateType::Update
    );
    
    systemManager.AddSystem(
        [](entt::registry& reg, float dt) { RenderSystem::DrawPixelPerfect(reg); },
        SystemManager::UpdateType::Draw
    );

    // Initialize systems that need it
    BulletSystem::Initialize( SCENE_NAME );

    SetupCamera();

    SpawnPlayer();
    SpawnEnemy();  // Spawn an enemy after the player
    LOG_DEBUG("Loaded the Game Scene");
}

void SceneGame::Unload() {
    systemManager.ClearAllSystems();
    AssetManager::RemoveSceneTextures(SCENE_NAME);
    LOG_DEBUG("Unloaded the Game Scene");
}

void SceneGame::Update(float d_time) {
    // Regular update - runs every frame
    // Good for visual updates, input handling, etc.
    systemManager.ExecuteSystems(SystemManager::UpdateType::Update, registry, d_time);
}

void SceneGame::FixedUpdate(float fixed_d_time) {
    // Fixed update - runs every 20ms
    // Good for physics, game logic, and other deterministic systems
    systemManager.ExecuteSystems(SystemManager::UpdateType::FixedUpdate, registry, fixed_d_time);
}

void SceneGame::AsyncUpdate(float d_time) {
    // Async update - runs on a separate thread
    // Good for heavy computations, pathfinding, etc.
    systemManager.ExecuteSystems(SystemManager::UpdateType::AsyncUpdate, registry, d_time);
    // LOG_DEBUG("Current FPS: %d", render::GetFPS());
}

void SceneGame::Draw() {
    systemManager.ExecuteSystems(SystemManager::UpdateType::Draw, registry, 0.0f);
}

void SceneGame::SetupCamera() {
    // Create the pixel-perfect camera entity
    auto cameraEntity = registry.create();
    auto& camera = registry.emplace<PixelPerfectCameraComponent>(cameraEntity);

    // Initialize the camera
    camera.OnCreate();
    
    // Set up the world space camera
    camera.worldSpaceCamera.offset = Vector2{ VIRTUAL_WIDTH / 2.0f, VIRTUAL_HEIGHT / 2.0f };
    camera.worldSpaceCamera.target = Vector2{ VIRTUAL_WIDTH / 2.0f, VIRTUAL_HEIGHT / 2.0f };
    camera.worldSpaceCamera.zoom = 1.0f;
    camera.worldSpaceCamera.rotation = 0.0f;
    
    // Set up the screen space camera
    camera.screenSpaceCamera.offset = Vector2{ 0.0f, 0.0f };
    camera.screenSpaceCamera.target = Vector2{ 0.0f, 0.0f };
    camera.screenSpaceCamera.zoom = 1.0f;
    camera.screenSpaceCamera.rotation = 0.0f;
}

void SceneGame::SpawnPlayer() {
    auto player = registry.create();

    auto& transform = registry.emplace<TransformComponent>(player);
    auto& player_comp = registry.emplace<PlayerComponent>(player);
    auto& player_immage = registry.emplace<SpriteComponent>(player);

    AssetManager::AddSceneTexture("player", "bomber_one.png", SCENE_NAME);
    auto& texture = AssetManager::GetTexture("player");

    // Define the desired sprite size
    player_immage.size.x = PLAYER_SPRITE_SIZE;
    player_immage.size.y = PLAYER_SPRITE_SIZE;

    // Position the player in the center of the virtual resolution
    transform.position.x = VIRTUAL_WIDTH / 2.0f;   // Center of virtual width
    transform.position.y = VIRTUAL_HEIGHT - (player_immage.size.y/2.0f + 5.0f);  // Center of virtual height

    // Set the source rectangle to use the entire texture
    player_immage.texture = &texture;
    player_immage.tint = WHITE;  // Use white tint to show original colors

    // Log player spawn info
    LOG_DEBUG("Player spawned with %d bullets", player_comp.bullets);
}

void SceneGame::SpawnEnemy() {
    auto enemy = registry.create();

    auto& transform = registry.emplace<TransformComponent>(enemy);
    auto& enemy_comp = registry.emplace<EnemyComponent>(enemy);
    auto& enemy_image = registry.emplace<SpriteComponent>(enemy);

    // Reuse player texture but make enemy smaller
    AssetManager::AddSceneTexture("enemy", "bomber_one.png", SCENE_NAME);
    auto& texture = AssetManager::GetTexture("enemy");

    // Define the desired sprite size (smaller than player)
    enemy_image.size.x = ENEMY_SPRITE_SIZE;
    enemy_image.size.y = ENEMY_SPRITE_SIZE;

    // Position the enemy at a random x position at the top of the screen
    transform.position.x = static_cast<float>(GetRandomValue(0, VIRTUAL_WIDTH));
    transform.position.y = enemy_image.size.y/2.0f + 5.0f;  // Just below top edge
    transform.rotation = 180.0f;  // Rotate 180 degrees to face downward

    // Set the source rectangle to use the entire texture
    enemy_image.texture = &texture;
    enemy_image.tint = RED;  // Make enemy red to distinguish it
    enemy_image.origin = Vector2{ enemy_image.size.x, enemy_image.size.y };  // Set origin to center for rotation

    LOG_DEBUG("Enemy spawned with %d health", enemy_comp.health);
}
