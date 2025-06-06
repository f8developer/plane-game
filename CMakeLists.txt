# Minimum CMake version and policies
cmake_minimum_required(VERSION 3.31)
cmake_policy(VERSION 3.31)
cmake_policy(SET CMP0048 NEW)  # Project version in project() command
cmake_policy(SET CMP0077 NEW)  # Option() honors normal variables
cmake_policy(SET CMP0000 NEW)  # Use new behavior for all policies

# Project configuration
set(CMAKE_DEPRECATION_WARNING OFF CACHE BOOL "Disable CMake deprecation warnings" FORCE)
option(ENABLE_WARNINGS "Enable compiler warnings" OFF)
project(plane_game VERSION 0.0.1 LANGUAGES CXX)

# C++ standard configuration
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Compiler warnings configuration
if(ENABLE_WARNINGS)
    if(MSVC)
        add_compile_options(/W4)
    else()
        add_compile_options(-Wall -Wextra -Wpedantic)
    endif()
endif()

# Debug configuration
add_compile_definitions(
        $<$<CONFIG:Debug>:_DEBUG>
        $<$<CONFIG:Debug>:GDEBUG>
)

# Windows-specific configuration - FIXED RC HANDLING
if(WIN32)
    if(MSVC)
        set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
        enable_language(RC)
        set(CMAKE_RC_COMPILER_INIT "rc")
    else()
        # MinGW/Clang specific resource handling - IMPROVED
        enable_language(RC)
        find_program(WINDRES_EXE windres)
        if(WINDRES_EXE)
            set(CMAKE_RC_COMPILER ${WINDRES_EXE})
            message(STATUS "Found windres: ${WINDRES_EXE}")
        else()
            message(WARNING "windres not found - resource compilation may fail")
        endif()
    endif()
endif()

# Dependencies
include(FetchContent)

# Raylib dependency
set(RAYLIB_VERSION 5.5)
find_package(raylib ${RAYLIB_VERSION} QUIET)
if(NOT raylib_FOUND)
    set(FETCHCONTENT_QUIET NO)
    FetchContent_Declare(
        raylib
        URL https://github.com/raysan5/raylib/archive/refs/tags/${RAYLIB_VERSION}.tar.gz
    )
    FetchContent_MakeAvailable(raylib)
    set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(RAYLIB_VERSION_CHECK OFF CACHE BOOL "" FORCE)
endif()

# magic_enum dependency - for enum reflection
FetchContent_Declare(
        magic_enum
        GIT_REPOSITORY https://github.com/Neargye/magic_enum.git
        GIT_TAG        v0.9.7
)
FetchContent_MakeAvailable(magic_enum)

# EnTT dependency - Entity Component System
FetchContent_Declare(
    entt
    GIT_REPOSITORY https://github.com/skypjack/entt.git
    GIT_TAG v3.15.0
)
FetchContent_MakeAvailable(entt)

# Project structure
add_subdirectory(engine)
add_subdirectory(src)

# Main executable
add_executable(${PROJECT_NAME} src/main.cpp)

# FIXED: Add resource file with proper path checking
if(WIN32)
    set(RC_FILE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/buildtime-res/app.rc")
    
    # Check if RC file exists
    if(EXISTS ${RC_FILE_PATH})
        message(STATUS "Found RC file: ${RC_FILE_PATH}")
        target_sources(${PROJECT_NAME} PRIVATE ${RC_FILE_PATH})
        
        # For MinGW, ensure proper RC compilation flags
        if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
            set_source_files_properties(${RC_FILE_PATH} PROPERTIES
                COMPILE_FLAGS "--input-format=rc --output-format=coff"
            )
        endif()
    else()
        message(WARNING "RC file not found at: ${RC_FILE_PATH}")
        message(STATUS "Please ensure your app.rc file exists in the buildtime-res directory")
    endif()
endif()

# Link dependencies
target_link_libraries(${PROJECT_NAME} PRIVATE game)

# MinGW/Clang specific configuration
if(WIN32 AND CMAKE_CXX_COMPILER_ID MATCHES "(GNU|Clang)")
    target_link_options(${PROJECT_NAME} PRIVATE -static)
    target_link_options(${PROJECT_NAME} PRIVATE
        $<$<CONFIG:Release>:-mwindows>
        $<$<CONFIG:RelWithDebInfo>:-mwindows>
        $<$<CONFIG:MinSizeRel>:-mwindows>
    )
endif()

# Post-build operations
add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_LIST_DIR}/assets
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/assets
    COMMENT "Copying assets to build directory"
)

add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:engine>
        $<TARGET_FILE_DIR:${PROJECT_NAME}>
    COMMENT "Copying engine library to build directory"
)

# macOS specific frameworks
if(APPLE)
    target_link_libraries(${PROJECT_NAME} PRIVATE
        "-framework IOKit"
        "-framework Cocoa"
        "-framework OpenGL"
    )
endif()