cmake_minimum_required(VERSION 3.8)
project(hpp_skel VERSION 0.1.0 LANGUAGES CXX)

# If this project is included via add_subdirectory(), do not build tests/bench or download deps by default.
set(GZIP_HPP_IS_TOP_LEVEL OFF)
if (CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
    set(GZIP_HPP_IS_TOP_LEVEL ON)
endif()

option(GZIP_HPP_BUILD_TESTS "Build gzip-hpp tests and benchmarks (requires mason packages)" ${GZIP_HPP_IS_TOP_LEVEL})

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED on)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(gzip_hpp INTERFACE)
add_library(gzip-hpp::gzip_hpp ALIAS gzip_hpp)
target_include_directories(gzip_hpp INTERFACE
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

if (GZIP_HPP_BUILD_TESTS)

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/mason.cmake)

option(WERROR "Add -Werror flag to build (turns warnings into errors)" ON)

# configure optimization
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(OPTIMIZATION_FLAGS "-O0 -DDEBUG")
    message("-- Configuring debug build")
else()
    set(OPTIMIZATION_FLAGS "-O3 -DNDEBUG")
    message("-- Configuring release build")
endif()

# Enable extra warnings to adhere to https://github.com/mapbox/cpp/issues/37
set(DESIRED_WARNINGS "-Wall -Wextra -Wconversion -Wunreachable-code -Wuninitialized -pedantic-errors -Wold-style-cast -Wno-error=unused-variable -Wshadow -Wfloat-equal")

# Note: -D_GLIBCXX_USE_CXX11_ABI=0 is needed to support mason packages that are precompiled libs
# Currently we only depend on a header only library, but this will help avoid issues when more libs are added via mason
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OPTIMIZATION_FLAGS} -D_GLIBCXX_USE_CXX11_ABI=0 ${DESIRED_WARNINGS}")

if (WERROR)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")
endif()

# mason_use is a mason function within the mason.cmake file and provides ready-to-go vars, like "STATIC_LIBS" and "INCLUDE_DIRS"
mason_use(catch VERSION 1.9.6 HEADER_ONLY)
include_directories(SYSTEM ${MASON_PACKAGE_catch_INCLUDE_DIRS})

mason_use(benchmark VERSION 1.3.0)
include_directories(SYSTEM ${MASON_PACKAGE_benchmark_INCLUDE_DIRS})

mason_use(zlib VERSION 1.2.8)
include_directories(SYSTEM ${MASON_PACKAGE_zlib_INCLUDE_DIRS})

file(GLOB TEST_SOURCES test/*.cpp)
add_executable(unit-tests ${TEST_SOURCES})

# libbenchmark.a supports threads and therefore needs pthread support
find_package(Threads REQUIRED)
file(GLOB BENCH_SOURCES bench/*.cpp)
add_executable(bench-tests ${BENCH_SOURCES})

# link zlib static library to the unit-tests binary so the tests know where to find the zlib impl code
target_link_libraries(unit-tests ${MASON_PACKAGE_zlib_STATIC_LIBS})
target_link_libraries(bench-tests ${MASON_PACKAGE_benchmark_STATIC_LIBS} ${CMAKE_THREAD_LIBS_INIT} ${MASON_PACKAGE_zlib_STATIC_LIBS})

endif()