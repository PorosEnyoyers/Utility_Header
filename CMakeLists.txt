cmake_minimum_required(VERSION 4.0)

# Project name and languages
project(Utility_Header LANGUAGES CXX)

# C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Library target
add_library(Utility_Header
    src/timer.cpp
    src/file_handle.cpp
 "include/Utility_Header/Linked_List.h")

# Make headers available to library users
target_include_directories(Utility_Header
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Demo executable
add_executable(demo
    "examples/demo.cpp"
 "include/Utility_Header/Linked_List.h")

# Link library to the demo
target_link_libraries(demo PRIVATE Utility_Header)