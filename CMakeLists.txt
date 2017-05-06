cmake_minimum_required(VERSION 2.8)
project(binary-parser)

set(CMAKE_CXX_FLAGS -std=c++11)

FILE(GLOB_RECURSE HEADERS "src/*.h")
FILE(GLOB_RECURSE SRCS "src/*.cpp")

# Some information for the build
MESSAGE(STATUS "Current built type: ${CMAKE_BUILD_TYPE}")
MESSAGE(STATUS "Current build type flags: ${CMAKE_CXX_FLAGS_${CMAKE_BUILD_TYPE}}")

add_executable( ${PROJECT_NAME} ${SRCS} ${HEADERS})
target_link_libraries( ${PROJECT_NAME})
