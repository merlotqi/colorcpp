if(COLORCPP_BUILD_EXAMPLES)
  add_executable(io_example io_example.cpp)
  target_link_libraries(io_example colorcpp::colorcpp)

  add_executable(css_example css_example.cpp)
  target_link_libraries(css_example colorcpp::colorcpp)

  add_executable(random_example random_example.cpp)
  target_link_libraries(random_example colorcpp::colorcpp)

  add_executable(cast_example cast_example.cpp)
  target_link_libraries(cast_example colorcpp::colorcpp)

  add_executable(interpolate_example interpolate_example.cpp)
  target_link_libraries(interpolate_example colorcpp::colorcpp)

  add_executable(gradient_example gradient_example.cpp)
  target_link_libraries(gradient_example colorcpp::colorcpp)

  add_executable(palette_example palette_example.cpp)
  target_link_libraries(palette_example colorcpp::colorcpp)

  # Serialization example requires nlohmann-json
  if(NOT TARGET nlohmann_json)
    include(FetchContent)
    FetchContent_Declare(
      nlohmann_json
      URL https://github.com/nlohmann/json/archive/refs/tags/v3.11.3.tar.gz
    )
    FetchContent_MakeAvailable(nlohmann_json)
  endif()

  add_executable(serialization_example serialization_example.cpp)
  target_link_libraries(serialization_example colorcpp::colorcpp nlohmann_json::nlohmann_json)

  add_executable(msgpack_example msgpack_example.cpp)
  target_link_libraries(msgpack_example colorcpp::colorcpp)

  add_executable(ansi_example ansi_example.cpp)
  target_link_libraries(ansi_example colorcpp::colorcpp)

  add_executable(gamut_example gamut_example.cpp)
  target_link_libraries(gamut_example colorcpp::colorcpp)

  add_executable(delta_e_example delta_e_example.cpp)
  target_link_libraries(delta_e_example colorcpp::colorcpp)

  add_executable(vision_example vision_example.cpp)
  target_link_libraries(vision_example colorcpp::colorcpp)

  add_executable(accessibility_example accessibility_example.cpp)
  target_link_libraries(accessibility_example colorcpp::colorcpp)

  add_executable(color_temperature_example color_temperature_example.cpp)
  target_link_libraries(color_temperature_example colorcpp::colorcpp)

  add_executable(blend_example blend_example.cpp)
  target_link_libraries(blend_example colorcpp::colorcpp)

  add_executable(apca_example apca_example.cpp)
  target_link_libraries(apca_example colorcpp::colorcpp)

  add_executable(compare_example compare_example.cpp)
  target_link_libraries(compare_example colorcpp::colorcpp)

  add_executable(display_p3_example display_p3_example.cpp)
  target_link_libraries(display_p3_example colorcpp::colorcpp)

  add_executable(harmony_example harmony_example.cpp)
  target_link_libraries(harmony_example colorcpp::colorcpp)
  
endif()
