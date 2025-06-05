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

#include "Defines.h"
#include "IGame.h"

using std::unique_ptr;

class Engine {
public:
    static constexpr size_t NUM_WORKER_THREADS = 4;
    static constexpr float FIXED_TIME_STEP = 0.02f;  // 20ms

    DLLEX void Start(int windowWidth, int windowHeight, const std::string& windowTitle,
                    std::unique_ptr<IGame> game);
    
    // Enable or disable profiling
    DLLEX static void SetProfilingEnabled(bool enabled, int framesBeforeProfiling = 60);

private:
    void ProcessNonRenderingTasks();
    void ProcessFixedUpdates(float deltaTime);

    std::string windowTitle;
    std::unique_ptr<IGame> game;

    float accumulator = 0.0f;
    std::atomic<bool> shouldExit{false};

    // Non-rendering tasks (worker threads)
    std::queue<std::function<void()>> nonRenderingTasks;
    std::mutex taskMutex;
    std::condition_variable taskCondition;
    std::vector<std::thread> workerThreads;

    static int framesBeforeProfiling;
};

#endif //ENGINE_H
