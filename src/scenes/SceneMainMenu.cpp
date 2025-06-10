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
#include "AssetManager.h"

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

    // Load the Lander font
    AssetManager::AddSceneFont("lander", FONT_LANDER, SCENE_NAME, UI_MAIN_MENU_FONT_SIZE);
    AssetManager::AddSceneFont("lander_bold", FONT_LANDER_BOLD, SCENE_NAME, UI_MAIN_MENU_FONT_SIZE);

    SetupMenuEntities();
    LOG_DEBUG("Loaded the Main Menu scene");
}

void SceneMainMenu::Unload() {
    systemManager.ClearAllSystems();
    AssetManager::RemoveSceneTextures(SCENE_NAME);
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

    // Get the Lander font
    const Font& landerFont = AssetManager::GetFont("lander");
    const Font& landerBoldFont = AssetManager::GetFont("lander_bold");

    // Create title entity
    auto titleEntity = registry.create();
    auto& titleTransform = registry.emplace<TransformComponent>(titleEntity);
    auto& titleText = registry.emplace<TextComponentPixelPerfect>(titleEntity);
    
    titleText.text = GAME_TITLE;
    titleText.font = landerBoldFont;
    titleText.fontSize = UI_MAIN_MENU_FONT_SIZE * 2;
    titleText.spacing = 1.0f;
    titleText.tint = DARKPURPLE;
    
    // Center the title using MeasureTextEx with the Lander font
    Vector2 titleSize = MeasureTextEx(landerFont, std::string(titleText.text).c_str(), titleText.fontSize, titleText.spacing);
    titleTransform.position = Vector2{
        (VIRTUAL_WIDTH - titleSize.x) / 2.0f,
        VIRTUAL_HEIGHT / 3.0f
    };

    // Create start text entity
    auto startTextEntity = registry.create();
    auto& startTransform = registry.emplace<TransformComponent>(startTextEntity);
    auto& startText = registry.emplace<TextComponentPixelPerfect>(startTextEntity);
    
    startText.text = "INSERT CREDIT(S)";
    startText.font = landerBoldFont;
    startText.fontSize = UI_MAIN_MENU_FONT_SIZE;
    startText.spacing = 1.0f;
    startText.tint = BLACK;
    
    // Center the start text using MeasureTextEx with the Lander font
    Vector2 startSize = MeasureTextEx(landerFont, std::string(startText.text).c_str(), startText.fontSize, startText.spacing);
    startTransform.position = Vector2{
        (VIRTUAL_WIDTH - startSize.x) / 2.0f,
        VIRTUAL_HEIGHT * 2.0f / 3.0f - 2.0f
    };

    // Create enter text entity
    auto enterTextEntity = registry.create();
    auto& enterTransform = registry.emplace<TransformComponent>(enterTextEntity);
    auto& enterText = registry.emplace<TextComponentPixelPerfect>(enterTextEntity);
    
    enterText.text = "OR PRESS ENTER";
    enterText.font = landerFont;
    enterText.fontSize = UI_MAIN_MENU_FONT_SIZE;
    enterText.spacing = 1.0f;
    enterText.tint = BLACK;
    
    // Center the enter text using MeasureTextEx with the Lander font
    Vector2 enterSize = MeasureTextEx(landerFont, std::string(enterText.text).c_str(), enterText.fontSize, enterText.spacing);
    enterTransform.position = Vector2{
        (VIRTUAL_WIDTH - enterSize.x) / 2.0f,
        VIRTUAL_HEIGHT * 2.0f / 3.0f + startText.fontSize
    };
}
