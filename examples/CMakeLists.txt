if(COLORCPP_BUILD_EXAMPLES)
  add_executable(io_example io_example.cpp)
  target_link_libraries(io_example colorcpp::colorcpp)

  add_executable(random_example random_example.cpp)
  target_link_libraries(random_example colorcpp::colorcpp)

  add_executable(cast_example cast_example.cpp)
  target_link_libraries(cast_example colorcpp::colorcpp)

  add_executable(interpolate_example interpolate_example.cpp)
  target_link_libraries(interpolate_example colorcpp::colorcpp)

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
endif()
