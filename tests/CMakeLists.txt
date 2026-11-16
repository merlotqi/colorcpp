if(COLORCPP_BUILD_TESTS)
    message(STATUS "Building unit tests")
    if(CMAKE_VERSION VERSION_LESS 3.14)
        message(FATAL_ERROR "COLORCPP_BUILD_TESTS requires CMake >= 3.14 (FetchContent_MakeAvailable)")
    endif()

    option(COLORCPP_ENABLE_COVERAGE "Instrument unit tests for coverage (GCC/Clang)" OFF)

    include(FetchContent)
    if(NOT TARGET GTest::gtest_main)
        FetchContent_Declare(
            googletest
            URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.tar.gz
        )
        set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
        set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)
        set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
        FetchContent_MakeAvailable(googletest)
    endif()

    include(GoogleTest)

    # PRE_TEST defers discovery to ctest time (serially), so the build never
    # executes test binaries. POST_BUILD is unsafe under parallel builds with
    # CMake 4.4.0: it omits TEST_TARGET, so every target in this directory
    # writes the same cmake_test_discovery_<hash>.json and MSBuild -j runs
    # race on it (empty/truncated JSON -> "string sub-command JSON failed").
    set(COLORCPP_GTEST_DISCOVERY_ARGS)
    if(NOT CMAKE_VERSION VERSION_LESS 3.18)
        list(APPEND COLORCPP_GTEST_DISCOVERY_ARGS DISCOVERY_MODE PRE_TEST)
    endif()

    file(GLOB_RECURSE TEST_MODULES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
    # compile_fail/ TUs are intentionally ill-formed; they are driven by the
    # negative-compilation harness below, never compiled into gtest binaries.
    list(FILTER TEST_MODULES EXCLUDE REGEX "/compile_fail/")

    foreach(TEST_SOURCE ${TEST_MODULES})
        get_filename_component(TEST_NAME ${TEST_SOURCE} NAME_WE)
        add_executable(${TEST_NAME} ${TEST_SOURCE})
        target_link_libraries(${TEST_NAME} PRIVATE GTest::gtest_main colorcpp colorcpp_warnings)
        set_target_properties(${TEST_NAME} PROPERTIES FOLDER "tests")

        gtest_discover_tests(${TEST_NAME}
            TEST_PREFIX "${TEST_NAME}."
            DISCOVERY_TIMEOUT 60
            ${COLORCPP_GTEST_DISCOVERY_ARGS}
        )

        if(COLORCPP_ENABLE_COVERAGE AND CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
            target_compile_options(${TEST_NAME} PRIVATE --coverage)
            target_link_libraries(${TEST_NAME} PRIVATE --coverage)
        endif()
    endforeach()

    # Negative-compilation tests: each compile_fail/fail_*.cpp must FAIL to
    # compile. The harness configures successfully iff try_compile fails, so
    # these tests flip to red the moment an ill-formed literal becomes valid.
    # Forward the toolchain so the nested configure uses the same compiler and
    # generator as this build (e.g. Ninja/clang-cl on Windows, AppleClang).
    set(COMPILE_FAIL_GENERATOR_ARGS -G "${CMAKE_GENERATOR}")
    # Each case reuses its binary dir across runs; a cache from an earlier
    # configure with a different generator/platform/toolset makes the nested
    # configure abort ("Does not match the toolset used previously").
    if(NOT CMAKE_VERSION VERSION_LESS 3.24)
        list(APPEND COMPILE_FAIL_GENERATOR_ARGS --fresh)
    endif()
    if(CMAKE_GENERATOR_PLATFORM)
        list(APPEND COMPILE_FAIL_GENERATOR_ARGS -A "${CMAKE_GENERATOR_PLATFORM}")
    endif()
    if(CMAKE_GENERATOR_TOOLSET)
        list(APPEND COMPILE_FAIL_GENERATOR_ARGS -T "${CMAKE_GENERATOR_TOOLSET}")
    endif()
    # IDE generators pick the compiler from -A/-T and ignore (with a warning)
    # an explicit CMAKE_CXX_COMPILER.
    if(NOT CMAKE_GENERATOR MATCHES "Visual Studio|Xcode")
        list(APPEND COMPILE_FAIL_GENERATOR_ARGS "-DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}")
        if(CMAKE_MAKE_PROGRAM)
            list(APPEND COMPILE_FAIL_GENERATOR_ARGS "-DCMAKE_MAKE_PROGRAM=${CMAKE_MAKE_PROGRAM}")
        endif()
    endif()
    if(CMAKE_TOOLCHAIN_FILE)
        list(APPEND COMPILE_FAIL_GENERATOR_ARGS "-DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}")
    endif()
    if(CMAKE_OSX_SYSROOT)
        list(APPEND COMPILE_FAIL_GENERATOR_ARGS "-DCMAKE_OSX_SYSROOT=${CMAKE_OSX_SYSROOT}")
    endif()
    file(GLOB COMPILE_FAIL_CASES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/compile_fail/fail_*.cpp)
    foreach(CASE_SRC ${COMPILE_FAIL_CASES})
        get_filename_component(CASE_NAME ${CASE_SRC} NAME_WE)
        add_test(NAME CompileFail.${CASE_NAME}
            COMMAND ${CMAKE_COMMAND}
                ${COMPILE_FAIL_GENERATOR_ARGS}
                -S ${CMAKE_CURRENT_SOURCE_DIR}/compile_fail
                -B ${CMAKE_CURRENT_BINARY_DIR}/compile_fail/${CASE_NAME}
                -DSOURCE=${CASE_SRC}
                -DINC=${PROJECT_SOURCE_DIR}/include
        )
    endforeach()

    # Reference-vector tests parse JSON fixtures (nlohmann/json).
    if(NOT TARGET nlohmann_json::nlohmann_json)
        FetchContent_Declare(
            nlohmann_json
            URL https://github.com/nlohmann/json/archive/refs/tags/v3.11.3.tar.gz
        )
        FetchContent_MakeAvailable(nlohmann_json)
    endif()
    target_link_libraries(test_reference_vectors PRIVATE nlohmann_json::nlohmann_json)
    target_compile_definitions(test_reference_vectors PRIVATE
        COLORCPP_REFERENCE_DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}/reference/data")

    # Multi-config generators (Visual Studio, Xcode, Ninja Multi-Config) need
    # -C <config>; omit it when single-config builds have no CMAKE_BUILD_TYPE.
    add_custom_target(run_all_tests
        COMMAND ${CMAKE_CTEST_COMMAND} "$<$<BOOL:$<CONFIG>>:-C;$<CONFIG>>" --output-on-failure
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMENT "Running all colorcpp test modules"
        VERBATIM
        COMMAND_EXPAND_LISTS
    )
endif()
