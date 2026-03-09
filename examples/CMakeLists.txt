if(COLORCPP_BUILD_EXAMPLES)
  add_executable(io_example io_example.cpp)
  target_link_libraries(io_example colorcpp::colorcpp)

  add_executable(random_example random_example.cpp)
  target_link_libraries(random_example colorcpp::colorcpp)
endif()
