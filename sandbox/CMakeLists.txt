add_executable(sandbox main.cpp)

target_link_libraries(sandbox PRIVATE
    Engine::Core
    Engine::Platform
    Engine::Renderer
)
