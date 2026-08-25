add_library(engine_math INTERFACE)
add_library(Engine::Math ALIAS engine_math)

target_include_directories(engine_math INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
target_compile_features(engine_math INTERFACE cxx_std_20)