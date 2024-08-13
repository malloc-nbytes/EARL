cmake_minimum_required(VERSION 3.28.5)
project(EARL VERSION 0.2.2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Define INSTALL_PREFIX and PROJECT_VERSION as cache variables
set(INSTALL_PREFIX "" CACHE STRING "The installation prefix")
set(PROJECT_VERSION ${PROJECT_VERSION} CACHE STRING "The project version")

# Set default INSTALL_PREFIX if not specified
if(NOT INSTALL_PREFIX)
    set(INSTALL_PREFIX "/usr/local")
endif()

# Set the CMAKE_INSTALL_PREFIX to the INSTALL_PREFIX variable
set(CMAKE_INSTALL_PREFIX ${INSTALL_PREFIX})

# Include directories
include_directories(${PROJECT_SOURCE_DIR}/src/include)

# Source files
file(GLOB_RECURSE SOURCES
    src/*.cpp
    src/grammar/*.cpp
    src/primitives/*.cpp
    src/member-intrinsics/*.cpp
    src/main.cpp
)

# Add executable
add_executable(earl ${SOURCES})

# Configure a header file to pass INSTALL_PREFIX and PROJECT_VERSION
configure_file(
    ${PROJECT_SOURCE_DIR}/src/include/config.h.in
    ${PROJECT_SOURCE_DIR}/src/include/config.h
    @ONLY
)

# Install targets
install(TARGETS earl DESTINATION bin)

# Install the contents of the src/std directory
install(DIRECTORY ${PROJECT_SOURCE_DIR}/src/std/
    DESTINATION include/EARL/std
    FILES_MATCHING PATTERN "*.*"
)

# uninstall target
if(NOT TARGET uninstall)
  configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)

  add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)
endif()
