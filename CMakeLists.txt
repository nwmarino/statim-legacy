cmake_minimum_required(VERSION 3.17)
project(statimc)

set(CMAKE_CXX_STANDARD 17)

file(GLOB_RECURSE SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/**.cxx)

# Testing file reference purposes
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(statimc ${SOURCE_FILES})
