# example/CMakeLists.txt

file(GLOB EXAMPLES "*.cpp")

foreach(EXAMPLE ${EXAMPLES})
	get_filename_component(EXAMPLE_NAME ${EXAMPLE} NAME_WE)
	add_executable(${EXAMPLE_NAME} ${EXAMPLE})

	target_link_libraries(${EXAMPLE_NAME} PRIVATE ${PROJECT_NAME})
	target_include_directories(${EXAMPLE_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/include")
	target_compile_definitions(${EXAMPLE_NAME} PRIVATE DATA_DIR="${CMAKE_SOURCE_DIR}/data")

endforeach()
