//
// Created by ivanr on 4.6.2025 г..
//

#include "SceneGame.h"
#include "GameConfig.h"
#include "systems/BulletSystem.h"
#include "systems/CollisionSystem.h"
#include "KeyManager.h"
#include "AssetManager.h"
#include "components/PlayerComponent.h"
#include "systems/MovementSystem.h"
#include "systems/TransitionSystem.h"
#include "systems/RenderSystem.h"

SceneGame::~SceneGame() {
    Unload();
}

void SceneGame::Load() {
    LOG_DEBUG("Starting Game Scene Load");
    
    // Set up camera first
    SetupCamera();
    
    // Initialize systems first
    BulletSystem::Initialize(SCENE_NAME);
    
    // Register systems and store IDs
    LOG_DEBUG("Registering systems");
    movementSystemID = systemManager.AddSystem(
        [](entt::registry& reg, float dt) { MovementSystem::Update(reg, dt); },
        SystemManager::UpdateType::Update
    );
    LOG_DEBUG("Movement system registered with ID: %llu", movementSystemID);
    
    bulletSystemID = systemManager.AddSystem(
        [](entt::registry& reg, float dt) { BulletSystem::Update(reg, dt); },
        SystemManager::UpdateType::Update
    );
    LOG_DEBUG("Bullet system registered with ID: %llu", bulletSystemID);

    transitionSystemID = systemManager.AddSystem(
        [](entt::registry& reg, float dt) { TransitionSystem::Update(reg, dt); },
        SystemManager::UpdateType::Update
    );
    LOG_DEBUG("Transition system registered with ID: %llu", transitionSystemID);
    
    // Add render system
    systemManager.AddSystem(
        [](entt::registry& reg, float dt) { RenderSystem::DrawPixelPerfect(reg); },
        SystemManager::UpdateType::Draw
    );
    
    // Set up transition system BEFORE creating player
    LOG_DEBUG("Setting up transition system");
    TransitionSystem::SetSystemManager(&systemManager);
    TransitionSystem::SetSystemIDs(movementSystemID, bulletSystemID, transitionSystemID);
    
    // Create player entity
    LOG_DEBUG("Creating player entity");
    auto player = registry.create();
    
    // Add components to player
    auto& transform = registry.emplace<TransformComponent>(player);
    auto& player_image = registry.emplace<SpriteComponent>(player);
    auto& player_comp = registry.emplace<PlayerComponent>(player);
    auto& transition = registry.emplace<TransitionComponent>(player);
    
    // Set up player transform
    transform.position = Vector2{ VIRTUAL_WIDTH / 2.0f, VIRTUAL_HEIGHT / 4.0f };
    transform.rotation = 0.0f;
    
    // Set up player sprite
    AssetManager::AddSceneTexture("player", "bomber_one.png", SCENE_NAME);
    auto& texture = AssetManager::GetTexture("player");
    player_image.texture = &texture;
    player_image.size = Vector2{ PLAYER_SPRITE_SIZE, PLAYER_SPRITE_SIZE };
    player_image.origin = Vector2{ PLAYER_SPRITE_SIZE/2.0f, PLAYER_SPRITE_SIZE/2.0f };
    
    // Set up player movement properties
    player_comp.moveSpeed = PLAYER_MOVE_SPEED;
    
    // Set up transition component
    transition.state = TransitionState::Landing;
    transition.speed = 1.0f;
    transition.targetPosition = Vector2{ VIRTUAL_WIDTH / 2.0f, VIRTUAL_HEIGHT - PLAYER_SPRITE_SIZE * 2.0f };
    transition.targetRotation = 0.0f;
    
    // Initialize transition system after player is set up
    LOG_DEBUG("Initializing transition system");
    TransitionSystem::Initialize(registry);
    
    // Pause movement and bullet systems during transition
    LOG_DEBUG("Pausing movement and bullet systems");
    systemManager.PauseSystem(movementSystemID);
    systemManager.PauseSystem(bulletSystemID);
    
    LOG_DEBUG("Game Scene Load Complete");
}

void SceneGame::Unload() {
    LOG_DEBUG("Starting Game Scene Unload");
    
    // Clean up all entities in the registry
    auto view = registry.view<TransformComponent, SpriteComponent>();
    for (auto entity : view) {
        registry.remove<TransformComponent>(entity);
        registry.remove<SpriteComponent>(entity);
        if (registry.all_of<PlayerComponent>(entity)) {
            registry.remove<PlayerComponent>(entity);
        }
        if (registry.all_of<TransitionComponent>(entity)) {
            registry.remove<TransitionComponent>(entity);
        }
        if (registry.all_of<BulletComponent>(entity)) {
            registry.remove<BulletComponent>(entity);
        }
        registry.destroy(entity);
    }
    
    // Clear all systems
    systemManager.ClearAllSystems();
    
    // Remove all scene textures
    AssetManager::RemoveSceneTextures(SCENE_NAME);
    
    LOG_DEBUG("Game Scene Unload Complete");
}

