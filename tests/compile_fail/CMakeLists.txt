# Negative-compilation harness for colorcpp.
#
# Configured by ctest (see tests/CMakeLists.txt) with:
#   -DSOURCE=<path to an intentionally ill-formed TU>
#   -DINC=<colorcpp include directory>
# Configure succeeds IFF compiling SOURCE fails — that inversion is the test.
cmake_minimum_required(VERSION 3.16)
project(colorcpp_compile_fail LANGUAGES CXX)

if(NOT DEFINED SOURCE OR NOT DEFINED INC)
    message(FATAL_ERROR "colorcpp compile-fail harness: pass -DSOURCE=<tu> -DINC=<include dir>")
endif()

# Wipe the try_compile workdir first so the result can never come from a stale cache.
file(REMOVE_RECURSE ${CMAKE_BINARY_DIR}/try_work)

try_compile(COMPILED
    ${CMAKE_BINARY_DIR}/try_work
    SOURCES ${SOURCE}
    CMAKE_FLAGS -DINCLUDE_DIRECTORIES=${INC}
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    OUTPUT_VARIABLE COMPILE_OUTPUT)

if(COMPILED)
    message(FATAL_ERROR
        "${SOURCE}\ncompiled successfully, but the TU must be ill-formed:\n${COMPILE_OUTPUT}")
endif()
message(STATUS "OK: compilation failed as expected for ${SOURCE}")
