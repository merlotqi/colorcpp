if(COLORCPP_BUILD_EXAMPLES)
  add_executable(random_example random_example.cpp)
  target_link_libraries(random_example colorcpp::colorcpp)

  add_executable(cast_example cast_example.cpp)
  target_link_libraries(cast_example colorcpp::colorcpp)

  add_executable(interpolation_example interpolation_example.cpp)
  target_link_libraries(interpolation_example colorcpp::colorcpp)
endif()