void SceneGame::Update(float d_time) {
    // Regular update - runs every frame
    systemManager.ExecuteSystems(SystemManager::UpdateType::Update, registry, d_time);
}

void SceneGame::FixedUpdate(float fixed_d_time) {
    systemManager.ExecuteSystems(SystemManager::UpdateType::FixedUpdate, registry, fixed_d_time);
}

void SceneGame::AsyncUpdate(float d_time) {
    systemManager.ExecuteSystems(SystemManager::UpdateType::AsyncUpdate, registry, d_time);
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
    camera.screenSpaceCamera.offset = Vector2{ GAME_WIDTH / 2.0f, GAME_HEIGHT / 2.0f };
    camera.screenSpaceCamera.target = Vector2{ GAME_WIDTH / 2.0f, GAME_HEIGHT / 2.0f };
    camera.screenSpaceCamera.zoom = 1.0f;
    camera.screenSpaceCamera.rotation = 0.0f;
}

void SceneGame::SpawnShip() {
    shipEntity = registry.create();

    auto& transform = registry.emplace<TransformComponent>(shipEntity);
    auto& ship_image = registry.emplace<SpriteComponent>(shipEntity);

    // Load ship texture
    AssetManager::AddSceneTexture("ship", "bomber_one.png", SCENE_NAME);
    auto& texture = AssetManager::GetTexture("ship");

    // Define the ship size (make it wider than tall for a boat-like appearance)
    ship_image.size.x = PLAYER_SPRITE_SIZE * 3.0f;  // Make ship wider
    ship_image.size.y = PLAYER_SPRITE_SIZE * 1.5f;  // Make ship shorter

    // Position the ship at the bottom of the screen
    transform.position.x = VIRTUAL_WIDTH / 2.0f;
    transform.position.y = VIRTUAL_HEIGHT - ship_image.size.y/2.0f;

    // Set up the ship sprite
    ship_image.texture = &texture;
    ship_image.tint = BLUE;  // Make ship blue to distinguish it
    ship_image.origin = Vector2{ ship_image.size.x/2.0f, ship_image.size.y/2.0f };  // Set origin to center for rotation
}

void SceneGame::SpawnPlayer() {
    auto player = registry.create();

    auto& transform = registry.emplace<TransformComponent>(player);
    auto& player_comp = registry.emplace<PlayerComponent>(player);
    auto& player_image = registry.emplace<SpriteComponent>(player);
    auto& transition = registry.emplace<TransitionComponent>(player);

    AssetManager::AddSceneTexture("player", "bomber_one.png", SCENE_NAME);
    auto& texture = AssetManager::GetTexture("player");

    // Define the desired sprite size
    player_image.size.x = PLAYER_SPRITE_SIZE;
    player_image.size.y = PLAYER_SPRITE_SIZE;

    // Position the player in the center of the virtual resolution
    transform.position.x = VIRTUAL_WIDTH / 2.0f;
    transform.position.y = VIRTUAL_HEIGHT - (player_image.size.y/2.0f + 5.0f);

    // Set the source rectangle to use the entire texture
    player_image.texture = &texture;
    player_image.tint = WHITE;

    LOG_DEBUG("Player spawned with %d bullets", player_comp.bullets);
}

void SceneGame::StartTransition() {
    auto playerView = registry.view<TransformComponent, TransitionComponent>();
    auto shipView = registry.view<TransformComponent>();
    
    if (playerView.begin() == playerView.end() || !registry.valid(shipEntity)) return;

    auto playerEntity = *playerView.begin();
    auto& playerTransform = playerView.get<TransformComponent>(playerEntity);
    auto& playerTransition = playerView.get<TransitionComponent>(playerEntity);
    const auto& shipTransform = shipView.get<TransformComponent>(shipEntity);

    // Set up landing transition
    playerTransition.state = TransitionState::Landing;
    playerTransition.progress = 0.0f;
    playerTransition.targetPosition = shipTransform.position;
    playerTransition.targetRotation = 180.0f;  // Rotate to face downward

    // Pause movement and bullet systems during transition
    systemManager.PauseSystem(movementSystemID);
    systemManager.PauseSystem(bulletSystemID);
    
    isTransitioning = true;
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
