set(HEADER_LIST "${ReadWD_SOURCE_DIR}/include/DAQFile.h")

add_library(readWD DAQFile.cpp ${HEADER_LIST})

target_include_directories(readWD PUBLIC ../include)

source_group(TREE "${PROJECT_SOURCE_DIR}/include"
			 PREFIX "Header Files"
			 FILES ${HEADER_LIST})