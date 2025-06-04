//
// Created by ivanr on 2.6.2025 г..
//

#ifndef LOG_H
#define LOG_H

#include <sstream>
#include <fstream>
#include <mutex>
#include <memory>
#include <thread>
#include <queue>
#include <condition_variable>
#include <atomic>
#include "Defines.h"
#include "raylib.h"

const str GAME_LOG_FILE = "runtime.log";

class Log {
public:
    // Singleton pattern for thread safety
    DLLEX static Log& Instance();

    Log(const Log&) = delete;
    Log& operator=(const Log&) = delete;
    Log(Log&&) = delete;
    Log& operator=(Log&&) = delete;

    ~Log();

    DLLEX void WriteLog(TraceLogLevel level, const std::string& message);
    DLLEX void WriteLog(TraceLogLevel level, const char* format, ...);

    static void EngineLog(TraceLogLevel level, const char *format, ...);
    static void SetupRaylibLogging();
    static void Restart();
    static void Shutdown();

    // RAII helper
    class LogStream {
    public:
        explicit LogStream(TraceLogLevel level);
        ~LogStream();

        template<typename T>
        LogStream& operator<<(const T& value) {
            if (shouldLog) {
                stream_ << value;
            }
            return *this;
        }

    private:
        std::ostringstream stream_;
        TraceLogLevel level_;
        bool shouldLog;
    };

private:
    Log();

    void InitializeFile();
    void ProcessLogQueue();
    void WriteToFile(const std::string& message);

    struct LogEntry {
        std::string message;
        TraceLogLevel level;
        std::string timestamp;
    };

    std::mutex fileMutex_;
    std::ofstream logFile_;

    std::queue<LogEntry> logQueue_;
    std::mutex queueMutex_;
    std::condition_variable queueCondition_;
    std::unique_ptr<std::thread> writerThread_;
    std::atomic<bool> shouldExit_{false};
    bool asyncMode_{false};

    static std::unique_ptr<Log> instance_;
    static std::once_flag initFlag_;

    static str GetLabel(TraceLogLevel level);
    static std::string GetTime();
    static void LogCallback(int level, const char* text, va_list args);
};

#if GDEBUG
constexpr TraceLogLevel MIN_LOG_LEVEL = LOG_ALL;
#else
constexpr TraceLogLevel MIN_LOG_LEVEL = LOG_DEBUG;
#endif

#define LOG(level)                                  \
    if (level <= MIN_LOG_LEVEL)                      \
        ;                                           \
    else                                            \
        Log::LogStream(level)

#define ENGINE_LOG(level, ...) \
        Log::EngineLog(level, __VA_ARGS__)


#define LOG_TRACE(...) Log::Instance().WriteLog(LOG_TRACE, __VA_ARGS__)
#define LOG_DEBUG(...) Log::Instance().WriteLog(LOG_DEBUG, __VA_ARGS__)
#define LOG_INFO(...) Log::Instance().WriteLog(LOG_INFO, __VA_ARGS__)
#define LOG_WARNING(...) Log::Instance().WriteLog(LOG_WARNING, __VA_ARGS__)
#define LOG_ERROR(...) Log::Instance().WriteLog(LOG_ERROR, __VA_ARGS__)
#define LOG_FATAL(...) Log::Instance().WriteLog(LOG_FATAL, __VA_ARGS__)

#endif //LOG_H