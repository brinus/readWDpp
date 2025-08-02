cmake_minimum_required(VERSION 3.10)
project(ReadWDpp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE SOURCES "src/*.cpp")

add_library(ReadWDpp SHARED ${SOURCES})

target_include_directories(ReadWDpp
    PUBLIC  ${CMAKE_CURRENT_SOURCE_DIR}/include/readWDpp
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include/private
)

add_subdirectory(examples)