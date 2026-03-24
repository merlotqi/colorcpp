cmake_minimum_required(VERSION 3.10...3.24)

project(colorcpp
  VERSION 0.1.0
  DESCRIPTION "Compile-time color library for C++"
  HOMEPAGE_URL "https://github.com/merlotqi/colorcpp"
  LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(COLORCPP_BUILD_TESTS "Build tests" OFF)
option(COLORCPP_BUILD_EXAMPLES "Build examples" OFF)
option(COLORCPP_BUILD_BENCHMARKS "Build microbenchmarks (Google Benchmark via FetchContent)" OFF)
option(COLORCPP_BUILD_DOCS "Build API documentation with Doxygen" OFF)
option(COLORCPP_BUILD_SPHINX "Build Sphinx HTML from docs/ (requires sphinx-build)" OFF)
option(COLORCPP_INSTALL_HEADERS "Install header files" OFF)

file(GLOB_RECURSE COLOR_HEADERS CONFIGURE_DEPENDS
     "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp"
)
add_library(colorcpp INTERFACE)

target_include_directories(colorcpp
  INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_compile_features(colorcpp INTERFACE cxx_std_17)

if(MSVC)
  target_compile_options(colorcpp INTERFACE /W4 /WX)
else()
  target_compile_options(colorcpp INTERFACE 
    -Wall -Wextra -Wpedantic -Werror
    -Wno-unused-parameter
    -Wno-unused-variable
  )
endif()
add_library(colorcpp::colorcpp ALIAS colorcpp)

add_subdirectory(tests)
add_subdirectory(examples)
add_subdirectory(benchmarks)

if(COLORCPP_INSTALL_HEADERS)
  install(DIRECTORY include/
    DESTINATION include
    FILES_MATCHING PATTERN "*.hpp"
  )
  
  install(TARGETS colorcpp
    EXPORT colorcppTargets
    INCLUDES DESTINATION include
  )
  
  include(CMakePackageConfigHelpers)
  write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/colorcppConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
  )
  
  configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/colorcppConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/colorcppConfig.cmake"
    INSTALL_DESTINATION lib/cmake/colorcpp
  )
  
  install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/colorcppConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/colorcppConfigVersion.cmake"
    DESTINATION lib/cmake/colorcpp
  )
  
  install(EXPORT colorcppTargets
    DESTINATION lib/cmake/colorcpp
    NAMESPACE colorcpp::
  )
endif()

export(TARGETS colorcpp NAMESPACE colorcpp:: FILE "${CMAKE_CURRENT_BINARY_DIR}/colorcppTargets.cmake")

if(COLORCPP_BUILD_SPHINX)
  find_program(SPHINX_BUILD_EXECUTABLE sphinx-build)
  if(SPHINX_BUILD_EXECUTABLE)
    set(COLORCPP_SPHINX_OUT "${CMAKE_CURRENT_BINARY_DIR}/doc_sphinx")
    add_custom_target(
      sphinx
      COMMAND "${SPHINX_BUILD_EXECUTABLE}" -b html "${CMAKE_CURRENT_SOURCE_DIR}/docs" "${COLORCPP_SPHINX_OUT}"
      WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
      COMMENT "Generating colorcpp Sphinx HTML documentation in ${COLORCPP_SPHINX_OUT}"
      VERBATIM
    )
  else()
    message(WARNING "COLORCPP_BUILD_SPHINX is ON but sphinx-build was not found; sphinx target not created")
  endif()
endif()

if(COLORCPP_BUILD_DOCS)
  find_package(Doxygen)
  if(DOXYGEN_FOUND)
    configure_file(
      "${CMAKE_CURRENT_SOURCE_DIR}/doxygen/Doxygen.in"
      "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile"
      @ONLY
    )
    add_custom_target(
      doc
      COMMAND "${DOXYGEN_EXECUTABLE}" "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile"
      WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
      COMMENT "Generating colorcpp API documentation (HTML) with Doxygen"
      VERBATIM
    )
  else()
    message(WARNING "COLORCPP_BUILD_DOCS is ON but Doxygen was not found; doc target not created")
  endif()
endif()

if(COLORCPP_INSTALL_HEADERS)
  configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/colorcpp.pc.in"
    "${CMAKE_CURRENT_BINARY_DIR}/colorcpp.pc"
    @ONLY
  )
  
  install(FILES "${CMAKE_CURRENT_BINARY_DIR}/colorcpp.pc"
    DESTINATION lib/pkgconfig
  )
endif()
