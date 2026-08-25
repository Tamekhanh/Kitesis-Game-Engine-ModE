add_executable(editor main.cpp)

target_link_libraries(editor PRIVATE
    Engine::Core
    Engine::Platform
    Engine::Renderer
    imgui
)