cmake_minimum_required(VERSION 3.12)
project(prng30 VERSION 1.0.0 LANGUAGES C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "" FORCE)
endif()

option(BUILD_SHARED_LIBS  "Build shared library instead of static" OFF)
option(BUILD_EXAMPLES     "Build example program"                  ON)
option(BUILD_VISUALIZER   "Build animated terminal visualizer"     ON)
option(ENABLE_SANITIZERS  "Enable ASan + UBSan"                    OFF)

if(MSVC)
    set(WARN_FLAGS /W4 /WX)
else()
    set(WARN_FLAGS
        -Wall -Wextra -Wpedantic
        -Wstrict-prototypes -Wmissing-prototypes
        -Wshadow -Wconversion -Wdouble-promotion
        -fno-strict-aliasing
    )
endif()

if(ENABLE_SANITIZERS AND NOT MSVC)
    set(SAN_FLAGS -fsanitize=address,undefined -fno-omit-frame-pointer)
endif()

# Library
add_library(prng30 src/prng.c)
set_target_properties(prng30 PROPERTIES
    VERSION ${PROJECT_VERSION} SOVERSION 1
    PUBLIC_HEADER include/prng30.h
)
target_include_directories(prng30 PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
target_compile_options(prng30 PRIVATE ${WARN_FLAGS} ${SAN_FLAGS})
target_link_options(prng30 INTERFACE ${SAN_FLAGS})

# Targets
if(BUILD_EXAMPLES)
    add_executable(example examples/example.c)
    target_link_libraries(example PRIVATE prng30)
    target_compile_options(example PRIVATE ${WARN_FLAGS})
endif()

if(BUILD_VISUALIZER)
    add_executable(prng30_visualizer visualizer/visualizer.c)
    target_link_libraries(prng30_visualizer PRIVATE prng30)
    target_compile_options(prng30_visualizer PRIVATE ${WARN_FLAGS})
endif()

add_executable(tests
    tests/main.c
    tests/test_core.c
    tests/test_statistical.c
    tests/test_double.c
)
target_link_libraries(tests PRIVATE prng30 m)
target_compile_options(tests PRIVATE ${WARN_FLAGS})

enable_testing()
add_test(NAME prng30_tests COMMAND tests)

# Benchmarks
option(BUILD_BENCH "Build benchmark/dump programs" ON)

if(BUILD_BENCH)
    add_executable(practrand_dump bench/practrand_dump.c)
    target_link_libraries(practrand_dump PRIVATE prng30)
    target_compile_options(practrand_dump PRIVATE ${WARN_FLAGS})

    add_executable(nist_dump bench/nist_dump.c)
    target_link_libraries(nist_dump PRIVATE prng30)
    target_compile_options(nist_dump PRIVATE ${WARN_FLAGS})
endif()

# Install
install(TARGETS prng30
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    PUBLIC_HEADER DESTINATION include
)