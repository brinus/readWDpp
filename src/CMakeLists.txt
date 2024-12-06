# src/CMakeLists.txt

set(SOURCES
	DAQFile.cpp
	DAQReader.cpp
)

set(HEADERS_PUBLIC
	"${CMAKE_SOURCE_DIR}/include/readWDpp/DAQFile.h"
)

set(HEADERS_PRIVATE
	"${CMAKE_SOURCE_DIR}/src/private/DAQReader.h"
	"${CMAKE_SOURCE_DIR}/src/private/DAQCommon.h"
)

add_library(${PROJECT_NAME} ${SOURCES} ${HEADERS_PUBLIC} ${HEADERS_PRIVATE})

target_include_directories(${PROJECT_NAME}
	PUBLIC  "${CMAKE_SOURCE_DIR}/include"
	PRIVATE "${CMAKE_SOURCE_DIR}/src/private"
)

source_group(
	TREE "${CMAKE_SOURCE_DIR}/include/readWDpp"
	PREFIX "Public Headers"
	FILES ${HEADERS_PUBLIC}
)
source_group(
	TREE "${CMAKE_SOURCE_DIR}/src/private"
	PREFIX "Private Headers"
	FILES ${HEADERS_PRIVATE}
)
source_group(
	TREE "${CMAKE_SOURCE_DIR}/src"
	PREFIX "Sources"
	FILES ${SOURCES}
)