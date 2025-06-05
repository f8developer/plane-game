//
// Created by ivanr on 4.6.2025 г..
//

#include "SceneGame.h"

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
        [](entt::registry& reg, float dt) { RenderSystem::DrawSprites(reg); },
        SystemManager::UpdateType::Draw
    );

    SpawnPlayer();
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
    // Good for physics, AI, and other deterministic systems
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

void SceneGame::SpawnPlayer() {
    auto player = registry.create();

    auto& transform = registry.emplace<TransformComponent>(player);
    auto& player_comp = registry.emplace<PlayerComponent>(player);
    auto& player_immage = registry.emplace<SpriteComponent>(player);

    const int screenWidth = render::GetScreenWidth();
    const int screenHeight = render::GetScreenHeight();

    transform.position.x = static_cast<float>(screenWidth) / 2;
    transform.position.y = static_cast<float>(screenHeight) / 2;

    AssetManager::AddSceneTexture("player", "bg.png", SCENE_NAME);
    player_immage.texture = &AssetManager::GetTexture("player");
}
