add_executable(editor
    main.cpp
    panels/hierarchy_panel.cpp
    panels/inspector_panel.cpp
    panels/asset_browser_panel.cpp
    panels/add_component_popup.cpp
    panels/sprite_editor_popup.cpp
    panels/animation_panel.cpp
    panels/tile_palette_panel.cpp
    panels/console_panel.cpp
)

target_link_libraries(editor PRIVATE
    Engine::Core
    Engine::Platform
    Engine::Renderer
    imgui
)

target_include_directories(editor PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

if(MSVC)
    target_link_options(editor PRIVATE
        /WHOLEARCHIVE:engine_core
        /WHOLEARCHIVE:engine_renderer
    )
endif()