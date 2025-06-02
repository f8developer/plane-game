file(GLOB GAME_SOURCES *.cpp)
add_library(game ${GAME_SOURCES})
target_include_directories(game PUBLIC ${CMAKE_EXPORT_SOURCE_DIR})

target_link_libraries(game raylib engine)