add_library(engine_core STATIC
    src/logger.cpp
    src/game_object.cpp
    src/project.cpp
    src/component_registry.cpp
    src/collider_component.cpp
    src/box_collider_component.cpp
    src/circle_collider_component.cpp
    src/collision_math.cpp
    src/tilemap_collider_component.cpp
    src/physics_component.cpp
    src/physics_world.cpp
    src/script_component.cpp
)
add_library(Engine::Core ALIAS engine_core)

target_include_directories(engine_core
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)
target_link_libraries(engine_core
    PUBLIC Engine::Math
    PRIVATE nlohmann_json::nlohmann_json sol2_lib
)
target_compile_features(engine_core PUBLIC cxx_std_20)