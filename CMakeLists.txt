cmake_minimum_required(VERSION 3.0)
project(ReadWD)

# Standard C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# CERN ROOT
find_package(ROOT REQUIRED)

# Main files for static library
add_library(LibReadWD STATIC
    readWD.hh
    readWD.cc
)

# Examples' main
add_executable(main0 example/main0.cc)
add_executable(main1 example/main1.cc)
add_executable(main2 example/main2.cc)
add_executable(main3 example/main3.cc)

# Collega gli eseguibili alla libreria statica e a CERN ROOT
target_link_libraries(main0 LibReadWD ${ROOT_LIBRARIES})
target_link_libraries(main1 LibReadWD ${ROOT_LIBRARIES})
target_link_libraries(main2 LibReadWD ${ROOT_LIBRARIES})
target_link_libraries(main3 LibReadWD ${ROOT_LIBRARIES})

# Aggiungi le directory di inclusione di CERN ROOT
target_include_directories(main0 PRIVATE ${ROOT_INCLUDE_DIRS})
target_include_directories(main1 PRIVATE ${ROOT_INCLUDE_DIRS})
target_include_directories(main2 PRIVATE ${ROOT_INCLUDE_DIRS})
target_include_directories(main3 PRIVATE ${ROOT_INCLUDE_DIRS})
