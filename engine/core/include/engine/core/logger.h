#pragma once
#include <string>
#include <vector>
#include <mutex>

namespace engine::core {

enum class LogLevel { Info, Error };

struct LogEntry {
    LogLevel level;
    std::string message;
};

class Logger {
public:
    static void Info(const std::string& msg);
    static void Error(const std::string& msg);

    // Dung cho Console panel: lay ban sao lich su log, thread-safe
    static std::vector<LogEntry> GetHistory();
    static void ClearHistory();

private:
    static void Push(LogLevel level, const std::string& msg);

    static std::vector<LogEntry> s_history;
    static std::mutex s_mutex;
    static const size_t kMaxHistory = 1000;
};

} // namespace engine::core