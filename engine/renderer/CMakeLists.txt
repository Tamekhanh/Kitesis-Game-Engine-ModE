add_library(engine_renderer STATIC
    src/shader.cpp
    src/sprite_renderer.cpp
    src/camera2d.cpp
    src/texture2d.cpp
    src/sprite_component.cpp
    src/framebuffer.cpp
    src/scene_serializer.cpp
)
add_library(Engine::Renderer ALIAS engine_renderer)

target_include_directories(engine_renderer
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)
target_link_libraries(engine_renderer
    PUBLIC  Engine::Platform Engine::Core
    PRIVATE stb_image nlohmann_json::nlohmann_json
)