cmake_minimum_required(VERSION 3.28.3)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
    message(FATAL_ERROR "make a different build directory in the project and use cmake.. in build directory")
endif()

project(cfs_schedular)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_compile_options(-Wall -Wextra)

file(GLOB SOURCES 
    "src/*.cpp"
    "monitoring/cpu_monitor.cpp"
    "monitoring/scheduler_monitor.cpp"
    "main.cpp"
)

add_executable(cfs_scheduler ${SOURCES})
target_include_directories(cfs_scheduler PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/external/imgui
    ${CMAKE_CURRENT_SOURCE_DIR}/external/imgui/backends
    ${CMAKE_CURRENT_SOURCE_DIR}/external/implot
)