#include "engine/core/logger.h"
#include <iostream>

namespace engine::core {

std::vector<LogEntry> Logger::s_history;
std::mutex Logger::s_mutex;

void Logger::Push(LogLevel level, const std::string& msg) {
    std::lock_guard<std::mutex> lock(s_mutex);
    s_history.push_back({level, msg});
    if (s_history.size() > kMaxHistory) {
        s_history.erase(s_history.begin());
    }
}

void Logger::Info(const std::string& msg) {
    std::cout << "[INFO] " << msg << "\n";
    Push(LogLevel::Info, msg);
}

void Logger::Error(const std::string& msg) {
    std::cerr << "[ERROR] " << msg << "\n";
    Push(LogLevel::Error, msg);
}

std::vector<LogEntry> Logger::GetHistory() {
    std::lock_guard<std::mutex> lock(s_mutex);
    return s_history; // tra ve ban sao, tranh giu lock lau khi UI dang doc/ve
}

void Logger::ClearHistory() {
    std::lock_guard<std::mutex> lock(s_mutex);
    s_history.clear();
}

} // namespace engine::core