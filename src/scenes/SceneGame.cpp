//
// Created by ivanr on 4.6.2025 г..
//

#include "SceneGame.h"

#include "Log.h"
#include "Renderer.h"
#include "../../engine/AssetManager.h"

SceneGame::~SceneGame() {
    Unload();
}

void SceneGame::Load() {
    AssetManager::AddSceneTexture("background", "assets/bg.png", ToSceneId(SceneName::SceneGame));
    texture = &AssetManager::GetTexture("background");

    LOG_DEBUG("Loaded the Game Scene");
}

void SceneGame::Unload() {
    AssetManager::RemoveSceneTextures(ToSceneId(SceneName::SceneGame));
    LOG_DEBUG("Unloaded the Game Scene");
}

void SceneGame::Update(float d_time) {
    // Regular update - runs every frame
    // Good for visual updates, input handling, etc.
}

void SceneGame::FixedUpdate(float fixed_d_time) {
    // Fixed update - runs every 20ms
    // Good for physics, AI, and other deterministic systems
}

void SceneGame::AsyncUpdate(float d_time) {
    // Async update - runs on a separate thread
    // Good for heavy computations, pathfinding, etc.
}

void SceneGame::Draw() {
    render::DrawTexture(texture, 0, 0, WHITE);
}
