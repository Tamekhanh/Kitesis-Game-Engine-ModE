cmake_minimum_required(VERSION 3.21)
project(GameEngine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)   # để clangd/IDE đọc được
list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)
include(cmake/Dependencies.cmake)

add_subdirectory(engine/math)
add_subdirectory(engine/core)
add_subdirectory(engine/platform)
add_subdirectory(engine/renderer)
if(ENGINE_BUILD_EDITOR)
    add_subdirectory(editor)
endif()
# add_subdirectory(engine/physics2d)
# add_subdirectory(engine/resource)

add_subdirectory(sandbox)