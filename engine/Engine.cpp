//
// Created by ivanr on 2.6.2025 г..
//

#include "Engine.h"

#include "Window.h"
#include "raylib.h"
#include "IGame.h"
#include "Log.h"
#include "Renderer.h"

void Engine::ProcessNonRenderingTasks() {
    while (!shouldExit) {
        std::unique_lock<std::mutex> lock(taskMutex);
        taskCondition.wait(lock, [this] { 
            return !nonRenderingTasks.empty() || shouldExit; 
        });

        while (!nonRenderingTasks.empty()) {
            auto task = std::move(nonRenderingTasks.front());
            nonRenderingTasks.pop();
            lock.unlock();

            // Execute non-rendering task
            task();

            lock.lock();
        }
    }
}

void Engine::ProcessFixedUpdates(float deltaTime) {
    accumulator += deltaTime;

    while (accumulator >= FIXED_TIME_STEP) {
        game->FixedUpdate(FIXED_TIME_STEP);
        accumulator -= FIXED_TIME_STEP;
    }
}

void Engine::Start(int windowWidth, int windowHeight, const str& windowTitleL,
                   std::unique_ptr<IGame> gameP)
{
    try {
        Log::SetupRaylibLogging();
        ENGINE_LOG(LOG_INFO, "Engine initialization started");

        #if GDEBUG
            ENGINE_LOG(LOG_WARNING, "RUNNING A DEBUG VERSION OF THE GAME!");
        #endif

        windowTitle = windowTitleL;
        game = std::move(gameP);
        
        ENGINE_LOG(LOG_DEBUG, "Creating window (%dx%d): %s", windowWidth, windowHeight, windowTitle.c_str());
        auto window = Window(windowWidth, windowHeight, std::move(windowTitle));

        SetTargetFPS(60);
        ENGINE_LOG(LOG_INFO, "Target FPS set to 60");
        ENGINE_LOG(LOG_DEBUG, "Window created successfully");

        // Create worker threads for NON-RENDERING tasks only
        for (size_t i = 0; i < NUM_WORKER_THREADS; ++i) {
            workerThreads.emplace_back(&Engine::ProcessNonRenderingTasks, this);
        }

        ENGINE_LOG(LOG_INFO, "Loading game...");
        game->Load();
        ENGINE_LOG(LOG_INFO, "Game loaded successfully");

        ENGINE_LOG(LOG_DEBUG, "Starting main game loop");

        while (!window.ShouldClose()) {
            const float deltaTime = GetFrameTime();
            
            ProcessFixedUpdates(deltaTime);

            // Queue non-rendering async work
            {
                std::lock_guard<std::mutex> lock(taskMutex);
                nonRenderingTasks.push([this, deltaTime]() {
                    // Only non-rendering logic here!
                    game->AsyncUpdate(deltaTime);
                    // NO DRAWING CALLS IN WORKER THREADS!
                });
            }
            taskCondition.notify_one();

            // All rendering happens on main thread
            game->Update(deltaTime);

            BeginDrawing();
            ClearBackground(BLACK);
            game->Draw();
            EndDrawing();
        }

        // Cleanup threads
        shouldExit = true;
        taskCondition.notify_all();
        for (auto& thread : workerThreads) {
            if (thread.joinable()) {
                thread.join();
            }
        }

        ENGINE_LOG(LOG_DEBUG, "Unloading game...");
        game->Unload();

        ENGINE_LOG(LOG_DEBUG, "Engine shutdown completed successfully");
    }
    catch (const std::exception& e) {
        ENGINE_LOG(LOG_FATAL, "Engine exception: %s", e.what());
    }
    catch (...) {
        ENGINE_LOG(LOG_FATAL, "Unknown engine exception occurred");
    }

    // CRITICAL: Ensure all logs are written before program exits
    ENGINE_LOG(LOG_DEBUG, "Flushing logs and shutting down logging system");
    Log::Shutdown();
}