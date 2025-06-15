//
// Created by ivanr on 2.6.2025 г..
//

#include "Engine.h"

#include "Window.h"
#include "raylib.h"
#include "IGame.h"
#include "Log.h"
#include "Renderer.h"
#include "Profiler.h"

int Engine::framesBeforeProfiling = 60;
Engine* Engine::instance = nullptr;
std::atomic<bool> Engine::frameStatsEnabled{true};  // Enable by default

void Engine::SetProfilingEnabled(bool enabled, int framesBeforeProfiling) {
    Profiler::GetInstance().SetEnabled(enabled);
    Engine::framesBeforeProfiling = framesBeforeProfiling;
}

void Engine::SetFrameStatsEnabled(bool enabled) {
    frameStatsEnabled = enabled;
}

void Engine::UpdateTargetFPS() {
    PROFILE_SCOPE("UpdateTargetFPS");
    int currentMonitor = GetCurrentMonitor();
    int refreshRate = GetMonitorRefreshRate(currentMonitor);
    SetTargetFPS(refreshRate);
    ENGINE_LOG(LOG_INFO, "Target FPS set to %d (Monitor %d refresh rate)", refreshRate, currentMonitor);
}

void Engine::QueueAsyncTask(std::function<void()>&& task) {
    std::lock_guard lock(taskMutex);
    if (nonRenderingTasks.size() < MAX_QUEUED_TASKS) {
        nonRenderingTasks.emplace(std::move(task));
        taskCondition.notify_one();
    } else {
        ENGINE_LOG(LOG_WARNING, "Task queue full, dropping task");
    }
}

void Engine::ProcessNonRenderingTasks() {
    PROFILE_SCOPE("ProcessNonRenderingTasks");
    activeThreads++;
    
    while (!shouldExit) {
        std::unique_lock<std::mutex> lock(taskMutex);
        taskCondition.wait(lock, [this] { 
            return !nonRenderingTasks.empty() || shouldExit; 
        });

        while (!nonRenderingTasks.empty() && !shouldExit) {
            auto task = std::move(nonRenderingTasks.front());
            nonRenderingTasks.pop();
            lock.unlock();

            try {
                PROFILE_SCOPE("AsyncTask");
                task();
            } catch (const std::exception& e) {
                ENGINE_LOG(LOG_ERROR, "Async task failed: %s", e.what());
            }

            lock.lock();
        }
    }
    
    activeThreads--;
}

void Engine::ProcessFixedUpdates() {
    PROFILE_SCOPE("ProcessFixedUpdates");
    while (!shouldExit) {
        std::unique_lock<std::mutex> lock(fixedUpdateMutex);
        fixedUpdateCondition.wait(lock, [this] { 
            return accumulator >= FIXED_TIME_STEP || shouldExit; 
        });

        if (shouldExit) break;

        // Cap accumulator to prevent spiral of death
        if (accumulator > MAX_ACCUMULATOR) {
            ENGINE_LOG(LOG_WARNING, "Fixed update falling behind, capping accumulator");
            accumulator = MAX_ACCUMULATOR;
        }

        while (accumulator >= FIXED_TIME_STEP) {
            try {
                PROFILE_SCOPE("GameFixedUpdate");
                game->FixedUpdate(FIXED_TIME_STEP);
            } catch (const std::exception& e) {
                ENGINE_LOG(LOG_ERROR, "Fixed update failed: %s", e.what());
            }
            accumulator -= FIXED_TIME_STEP;
        }
    }
}

void Engine::InitializeRenderer() {
    PROFILE_SCOPE("InitializeRenderer");
    render::Initialize();
    ENGINE_LOG(LOG_INFO, "Renderer initialized");
}

void Engine::ShutdownRenderer() {
    PROFILE_SCOPE("ShutdownRenderer");
    render::Shutdown();
    ENGINE_LOG(LOG_INFO, "Renderer shut down");
}

void Engine::CleanupResources() {
    PROFILE_SCOPE("CleanupResources");
    shouldExit = true;
    isRunning = false;
    
    // Notify all threads
    taskCondition.notify_all();
    fixedUpdateCondition.notify_one();
    
    // Wait for all worker threads
    for (auto& thread : workerThreads) {
        if (thread.joinable()) {
            thread.join();
        }
    }
    workerThreads.clear();
    
    // Wait for fixed update thread
    if (fixedUpdateThread.joinable()) {
        fixedUpdateThread.join();
    }
    
    // Clear task queue
    std::queue<std::function<void()>>().swap(nonRenderingTasks);
}

