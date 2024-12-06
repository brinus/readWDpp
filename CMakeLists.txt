# CMakeLists.txt

cmake_minimum_required(VERSION 3.15...3.31)
project(ReadWD VERSION 1.0
			   DESCRIPTION "C++ library to read binary files from WDB and more"
			   LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_<LANG>_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(src)
add_subdirectory(example)