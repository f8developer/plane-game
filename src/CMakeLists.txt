file(GLOB_RECURSE GAME_SOURCES CONFIGURE_DEPENDS
        *.cpp
        *.hpp
        *.h
        scenes/*.cpp
        systems/*.cpp
        components/*.cpp
)

file(GLOB_RECURSE ALL_DIRS LIST_DIRECTORIES true *)
foreach(DIR ${ALL_DIRS})
    if(IS_DIRECTORY ${DIR})
        list(APPEND INCLUDE_DIRS ${DIR})
    endif()
endforeach()

add_library(game ${GAME_SOURCES})
target_include_directories(game PUBLIC
    ${INCLUDE_DIRS}
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