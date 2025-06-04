//
// Created by ivanr on 4.6.2025 г..
//

#include "SceneMainMenu.h"
#include "Log.h"
#include "Game.h"
#include "KeyManager.h"
#include "scenes/SceneGame.h"
#include "raylib.h"

void SceneMainMenu::Load() {
    menuText = "INSERT CREDIT(S) TO START";
    LOG_DEBUG("Loaded the Main Menu scene");
}

void SceneMainMenu::Update(float d_time) {
    // Update blinking timer
    blinkTimer += d_time;
    if (blinkTimer >= BLINK_INTERVAL) {
        blinkTimer = 0.0f;
        isTextVisible = !isTextVisible;
    }

    // Check for Enter key press
    if (key_manager::IsKeyPressed(KEY_ENTER)) {
        // Create and switch to the game scene
        auto gameScene = std::make_unique<SceneGame>();
        Game::SwitchScene(std::move(gameScene));
    }
}

void SceneMainMenu::Draw() {
    // Get screen dimensions
    int screenWidth = render::GetScreenWidth();
    int screenHeight = render::GetScreenHeight();
    
    // Draw the game title
    const char* gameTitle = "PLANE GAME";
    int titleFontSize = 60;
    int titleWidth = render::MeasureText(gameTitle, titleFontSize);
    render::DrawText(gameTitle, 
        (screenWidth - titleWidth) / 2,  // Center horizontally
        screenHeight / 4,                // Position at 1/4 of screen height
        titleFontSize, 
        YELLOW);

    // Draw the blinking start text
    if (isTextVisible) {
        int textFontSize = 40;
        int textWidth = render::MeasureText(menuText.c_str(), textFontSize);
        render::DrawText(menuText.c_str(),
            (screenWidth - textWidth) / 2,  // Center horizontally
            screenHeight * 2 / 3,           // Position at 2/3 of screen height
            textFontSize,
            WHITE);
            
        // Draw the "OR PRESS ENTER" text below
        const char* enterText = "OR PRESS ENTER";
        int enterWidth = render::MeasureText(enterText, textFontSize);
        render::DrawText(enterText,
            (screenWidth - enterWidth) / 2,  // Center horizontally
            screenHeight * 2 / 3 + textFontSize + 10,  // Position below the main text with some spacing
            textFontSize,
            WHITE);
    }
}
