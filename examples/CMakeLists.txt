# Trova tutti i file .cpp nella cartella degli esempi
file(GLOB EXAMPLE_SOURCES *.cpp)

# Definisci la directory dei dati come variabile CMake
set(DATA_DIR "${CMAKE_SOURCE_DIR}/data")

foreach(SOURCE_FILE ${EXAMPLE_SOURCES})
    get_filename_component(EXECUTABLE_NAME ${SOURCE_FILE} NAME_WE)
    add_executable(${EXECUTABLE_NAME} ${SOURCE_FILE})
    target_link_libraries(${EXECUTABLE_NAME} PRIVATE ReadWDpp)
    target_include_directories(${EXECUTABLE_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/include)
    # Definisci la macro DATA_DIR per il codice C++
    target_compile_definitions(${EXECUTABLE_NAME} PRIVATE DATA_DIR="${DATA_DIR}")
endforeach()
