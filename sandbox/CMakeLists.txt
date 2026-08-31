add_executable(sandbox main.cpp)

target_link_libraries(sandbox PRIVATE
    Engine::Core
    Engine::Platform
    Engine::Renderer
)

if(MSVC)
    target_link_options(sandbox PRIVATE
        /WHOLEARCHIVE:engine_core
        /WHOLEARCHIVE:engine_renderer
    )
endif()