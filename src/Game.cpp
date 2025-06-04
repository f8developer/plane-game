//
// Created by ivanr on 2.6.2025 г..
//

#include "Game.h"

#include <ranges>
#include <future>

#include "SceneMainMenu.h"
#include "scenes/SceneGame.h"

// Initialize static members
std::vector<std::unique_ptr<IScene>> Game::scenes;
Game* Game::instance = nullptr;

void Game::Load() {
    instance = this;
    auto startScene = std::make_unique<SceneMainMenu>();
    AddScene(std::move(startScene));
}

void Game::Unload() {
    for (auto&& scene : std::ranges::reverse_view(scenes)) {
        scene->Unload();
    }
    scenes.clear();
    instance = nullptr;
}

void Game::Update(const float d_time) {
    for (auto&& scene : std::ranges::reverse_view(scenes)) {
        scene->Update(d_time);
        if (scene->GetLocking()) break;
    }
}

void Game::FixedUpdate(const float fixed_d_time) {
    for (auto&& scene : std::ranges::reverse_view(scenes)) {
        scene->FixedUpdate(fixed_d_time);
        if (scene->GetLocking()) break;
    }
}

void Game::AsyncUpdate(const float d_time) {
    // This runs in worker threads - only non-rendering logic!
    for (auto&& scene : std::ranges::reverse_view(scenes)) {
        scene->AsyncUpdate(d_time);
        if (scene->GetLocking()) break;
    }
}

void Game::Draw() {
    for (auto&& scene : std::ranges::reverse_view(scenes)) {
        scene->Draw();
        if (scene->GetTransparent()) break;
    }
}

void Game::AddScene(std::unique_ptr<IScene> newScene) {
    scenes.emplace_back(std::move(newScene));
    scenes.back()->Load();
}

void Game::RemoveTopScene() {
    if (scenes.empty()) return;
    scenes.back()->Unload();
    scenes.pop_back();
}

void Game::SwitchScene(std::unique_ptr<IScene> newScene) {
    if (!scenes.empty()) {
        RemoveTopScene();
    }
    AddScene(std::move(newScene));
}
