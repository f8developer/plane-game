//
// Created by ivanr on 4.6.2025 г..
//

#include "SceneMainMenu.h"

#include "Log.h"
#include "Game.h"
#include "KeyManager.h"
#include "scenes/SceneGame.h"
#include "raylib.h"
#include "GameConfig.h"
#include "systems/MenuSystem.h"
#include "systems/RenderSystem.h"

void SceneMainMenu::Load() {
    // Register systems with the system manager
    systemManager.AddSystem(
        [](entt::registry& reg, float dt) { MenuSystem::Update(reg, dt); },
        SystemManager::UpdateType::Update
    );
    
    systemManager.AddSystem(
        [](entt::registry& reg, float dt) { RenderSystem::DrawPixelPerfect(reg); },
        SystemManager::UpdateType::Draw
    );

    SetupMenuEntities();
    LOG_DEBUG("Loaded the Main Menu scene");
}

void SceneMainMenu::Unload() {
    systemManager.ClearAllSystems();
    LOG_DEBUG("Unloaded the Main Menu scene");
}

void SceneMainMenu::Update(float d_time) {
    systemManager.ExecuteSystems(SystemManager::UpdateType::Update, registry, d_time);
}

void SceneMainMenu::Draw() {
    systemManager.ExecuteSystems(SystemManager::UpdateType::Draw, registry, 0.0f);
}

void SceneMainMenu::SetupMenuEntities() {
    // Create camera entity
    auto cameraEntity = registry.create();
    auto& camera = registry.emplace<PixelPerfectCameraComponent>(cameraEntity);
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

    // Create title entity
    auto titleEntity = registry.create();
    auto& titleTransform = registry.emplace<TransformComponent>(titleEntity);
    auto& titleText = registry.emplace<TextComponent>(titleEntity);
    
    titleText.text = GAME_TITLE;
    titleText.fontSize = UI_TITLE_FONT_SIZE;
    titleText.color = DARKPURPLE;
    
    // Center the title
    int titleWidth = render::MeasureText(std::string(titleText.text), titleText.fontSize);
    titleTransform.position = Vector2{
        (VIRTUAL_WIDTH - titleWidth) / 2.0f,
        VIRTUAL_HEIGHT / 4.0f
    };

    // Create start text entity
    auto startTextEntity = registry.create();
    auto& startTransform = registry.emplace<TransformComponent>(startTextEntity);
    auto& startText = registry.emplace<TextComponent>(startTextEntity);
    
    startText.text = "INSERT CREDIT(S)";
    startText.fontSize = UI_START_TEXT_FONT_SIZE;
    startText.color = BLACK;
    
    // Center the start text
    int startWidth = render::MeasureText(std::string(startText.text), startText.fontSize);
    startTransform.position = Vector2{
        (VIRTUAL_WIDTH - startWidth) / 2.0f,
        VIRTUAL_HEIGHT * 2.0f / 3.0f
    };

    // Create enter text entity
    auto enterTextEntity = registry.create();
    auto& enterTransform = registry.emplace<TransformComponent>(enterTextEntity);
    auto& enterText = registry.emplace<TextComponent>(enterTextEntity);
    
    enterText.text = "OR PRESS ENTER";
    enterText.fontSize = UI_ENTER_TEXT_FONT_SIZE;
    enterText.color = BLACK;
    
    // Center the enter text below the start text
    int enterWidth = render::MeasureText(std::string(enterText.text), enterText.fontSize);
    enterTransform.position = Vector2{
        (VIRTUAL_WIDTH - enterWidth) / 2.0f,
        VIRTUAL_HEIGHT * 2.0f / 3.0f + startText.fontSize
    };
}
