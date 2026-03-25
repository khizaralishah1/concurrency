cmake_minimum_required(VERSION 3.22.0)

set(CMAKE_CXX_STANDARD 20)

project(sorting VERSION 1.0.0)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release")
endif()

## ccache
find_program(CCACHE_FOUND ccache)
if(CCACHE_FOUND)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ccache)
endif()

file(GLOB_RECURSE HEADERS "include/*.h")
file(GLOB_RECURSE SOURCES "src/*.cpp")

add_library(threads_lib ${HEADERS} ${SOURCES})
target_include_directories(threads_lib PUBLIC include)

add_subdirectory(demo)