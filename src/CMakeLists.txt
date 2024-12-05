# src/CMakeLists.txt

set(SOURCES
	DAQFile.cpp
)

set(HEADERS_PUBLIC
	"${CMAKE_SOURCE_DIR}/include/DAQFile.h"
)

add_library(${PROJECT_NAME} ${SOURCES} ${HEADERS_PUBLIC})

target_include_directories(${PROJECT_NAME}
	PUBLIC "${CMAKE_SOURCE_DIR}/include"
	PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}"
)

source_group(
	TREE "${CMAKE_SOURCE_DIR}/include"
	PREFIX "Public Headers"
	FILES ${HEADERS_PUBLIC}
)
source_group(
	TREE "${CMAKE_SOURCE_DIR}/src"
	PREFIX "Sources"
	FILES ${SOURCES}
)