if(COLORCPP_BUILD_TESTS)
    message(STATUS "Building unit tests")
    if(NOT TARGET googletest)
        include(FetchContent)
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

    file(GLOB_RECURSE TEST_MODULES "*.cpp")
    # compile_fail/ TUs are intentionally ill-formed; they are driven by the
    # negative-compilation harness below, never compiled into gtest binaries.
    list(FILTER TEST_MODULES EXCLUDE REGEX "compile_fail")

    foreach(TEST_SOURCE ${TEST_MODULES})
        get_filename_component(TEST_NAME ${TEST_SOURCE} NAME_WE)
        add_executable(${TEST_NAME} ${TEST_SOURCE})
        target_link_libraries(${TEST_NAME} PRIVATE GTest::gtest_main colorcpp colorcpp_warnings)
        set_target_properties(${TEST_NAME} PROPERTIES FOLDER "tests")
        
        gtest_discover_tests(${TEST_NAME}
            TEST_PREFIX "${TEST_NAME}."
            DISCOVERY_TIMEOUT 60
            # Disable PRE_TEST mode on Windows to avoid MSBuild file name parsing bug with [ ] characters
            $<$<NOT:$<PLATFORM_ID:Windows>>:DISCOVERY_MODE PRE_TEST>
        )

        if(MEMPOOL_ENABLE_COVERAGE AND (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang"))
            target_compile_options(${TEST_NAME} PRIVATE --coverage -fprofile-arcs -ftest-coverage)
            target_link_options(${TEST_NAME} PRIVATE --coverage)
        endif()
    endforeach()

    # Negative-compilation tests: each compile_fail/fail_*.cpp must FAIL to
    # compile. The harness configures successfully iff try_compile fails, so
    # these tests flip to red the moment an ill-formed literal becomes valid.
    set(COMPILE_FAIL_GENERATOR_ARGS -G ${CMAKE_GENERATOR})
    if(CMAKE_GENERATOR_PLATFORM)
        list(APPEND COMPILE_FAIL_GENERATOR_ARGS -A ${CMAKE_GENERATOR_PLATFORM})
    endif()
    file(GLOB COMPILE_FAIL_CASES ${CMAKE_CURRENT_SOURCE_DIR}/compile_fail/fail_*.cpp)
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
    if(NOT TARGET nlohmann_json)
        include(FetchContent)
        FetchContent_Declare(
            nlohmann_json
            URL https://github.com/nlohmann/json/archive/refs/tags/v3.11.3.tar.gz
        )
        FetchContent_MakeAvailable(nlohmann_json)
    endif()
    target_link_libraries(test_reference_vectors PRIVATE nlohmann_json::nlohmann_json)
    target_compile_definitions(test_reference_vectors PRIVATE
        COLORCPP_REFERENCE_DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}/reference/data")

    add_custom_target(run_all_tests
        COMMAND ${CMAKE_CTEST_COMMAND} --test-dir ${CMAKE_BINARY_DIR} --output-on-failure
        COMMENT "Running all colorcpp test modules"
    )
endif()
