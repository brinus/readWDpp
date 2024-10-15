cmake_minimum_required(VERSION 3.0)
project(ReadWD)

# Standard C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# CERN ROOT
find_package(ROOT REQUIRED)

# Include directories
include_directories(include)

# Main files for static library
file(GLOB_RECURSE SRC_FILES src/*.cc)

# Debug message to check if files are found
message(STATUS "Source files: ${SRC_FILES}")

# Output directory for the library
set(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/lib)

# Create the static library
add_library(ReadWD STATIC ${SRC_FILES})

# Link against ROOT libraries
target_link_libraries(ReadWD ${ROOT_LIBRARIES})

# Main executable
add_executable(DAQFile_test00 example/DAQFile_test00.cc)

# Link the executable with the ReadWD library and ROOT libraries
target_link_libraries(DAQFile_test00 ReadWD ${ROOT_LIBRARIES})