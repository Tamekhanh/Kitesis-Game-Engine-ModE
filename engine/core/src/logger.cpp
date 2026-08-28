#include "engine/core/logger.h"
#include <iostream>

namespace engine::core
{

    void Logger::Info(const std::string &msg) { std::cout << "[INFO] " << msg << "\n"; }
    void Logger::Error(const std::string &msg) { std::cerr << "[ERROR] " << msg << "\n"; }

} // namespace engine::core