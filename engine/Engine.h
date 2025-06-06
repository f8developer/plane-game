//
// Created by ivanr on 2.6.2025 г..
//

#ifndef ENGINE_H
#define ENGINE_H

#include <string>
#include <memory>
#include <thread>
#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <atomic>
#include <array>
#include <chrono>

#include "Defines.h"
#include "IGame.h"

using std::unique_ptr;

class Engine {
public:
    static constexpr size_t NUM_WORKER_THREADS = 4;
    static constexpr float FIXED_TIME_STEP = 0.02f;  // 20ms
    static constexpr size_t MAX_QUEUED_TASKS = 1000;  // Prevent unbounded growth
    static constexpr float MAX_ACCUMULATOR = 0.25f;   // Cap accumulator to prevent spiral of death

    DLLEX void Start(int windowWidth, int windowHeight, const std::string& windowTitle,
                    std::unique_ptr<IGame> game);
    
    // Enable or disable profiling
    DLLEX static void SetProfilingEnabled(bool enabled, int framesBeforeProfiling = 60);
    
    // Enable or disable frame stats reporting
    DLLEX static void SetFrameStatsEnabled(bool enabled);

private:
    void ProcessNonRenderingTasks();
    void ProcessFixedUpdates();
    void UpdateTargetFPS();
    void QueueAsyncTask(std::function<void()>&& task);
    void CleanupResources();
    void InitializeRenderer();
    void ShutdownRenderer();

    std::string windowTitle;
    std::unique_ptr<IGame> game;

    float accumulator = 0.0f;
    std::atomic<bool> shouldExit{false};
    std::atomic<bool> isRunning{false};

    // Fixed update thread
    std::thread fixedUpdateThread;
    std::mutex fixedUpdateMutex;
    std::condition_variable fixedUpdateCondition;

    // Non-rendering tasks (worker threads)
    std::queue<std::function<void()>> nonRenderingTasks;
    std::mutex taskMutex;
    std::condition_variable taskCondition;
    std::vector<std::thread> workerThreads;
    std::atomic<size_t> activeThreads{0};

    // Performance tracking
    struct FrameStats {
        float frameTimeAccumulator = 0.0f;
        int frameCount = 0;
        std::chrono::steady_clock::time_point lastFrameTime;
        float minFrameTime = std::numeric_limits<float>::max();
        float maxFrameTime = 0.0f;
    } frameStats;

    static int framesBeforeProfiling;
    static Engine* instance;  // For monitor callback
    static std::atomic<bool> frameStatsEnabled;  // Control frame stats reporting
};

#endif //ENGINE_H
