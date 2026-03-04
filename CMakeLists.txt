cmake_minimum_required(VERSION 4.2.2)
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

message(STATUS "CMake Version: ${CMAKE_VERSION}")

if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
        set(P_NAME "linux_dare_debug")
else(${CMAKE_BUILD_TYPE} STREQUAL "Release")
        set(P_NAME "linux_dare_release")
endif()

project(${P_NAME} LANGUAGES C VERSION 1.0.0)
message(STATUS "Project Name: ${PROJECT_NAME}")
message(STATUS "Project Version: ${PROJECT_VERSION}")

file(TO_CMAKE_PATH "${PROJECT_BINARY_DIR}/CMakeLists.txt" LOC_PATH)
if(EXISTS $LOC_PATH)
        message(FATAL_ERROR "Cannot not build in source dir please make a build directory")
endif()

if(NOT EXISTS "${CMAKE_CURRENT_LIST_DIR}/Src")
        message(FATAL_ERROR "Source directory not found")
endif()
add_subdirectory(Src)

if(EXISTS "${CMAKE_CURRENT_LIST_DIR}/Deps")
        add_subdirectory(Deps)
endif()
