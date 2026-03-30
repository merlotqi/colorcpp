if(COLORCPP_BUILD_BENCHMARKS)
  message(STATUS "Building benchmarks")
  include(FetchContent)
  FetchContent_Declare(
    googlebenchmark
    URL https://github.com/google/benchmark/archive/refs/tags/v1.9.2.tar.gz
  )
  set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "" FORCE)
  set(BENCHMARK_ENABLE_GTEST_TESTS OFF CACHE BOOL "" FORCE)
  FetchContent_MakeAvailable(googlebenchmark)

  # List of benchmark sources
  set(BENCHMARK_SOURCES
    benchmark_conversion.cpp
    benchmark_blend.cpp
    benchmark_interpolate.cpp
    benchmark_delta_e.cpp
    benchmark_io.cpp
    benchmark_gamut_palette.cpp
    benchmark_harmony.cpp
    benchmark_accessibility.cpp
    benchmark_vision.cpp
    benchmark_palette.cpp
    benchmark_compare.cpp
  )

  # Create individual executable for each benchmark
  set(BENCHMARK_TARGETS)
  foreach(BENCHMARK_SOURCE ${BENCHMARK_SOURCES})
    get_filename_component(BENCHMARK_NAME ${BENCHMARK_SOURCE} NAME_WE)
    add_executable(${BENCHMARK_NAME} ${BENCHMARK_SOURCE})
    target_link_libraries(${BENCHMARK_NAME} PRIVATE benchmark::benchmark_main colorcpp::colorcpp)
    list(APPEND BENCHMARK_TARGETS ${BENCHMARK_NAME})
  endforeach()

  # Target to run all benchmarks
  add_custom_target(run_all_benchmarks
    COMMAND ${CMAKE_COMMAND} -E echo "Running all benchmarks..."
    COMMENT "Running all colorcpp benchmarks"
  )
  foreach(BENCHMARK_TARGET ${BENCHMARK_TARGETS})
    add_dependencies(run_all_benchmarks ${BENCHMARK_TARGET})
    add_custom_command(
      TARGET run_all_benchmarks
      POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E echo "=== ${BENCHMARK_TARGET} ==="
      COMMAND $<TARGET_FILE:${BENCHMARK_TARGET}> --benchmark_min_time=0.001s
    )
  endforeach()
endif()