void Engine::Start(int windowWidth, int windowHeight, const str& windowTitleL,
                   std::unique_ptr<IGame> gameP)
{
    try {
        PROFILE_SCOPE("EngineStart");
        Log::SetupRaylibLogging();
        ENGINE_LOG(LOG_INFO, "Engine initialization started");

        #if GDEBUG
            ENGINE_LOG(LOG_WARNING, "RUNNING A DEBUG VERSION OF THE GAME!");
        #endif

        windowTitle = windowTitleL;
        game = std::move(gameP);
        instance = this;
        isRunning = true;
        
        ENGINE_LOG(LOG_DEBUG, "Creating window (%dx%d): %s", windowWidth, windowHeight, windowTitle.c_str());
        auto window = Window(windowWidth, windowHeight, std::move(windowTitle));
        
        UpdateTargetFPS();
        ENGINE_LOG(LOG_DEBUG, "Window created successfully");

        // Initialize renderer
        InitializeRenderer();

        // Initialize frame stats
        frameStats.lastFrameTime = std::chrono::steady_clock::now();

        // Create worker threads
        {
            PROFILE_SCOPE("ThreadInitialization");
            workerThreads.reserve(NUM_WORKER_THREADS);
            for (size_t i = 0; i < NUM_WORKER_THREADS; ++i) {
                workerThreads.emplace_back(&Engine::ProcessNonRenderingTasks, this);
            }
            fixedUpdateThread = std::thread(&Engine::ProcessFixedUpdates, this);
        }

        ENGINE_LOG(LOG_INFO, "Loading game...");
        {
            PROFILE_SCOPE("GameLoad");
            game->Load();
        }
        ENGINE_LOG(LOG_INFO, "Game loaded successfully");

        ENGINE_LOG(LOG_DEBUG, "Starting main game loop");

        int lastMonitor = GetCurrentMonitor();

        while (!window.ShouldClose() && isRunning) {
            PROFILE_SCOPE("MainLoop");
            
            // Calculate frame time using steady clock for better accuracy
            auto currentTime = std::chrono::steady_clock::now();
            float deltaTime = std::chrono::duration<float>(currentTime - frameStats.lastFrameTime).count();
            frameStats.lastFrameTime = currentTime;
            
            // Update frame stats
            frameStats.frameTimeAccumulator += deltaTime;
            frameStats.frameCount++;
            frameStats.minFrameTime = std::min(frameStats.minFrameTime, deltaTime);
            frameStats.maxFrameTime = std::max(frameStats.maxFrameTime, deltaTime);
            
            // Check for monitor changes
            int currentMonitor = GetCurrentMonitor();
            if (currentMonitor != lastMonitor) {
                UpdateTargetFPS();
                lastMonitor = currentMonitor;
            }
            
            // Update accumulator and notify fixed update thread
            {
                PROFILE_SCOPE("FixedUpdateAccumulation");
                std::lock_guard lock(fixedUpdateMutex);
                accumulator += deltaTime;
                if (accumulator >= FIXED_TIME_STEP) {
                    fixedUpdateCondition.notify_one();
                }
            }

            // Queue async update
            QueueAsyncTask([this, deltaTime]() {
                PROFILE_SCOPE("AsyncUpdate");
                game->AsyncUpdate(deltaTime);
            });

            // Game update and rendering
            {
                PROFILE_SCOPE("GameUpdate");
                game->Update(deltaTime);
            }

            {
                PROFILE_SCOPE("Rendering");
                render::BeginDraw();
                render::Clear();
                game->Draw();
                render::EndDraw();
            }

            // Print frame stats
            if (frameStats.frameCount >= framesBeforeProfiling && frameStatsEnabled) {
                float avgFrameTime = frameStats.frameTimeAccumulator / frameStats.frameCount;
                ENGINE_LOG(LOG_INFO, "Frame Stats - Avg: %.3f ms, Min: %.3f ms, Max: %.3f ms, FPS: %.1f", 
                          avgFrameTime * 1000.0f, 
                          frameStats.minFrameTime * 1000.0f,
                          frameStats.maxFrameTime * 1000.0f,
                          1.0f / avgFrameTime);
                
                Profiler::GetInstance().PrintFrameStats();
                
                // Reset stats
                frameStats = FrameStats{};
                frameStats.lastFrameTime = currentTime;
            }
        }

        ENGINE_LOG(LOG_DEBUG, "Unloading game...");
        {
            PROFILE_SCOPE("GameUnload");
            game->Unload();
        }

        ShutdownRenderer();
        CleanupResources();
        ENGINE_LOG(LOG_DEBUG, "Engine shutdown completed successfully");
    }
    catch (const std::exception& e) {
        ENGINE_LOG(LOG_FATAL, "Engine exception: %s", e.what());
        CleanupResources();
    }
    catch (...) {
        ENGINE_LOG(LOG_FATAL, "Unknown engine exception occurred");
        CleanupResources();
    }

    ENGINE_LOG(LOG_DEBUG, "Flushing logs and shutting down logging system");
    Log::Shutdown();
}