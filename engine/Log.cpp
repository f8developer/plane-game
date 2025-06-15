#include "Log.h"
#include "magic_enum/magic_enum.hpp"
#include <iostream>
#include <cstdarg>
#include <iomanip>
#include <chrono>
#include <sstream>
#include <format>

std::unique_ptr<Log> Log::instance_ = nullptr;
std::once_flag Log::initFlag_;

Log& Log::Instance() {
    std::call_once(initFlag_, []() {
        instance_ = std::unique_ptr<Log>(new Log());
    });
    return *instance_;
}

Log::Log() {
    #if GDEBUG
        asyncMode_ = false;
    #else
        asyncMode_ = true;
    #endif

    InitializeFile();

    if (asyncMode_) {
        writerThread_ = std::make_unique<std::thread>(&Log::ProcessLogQueue, this);
    }
}

Log::~Log() {
    Shutdown();
}

void Log::InitializeFile() {
    std::lock_guard<std::mutex> lock(fileMutex_);
    logFile_.open(GAME_LOG_FILE, std::ios::out | std::ios::trunc);
    if (!logFile_.is_open()) {
        std::cerr << "Failed to open log file: " << GAME_LOG_FILE << std::endl;
    }
}

void Log::WriteLog(TraceLogLevel level, const std::string& message) {
    std::string timestamp = GetTime();
    std::string label = GetLabel(level);

    std::string fullMessage = timestamp + " " + label + " -> " + message;

    // Print to console
    std::cout << fullMessage << std::endl;

    if (asyncMode_) {
        {
            std::lock_guard<std::mutex> lock(queueMutex_);
            logQueue_.push({fullMessage, level, timestamp});
        }
        queueCondition_.notify_one();
    } else {
        WriteToFile(fullMessage);
    }
}

void Log::WriteLog(TraceLogLevel level, const char* format, ...) {
    if (level <= MIN_LOG_LEVEL)
        return; // Skip logs above max level

    va_list args;
    va_start(args, format);

    // Get required buffer size
    va_list args_copy;
    va_copy(args_copy, args);
    int size = std::vsnprintf(nullptr, 0, format, args_copy);
    va_end(args_copy);

    if (size < 0) {
        va_end(args);
        return;
    }

    std::string buffer(size + 1, '\0');
    std::vsnprintf(buffer.data(), buffer.size(), format, args);
    va_end(args);

    buffer.resize(size); // Remove null terminator
    WriteLog(level, buffer);
}

void Log::WriteToFile(const std::string& message) {
    std::lock_guard<std::mutex> lock(fileMutex_);
    if (logFile_.is_open()) {
        logFile_ << message << std::endl;
        logFile_.flush();
    }
}

void Log::ProcessLogQueue() {
    while (!shouldExit_) {
        std::unique_lock<std::mutex> lock(queueMutex_);
        queueCondition_.wait(lock, [this] {
            return !logQueue_.empty() || shouldExit_;
        });

        while (!logQueue_.empty()) {
            LogEntry entry = std::move(logQueue_.front());
            logQueue_.pop();
            lock.unlock();

            WriteToFile(entry.message);

            lock.lock();
        }
    }
}

void Log::LogCallback(int level, const char *text, va_list args) {
    char buffer[1024];
    std::vsnprintf(buffer, sizeof(buffer), text, args);
    Instance().WriteLog(static_cast<TraceLogLevel>(level), "%s", buffer);
}

void Log::EngineLog(const TraceLogLevel level, const char *format, ...) {
    if (level <= MIN_LOG_LEVEL)
        return; // Skip logs above max level

    va_list args;
    va_start(args, format);

    va_list args_copy;
    va_copy(args_copy, args);
    int size = std::vsnprintf(nullptr, 0, format, args_copy);
    va_end(args_copy);

    if (size < 0) {
        va_end(args);
        return;
    }

    std::string buffer(size + 1, '\0');
    std::vsnprintf(buffer.data(), buffer.size(), format, args);
    va_end(args);

    buffer.resize(size);

    // Compose full message without fmt::format
    std::string fullMsg = std::string("ENGINE: ") + buffer;
    Instance().WriteLog(level, fullMsg);
}

void Log::SetupRaylibLogging() {
    SetTraceLogCallback(LogCallback);
}

void Log::Restart() {
    auto& instance = Instance();
    std::lock_guard<std::mutex> lock(instance.fileMutex_);

    instance.logFile_.close();
    instance.logFile_.open(GAME_LOG_FILE, std::ios::out | std::ios::trunc);

    if (!instance.logFile_.is_open()) {
        std::cerr << "Failed to reopen log file: " << GAME_LOG_FILE << std::endl;
    }
}

void Log::Shutdown() {
    if (instance_) {
        auto& inst = *instance_;

        if (inst.writerThread_) {
            inst.shouldExit_ = true;
            inst.queueCondition_.notify_all();
            if (inst.writerThread_->joinable()) {
                inst.writerThread_->join();
            }
            inst.writerThread_.reset();
        }

        std::lock_guard<std::mutex> lock(inst.fileMutex_);
        if (inst.logFile_.is_open()) {
            inst.logFile_.close();
        }
    }
}

std::string Log::GetLabel(const TraceLogLevel level) {
    auto name = magic_enum::enum_name(level);
    return std::string(name.substr(4));  // Assuming label starts at pos 4
}

std::string Log::GetTime() {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;

    std::ostringstream oss;
    oss << std::put_time(std::localtime(&time_t), "%H:%M:%S");
    oss << '.' << std::setfill('0') << std::setw(3) << ms.count();

    return oss.str();
}

// LogStream implementation
Log::LogStream::LogStream(const TraceLogLevel level)
    : level_(level), shouldLog(level <= MIN_LOG_LEVEL) {
}

Log::LogStream::~LogStream() {
    if (shouldLog && !stream_.str().empty()) {
        Log::Instance().WriteLog(level_, stream_.str());
    }
}
