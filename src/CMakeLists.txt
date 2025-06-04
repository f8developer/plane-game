file(GLOB_RECURSE GAME_SOURCES 
    *.cpp
    scenes/*.cpp
)
add_library(game ${GAME_SOURCES})
target_include_directories(game PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/scenes
)

target_link_libraries(game PUBLIC raylib engine EnTT::EnTT)