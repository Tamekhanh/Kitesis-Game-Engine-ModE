add_library(engine_core STATIC
    src/logger.cpp
    src/game_object.cpp
    src/project.cpp
    src/component_registry.cpp
    src/collider_component.cpp
)
add_library(Engine::Core ALIAS engine_core)

target_include_directories(engine_core
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)
target_link_libraries(engine_core
    PUBLIC Engine::Math
    PRIVATE nlohmann_json::nlohmann_json
)
target_compile_features(engine_core PUBLIC cxx_std_20)