#pragma once
#include <string>

namespace engine::core
{

    class Logger
    {
    public:
        static void Info(const std::string &msg);
        static void Error(const std::string &msg);
    };

}