//
// Created by ivanr on 2.6.2025 г..
//

#include "Engine.h"

#include "Window.h"
#include "raylib.h"
#include "IGame.h"
#include "Log.h"

void Engine::Start(int windowWidth, int windowHeight, const std::string& windowTitleL,
                   unique_ptr<IGame> gameP)
{
    try {
        Log::SetupRaylibLogging();
        ENGINE_LOG(LOG_INFO, "Engine initialization started");

        #if GDEBUG
            ENGINE_LOG(LOG_WARNING, "RUNNING A DEBUG VERSION OF THE GAME!");
        #endif

        windowTitle = windowTitleL;
        game = std::move(gameP);

        ENGINE_LOG(LOG_INFO, "Creating window (%dx%d): %s", windowWidth, windowHeight, windowTitle.c_str());
        auto window = Window(windowWidth, windowHeight, std::move(windowTitle));

        SetTargetFPS(60);
        ENGINE_LOG(LOG_INFO, "Target FPS set to 60");
        ENGINE_LOG(LOG_INFO, "Window created successfully");

        ENGINE_LOG(LOG_INFO, "Loading game...");
        game->Load();
        ENGINE_LOG(LOG_INFO, "Game loaded successfully");

        ENGINE_LOG(LOG_INFO, "Starting main game loop");

        while (!window.ShouldClose()) {
            game->Update(0);

            renderer.BeginDraw();
            renderer.Clear();

            game->Draw(renderer);

            renderer.EndDraw();

        }

        ENGINE_LOG(LOG_INFO, "Unloading game...");
        game->Unload();

        ENGINE_LOG(LOG_INFO, "Engine shutdown completed successfully");
    }
    catch (const std::exception& e) {
        ENGINE_LOG(LOG_FATAL, "Engine exception: %s", e.what());
    }
    catch (...) {
        ENGINE_LOG(LOG_FATAL, "Unknown engine exception occurred");
    }

    // CRITICAL: Ensure all logs are written before program exits
    ENGINE_LOG(LOG_INFO, "Flushing logs and shutting down logging system");
    Log::Shutdown();
}