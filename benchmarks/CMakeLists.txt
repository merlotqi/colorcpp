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

  add_executable(
    colorcpp_benchmark
    benchmark_conversion.cpp
    benchmark_blend.cpp
    benchmark_interpolate.cpp
    benchmark_delta_e.cpp
    benchmark_io.cpp
    benchmark_gamut_palette.cpp
  )
  target_link_libraries(colorcpp_benchmark PRIVATE benchmark::benchmark_main colorcpp::colorcpp)

  add_custom_target(run_benchmarks
    COMMAND $<TARGET_FILE:colorcpp_benchmark>
    COMMENT "Running colorcpp benchmarks"
    DEPENDS colorcpp_benchmark
  )
endif()
