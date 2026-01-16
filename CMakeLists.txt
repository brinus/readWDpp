cmake_minimum_required(VERSION 3.19)
project(readWDpp
    VERSION 2.0.0
    LANGUAGES CXX
)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cc"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cxx"
)

add_library(readWDpp SHARED ${SOURCES})

target_include_directories(readWDpp
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

install(TARGETS readWDpp
    EXPORT readWDppTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/readWDpp/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/readWDpp
)

set(readWDpp_INSTALL_CMAKEDIR "${CMAKE_INSTALL_LIBDIR}/cmake/readWDpp")

install(EXPORT readWDppTargets
    NAMESPACE readWDpp::
    DESTINATION ${readWDpp_INSTALL_CMAKEDIR}
)

configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/readWDppConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/readWDppConfig.cmake
    INSTALL_DESTINATION ${readWDpp_INSTALL_CMAKEDIR}
)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/readWDppConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/readWDppConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/readWDppConfigVersion.cmake
    DESTINATION ${readWDpp_INSTALL_CMAKEDIR}
)
