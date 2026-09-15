cmake_minimum_required(VERSION 3.20)

project(Utility_Header LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(Utility_Header
    src/timer.cpp
    src/file_handle.cpp
 "include/Utility_Header/Stack.h" "include/Queue.h" "include/Utility_Header/Prio_Queue.h" "include/Utility_Header/Binary_Tree.h" "include/Utility_Header/AVL_Tree.h" "include/Utility_Header/B_Tree.h" "include/Utility_Header/Red_Black_Tree.h" "include/Utility_Header/Red_Black_Tree_Ver2.h" "include/Utility_Header/Circular_Buffer.h")
target_include_directories(Utility_Header
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_executable(demo
    examples/demo.cpp
 "include/Utility_Header/Stack.h" "include/Queue.h" "include/Utility_Header/Prio_Queue.h" "include/Utility_Header/Binary_Tree.h" "include/Utility_Header/AVL_Tree.h" "include/Utility_Header/B_Tree.h" "include/Utility_Header/Red_Black_Tree.h" "include/Utility_Header/Red_Black_Tree_Ver2.h" "include/Utility_Header/Circular_Buffer.h")
target_link_libraries(demo PRIVATE Utility_Header)