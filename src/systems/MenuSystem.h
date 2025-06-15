#ifndef MENUSYSTEM_H
#define MENUSYSTEM_H

#include <entt/entity/registry.hpp>
#include "components/DrawingComponent.h"
#include "KeyManager.h"
#include "Game.h"
#include "scenes/SceneGame.h"

class MenuSystem {
public:
    static void Update(entt::registry& registry, float deltaTime) {
        static float blinkTimer = 0.0f;
        static bool isVisible = true;
        static constexpr float BLINK_INTERVAL = 0.5f;  // Blink every 0.5 seconds

        // Update blinking timer
        blinkTimer += deltaTime;
        if (blinkTimer >= BLINK_INTERVAL) {
            blinkTimer = 0.0f;
            isVisible = !isVisible;
        }

        // Update text visibility
        auto view = registry.view<TextComponentPixelPerfect>();
        for (auto entity : view) {
            auto& text = view.get<TextComponentPixelPerfect>(entity);
            if (text.text == "INSERT CREDIT(S)" || text.text == "OR PRESS ENTER") {
                text.tint.a = isVisible ? 255 : 0;
            }
        }

        // Check for Enter key press
        if (key_manager::IsKeyPressed(KEY_ENTER)) {
            // Create and switch to the game scene
            Game::SwitchScene(std::make_unique<SceneGame>());
        }
    }
};

#endif //MENUSYSTEM_H 