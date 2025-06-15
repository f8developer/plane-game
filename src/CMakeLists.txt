file(GLOB_RECURSE GAME_SOURCES 
    *.cpp
    scenes/*.cpp
)
add_library(game ${GAME_SOURCES})
target_include_directories(game PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/scenes
    ${CMAKE_CURRENT_SOURCE_DIR}/systems
    ${CMAKE_CURRENT_SOURCE_DIR}/components
)

# Add GDEBUG definition to the game target specifically
target_compile_definitions(game PRIVATE
        $<$<CONFIG:Debug>:GDEBUG>
)

target_link_libraries(game PUBLIC raylib engine EnTT::EnTT)