add_library(engine_platform STATIC
    src/window.cpp
    src/window_internal.cpp
    src/input.cpp
)
add_library(Engine::Platform ALIAS engine_platform)

target_include_directories(engine_platform
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)
target_link_libraries(engine_platform
    PUBLIC  Engine::Math glad_gl_core
    PRIVATE glfw
)