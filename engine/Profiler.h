#ifndef PROFILER_H
#define PROFILER_H

#include <string>
#include <unordered_map>
#include <chrono>
#include <vector>
#include <mutex>
#include "Log.h"

// Helper macros for concatenation
#define CONCAT_IMPL(x, y) x##y
#define CONCAT(x, y) CONCAT_IMPL(x, y)

class Profiler {
public:
    struct ProfileData {
        double totalTime = 0.0;
        double minTime = std::numeric_limits<double>::max();
        double maxTime = 0.0;
        size_t callCount = 0;
    };

    static Profiler& GetInstance() {
        static Profiler instance;
        return instance;
    }

    class ScopedTimer {
    public:
        ScopedTimer(const std::string& name) : name(name) {
            if (Profiler::GetInstance().IsEnabled()) {
                startTime = std::chrono::high_resolution_clock::now();
            }
        }

        ~ScopedTimer() {
            if (Profiler::GetInstance().IsEnabled()) {
                auto endTime = std::chrono::high_resolution_clock::now();
                auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count() / 1000.0;
                Profiler::GetInstance().RecordTime(name, duration);
            }
        }

    private:
        std::string name;
        std::chrono::time_point<std::chrono::high_resolution_clock> startTime;
    };

    void RecordTime(const std::string& name, double milliseconds) {
        if (!enabled) return;
        
        std::lock_guard<std::mutex> lock(mutex);
        auto& data = profileData[name];
        data.totalTime += milliseconds;
        data.minTime = std::min(data.minTime, milliseconds);
        data.maxTime = std::max(data.maxTime, milliseconds);
        data.callCount++;
    }

    void PrintFrameStats() {
        if (!enabled) return;
        
        std::lock_guard<std::mutex> lock(mutex);
        ENGINE_LOG(LOG_INFO, "=== Frame Performance Stats ===");
        for (const auto& [name, data] : profileData) {
            double avgTime = data.totalTime / data.callCount;
            ENGINE_LOG(LOG_INFO, "%s: Avg=%.3fms, Min=%.3fms, Max=%.3fms, Calls=%zu",
                      name.c_str(), avgTime, data.minTime, data.maxTime, data.callCount);
        }
        ENGINE_LOG(LOG_INFO, "=============================");
        profileData.clear();
    }

    void SetEnabled(bool value) {
        enabled = value;
        if (!enabled) {
            std::lock_guard<std::mutex> lock(mutex);
            profileData.clear();
        }
    }

    bool IsEnabled() const {
        return enabled;
    }

private:
    Profiler() = default;
    std::unordered_map<std::string, ProfileData> profileData;
    std::mutex mutex;
    std::atomic<bool> enabled{true};
};

#define PROFILE_SCOPE(name) Profiler::ScopedTimer CONCAT(profiler_timer_, __LINE__)(name)

#endif // PROFILER_H 