cmake_minimum_required(VERSION 3.10)
project(prng30 VERSION 1.0.0 LANGUAGES C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Option to build shared or static library
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

# Create the library
add_library(prng30
    src/prng.c
    src/visualizer.c
)

# Set library properties
set_target_properties(prng30 PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION 1
    PUBLIC_HEADER include/prng30.h
)

# Include directories
target_include_directories(prng30 PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# Build examples
add_executable(example examples/example.c)
target_link_libraries(example prng30)

# Build tests
add_executable(tests tests/tests.c)
target_link_libraries(tests prng30 m)

# Install the library
install(TARGETS prng30
    EXPORT prng30Targets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    PUBLIC_HEADER DESTINATION include
)

# Install the export set
install(EXPORT prng30Targets
    FILE prng30Targets.cmake
    NAMESPACE prng30::
    DESTINATION lib/cmake/prng30
)

# Create and install package config file
include(CMakePackageConfigHelpers)

configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/prng30Config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/prng30Config.cmake
    INSTALL_DESTINATION lib/cmake/prng30
)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/prng30ConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/prng30Config.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/prng30ConfigVersion.cmake
    DESTINATION lib/cmake/prng30
)

# Optionally install examples and tests
option(INSTALL_EXAMPLES "Install example programs" ON)
if(INSTALL_EXAMPLES)
    install(TARGETS example tests DESTINATION bin)
endif()

# Print build information
message(STATUS "")
message(STATUS "PRNG30 Configuration:")
message(STATUS "  Version: ${PROJECT_VERSION}")
message(STATUS "  Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  Shared libs: ${BUILD_SHARED_LIBS}")
message(STATUS "  Install prefix: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "  Install examples: ${INSTALL_EXAMPLES}")
message(STATUS "")