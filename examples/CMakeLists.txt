file(GLOB EXAMPLE_SOURCES *.cpp)
set(DATA_DIR "${CMAKE_SOURCE_DIR}/data")

foreach(src ${EXAMPLE_SOURCES})
    get_filename_component(exec_name ${src} NAME_WE)
    add_executable(${exec_name} ${src})
    list(APPEND EXAMPLES ${exec_name})
endforeach()

foreach(exec ${EXAMPLES})
    target_link_libraries(${exec} PRIVATE ReadWDpp)
    target_include_directories(${exec} PRIVATE ${CMAKE_SOURCE_DIR}/include)
    target_compile_definitions(${exec} PRIVATE DATA_DIR="${DATA_DIR}")
endforeach()