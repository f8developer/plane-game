cmake_minimum_required(VERSION 3.31)
project(engine VERSION 0.0.1 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Source files
file(GLOB_RECURSE ENGINE_SOURCES 
    "*.cpp"
    "*.h"
)

# Create static library
add_library(${PROJECT_NAME} STATIC ${ENGINE_SOURCES})

# Include directories
target_include_directories(${PROJECT_NAME}
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

# Link dependencies
target_link_libraries(${PROJECT_NAME} PRIVATE raylib magic_enum EnTT::EnTT)

# Set output name (optional for static libraries, but keeps consistency)
set_target_properties(${PROJECT_NAME} PROPERTIES
    OUTPUT_NAME "engine"
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
)