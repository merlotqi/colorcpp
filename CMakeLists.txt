cmake_minimum_required(VERSION 3.10...3.24)

project(colorcpp
  VERSION 0.1.0
  DESCRIPTION "Compile-time color library for C++"
  HOMEPAGE_URL "https://github.com/merlotqi/colorcpp"
  LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(BUILD_TESTS "Build tests" OFF)
option(BUILD_EXAMPLES "Build examples" OFF)
option(INSTALL_HEADERS "Install header files" OFF)

add_library(colorcpp INTERFACE)

target_include_directories(colorcpp
  INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

file(GLOB_RECURSE COLOR_HEADERS "include/color/**/*.hpp")
target_sources(colorcpp INTERFACE ${COLOR_HEADERS})

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

if(BUILD_EXAMPLES)
  add_executable(example_color_member example_color_member.cpp)
  target_link_libraries(example_color_member PRIVATE colorcpp)
  
  set_property(TARGET example_color_member PROPERTY CXX_STANDARD 17)
  set_property(TARGET example_color_member PROPERTY CXX_STANDARD_REQUIRED ON)
endif()

if(BUILD_TESTS)
  enable_testing()
  
  add_executable(test_all_models test_all_models.cpp)
  target_link_libraries(test_all_models PRIVATE colorcpp)
  set_property(TARGET test_all_models PROPERTY CXX_STANDARD 17)
  set_property(TARGET test_all_models PROPERTY CXX_STANDARD_REQUIRED ON)
  
  add_executable(test_unified_color test_unified_color.cpp)
  target_link_libraries(test_unified_color PRIVATE colorcpp)
  set_property(TARGET test_unified_color PROPERTY CXX_STANDARD 17)
  set_property(TARGET test_unified_color PROPERTY CXX_STANDARD_REQUIRED ON)
  
  add_test(NAME test_all_models COMMAND test_all_models)
  add_test(NAME test_unified_color COMMAND test_unified_color)
endif()

if(INSTALL_HEADERS)
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

if(INSTALL_HEADERS)
  configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/colorcpp.pc.in"
    "${CMAKE_CURRENT_BINARY_DIR}/colorcpp.pc"
    @ONLY
  )
  
  install(FILES "${CMAKE_CURRENT_BINARY_DIR}/colorcpp.pc"
    DESTINATION lib/pkgconfig
  )
endif()
