# src/CMakeLists.txt

file(GLOB SOURCES "${CMAKE_SOURCE_DIR}/src/*.cpp")
file(GLOB HEADERS_PUBLIC "${CMAKE_SOURCE_DIR}/include/readWDpp/*.h")
file(GLOB HEADERS_PRIVATE "${CMAKE_SOURCE_DIR}/include/private/*.h")

add_library(${PROJECT_NAME} ${SOURCES} ${HEADERS_PUBLIC})

target_include_directories(${PROJECT_NAME}
	PUBLIC  "${CMAKE_SOURCE_DIR}/include/readWDpp"
	PRIVATE "${CMAKE_SOURCE_DIR}/include/private"
)

install(TARGETS ${PROJECT_NAME}
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

source_group(
	TREE "${CMAKE_SOURCE_DIR}/include/readWDpp"
	PREFIX "Public Headers"
	FILES ${HEADERS_PUBLIC}
)
source_group(
	TREE "${CMAKE_SOURCE_DIR}/include/private"
	PREFIX "Private Headers"
	FILES ${HEADERS_PRIVATE}
)
source_group(
	TREE "${CMAKE_SOURCE_DIR}/src"
	PREFIX "Sources"
	FILES ${SOURCES}
)