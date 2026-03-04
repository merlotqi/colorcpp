if(COLORCPP_BUILD_EXAMPLES)
  add_executable(random_example random_example.cpp)
  target_link_libraries(random_example colorcpp::colorcpp)
endif()
