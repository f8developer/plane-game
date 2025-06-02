//
// Created by ivanr on 2.6.2025 г..
//

#include "Engine.h"

#include "Window.h"
#include "raylib.h"
#include "IGame.h"

void Engine::Start(int windowWidth, int windowHeight, const std::string& windowTitleL,
                    unique_ptr<IGame> gameP)
{
    windowTitle = windowTitleL;
    game = std::move(gameP);

    auto window = Window(windowWidth, windowHeight, std::move(windowTitle));
    SetTargetFPS(60);

    game->Load();

    while (!window.ShouldClose()) {

        game->Update(0);

        renderer.BeginDraw();
        renderer.Clear();

        game->Draw(renderer);

        renderer.EndDraw();
    }
}
