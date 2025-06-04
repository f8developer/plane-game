//
// Created by ivanr on 4.6.2025 г..
//

#ifndef SCENEMAINMENU_H
#define SCENEMAINMENU_H

#include "IScene.h"
#include "Renderer.h"
#include <string>
#include <chrono>

class SceneMainMenu final : public IScene {
public:
    void Load() override;
    void Update(float d_time) override;
    void Draw() override;

private:
    std::string menuText;
    float blinkTimer = 0.5f;
    bool isTextVisible = true;
    static constexpr float BLINK_INTERVAL = 0.5f; // Blink every 0.5 seconds
};

#endif //SCENEMAINMENU_H
