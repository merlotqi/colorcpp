/**
 * @file test_serialization.cpp
 * @brief Unit tests for serialization traits and adapter helpers.
 */

#include <gtest/gtest.h>

#include <colorcpp/core/oklab.hpp>
#include <colorcpp/core/rgb.hpp>
#include <colorcpp/io/serialization/details.hpp>
#include <colorcpp/io/serialization/json_adapter.hpp>
#include <colorcpp/io/serialization/msgpack_adapter.hpp>
#include <colorcpp/io/serialization/traits.hpp>
#include <limits>
#include <map>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

using namespace colorcpp;
using namespace colorcpp::io::serialization;

struct fake_json {
  enum class kind {
    null_value,
    number,
    array,
    object
  };

  kind type = kind::null_value;
  double number_value = 0.0;
  std::vector<fake_json> array_values;
  std::map<std::string, fake_json> object_values;
};

struct fake_msgpack_packer {
  enum class mode {
    none,
    array,
    map
  };

  mode packed_mode = mode::none;
  std::vector<double> numbers;
  std::vector<std::pair<std::string, double>> key_values;
};

struct fake_msgpack_array_view {
  std::vector<double> values;
};

struct fake_msgpack_map_view {
  std::map<std::string, double> values;
};

template <>
struct colorcpp::io::serialization::json_adapter<fake_json> {
  using json_type = fake_json;

  static fake_json make_array() {
    fake_json json;
    json.type = fake_json::kind::array;
    return json;
  }

  static fake_json make_object() {
    fake_json json;
    json.type = fake_json::kind::object;
    return json;
  }

  static void push_back(fake_json& arr, double v) {
    fake_json value;
    value.type = fake_json::kind::number;
    value.number_value = v;
    arr.array_values.push_back(value);
  }

  static void set(fake_json& obj, std::string_view key, const fake_json& val) {
    obj.object_values[std::string(key)] = val;
  }

  static void set(fake_json& obj, std::string_view key, double v) {
    fake_json value;
    value.type = fake_json::kind::number;
    value.number_value = v;
    obj.object_values[std::string(key)] = value;
  }

  static void set(fake_json& obj, std::string_view key, std::string_view /*v*/) {
    fake_json value;
    value.type = fake_json::kind::null_value;
    obj.object_values[std::string(key)] = value;
  }

  static bool is_array(const fake_json& j) { return j.type == fake_json::kind::array; }

  static bool is_object(const fake_json& j) { return j.type == fake_json::kind::object; }

  static std::size_t array_size(const fake_json& j) { return j.array_values.size(); }

  static double get_double(const fake_json& j, std::size_t index) { return j.array_values.at(index).number_value; }

  static double get_double(const fake_json& j, std::string_view key) {
    return j.object_values.at(std::string(key)).number_value;
  }

  static bool has_key(const fake_json& j, std::string_view key) {
    return j.object_values.find(std::string(key)) != j.object_values.end();
  }
};

template <>
struct colorcpp::io::serialization::msgpack_packer<fake_msgpack_packer> {
  using packer_type = fake_msgpack_packer;

  static void pack_array(fake_msgpack_packer& p, std::size_t /*n*/) {
    p.packed_mode = fake_msgpack_packer::mode::array;
    p.numbers.clear();
    p.key_values.clear();
  }

  static void pack_map(fake_msgpack_packer& p, std::size_t /*n*/) {
    p.packed_mode = fake_msgpack_packer::mode::map;
    p.numbers.clear();
    p.key_values.clear();
  }

  static void pack_double(fake_msgpack_packer& p, double v) {
    if (p.packed_mode == fake_msgpack_packer::mode::array) {
      p.numbers.push_back(v);
    } else {
      p.key_values.back().second = v;
    }
  }

  static void pack_string(fake_msgpack_packer& p, std::string_view s) {
    p.key_values.emplace_back(std::string(s), 0.0);
  }
};

template <>
struct colorcpp::io::serialization::msgpack_unpacker<fake_msgpack_array_view> {
  using unpacker_type = fake_msgpack_array_view;

  static bool is_array(const fake_msgpack_array_view&) { return true; }
  static bool is_map(const fake_msgpack_array_view&) { return false; }
  static std::size_t array_size(const fake_msgpack_array_view& u) { return u.values.size(); }
  static double get_double(const fake_msgpack_array_view& u, std::size_t index) { return u.values.at(index); }
  static double get_double(const fake_msgpack_array_view&, std::string_view) { return 0.0; }
  static bool has_key(const fake_msgpack_array_view&, std::string_view) { return false; }
};

template <>
struct colorcpp::io::serialization::msgpack_unpacker<fake_msgpack_map_view> {
  using unpacker_type = fake_msgpack_map_view;

  static bool is_array(const fake_msgpack_map_view&) { return false; }
  static bool is_map(const fake_msgpack_map_view&) { return true; }
  static std::size_t array_size(const fake_msgpack_map_view&) { return 0; }
  static double get_double(const fake_msgpack_map_view&, std::size_t) { return 0.0; }
  static double get_double(const fake_msgpack_map_view& u, std::string_view key) {
    return u.values.at(std::string(key));
  }
  static bool has_key(const fake_msgpack_map_view& u, std::string_view key) {
    return u.values.find(std::string(key)) != u.values.end();
  }
};

// ===== Traits Tests =====

TEST(SerializationTraits, FormatEnum) {
  EXPECT_EQ(static_cast<int>(serialization_format::compact), 0);
  EXPECT_EQ(static_cast<int>(serialization_format::named), 1);
}

TEST(SerializationTraits, ColorSpaceNames) {
  EXPECT_EQ(color_space_name(color_space_id::rgb8), "rgb8");
  EXPECT_EQ(color_space_name(color_space_id::rgba8), "rgba8");
  EXPECT_EQ(color_space_name(color_space_id::rgbf), "rgbf");
  EXPECT_EQ(color_space_name(color_space_id::oklab), "oklab");
  EXPECT_EQ(color_space_name(color_space_id::oklch), "oklch");
  EXPECT_EQ(color_space_name(color_space_id::cielab), "cielab");
  EXPECT_EQ(color_space_name(color_space_id::display_p3), "display_p3");
  EXPECT_EQ(color_space_name(color_space_id::xyz), "xyz");
}

TEST(SerializationTraits, DefaultOptions) {
  serialization_options opts;
  EXPECT_EQ(opts.format, serialization_format::compact);
}

// ===== Details Tests =====

TEST(SerializationDetails, ToDoubleUint8) {
  using namespace colorcpp::io::serialization::details;
  EXPECT_DOUBLE_EQ(to_double(static_cast<uint8_t>(0)), 0.0);
  EXPECT_DOUBLE_EQ(to_double(static_cast<uint8_t>(255)), 1.0);
  EXPECT_DOUBLE_EQ(to_double(static_cast<uint8_t>(128)), 128.0 / 255.0);
}

TEST(SerializationDetails, FromDoubleUint8) {
  using namespace colorcpp::io::serialization::details;
  EXPECT_EQ(from_double<uint8_t>(0.0), 0);
  EXPECT_EQ(from_double<uint8_t>(1.0), 255);
  EXPECT_EQ(from_double<uint8_t>(0.5), 128);
}

TEST(SerializationDetails, FromDoubleFloat) {
  using namespace colorcpp::io::serialization::details;
  EXPECT_FLOAT_EQ(from_double<float>(0.5f), 0.5f);
  EXPECT_FLOAT_EQ(from_double<float>(1.0f), 1.0f);
}

TEST(SerializationDetails, FromDoubleNaNThrows) {
  using namespace colorcpp::io::serialization::details;
  EXPECT_THROW(from_double<uint8_t>(std::numeric_limits<double>::quiet_NaN()), std::invalid_argument);
  EXPECT_THROW(from_double<uint8_t>(-std::numeric_limits<double>::quiet_NaN()), std::invalid_argument);
}

TEST(SerializationDetails, FromDoubleInfPropagatesToFloat) {
  using namespace colorcpp::io::serialization::details;
  // float channels propagate Inf (only NaN is guarded)
  EXPECT_TRUE(std::isinf(from_double<float>(std::numeric_limits<double>::infinity())));
}

TEST(SerializationDetails, DefaultChannelNames) {
  using namespace colorcpp::io::serialization::details;
  EXPECT_EQ(default_channel_name(0), "ch0");
  EXPECT_EQ(default_channel_name(1), "ch1");
  EXPECT_EQ(default_channel_name(2), "ch2");
}

// ===== JSON Adapter Tests =====

TEST(SerializationJson, CompactRoundTripRgba8) {
  const core::rgba8_t input{255, 127, 80, 64};
  const auto json = to_json<fake_json>(input);

  ASSERT_TRUE(json_adapter<fake_json>::is_array(json));
  ASSERT_EQ(json.array_values.size(), 4u);
  EXPECT_DOUBLE_EQ(json.array_values[0].number_value, 1.0);
  EXPECT_DOUBLE_EQ(json.array_values[1].number_value, 127.0 / 255.0);
  EXPECT_DOUBLE_EQ(json.array_values[2].number_value, 80.0 / 255.0);
  EXPECT_DOUBLE_EQ(json.array_values[3].number_value, 64.0 / 255.0);

  const auto recovered = from_json<fake_json, core::rgba8_t>(json);
  ASSERT_TRUE(recovered.has_value());
  EXPECT_EQ(recovered->r(), 255);
  EXPECT_EQ(recovered->g(), 127);
  EXPECT_EQ(recovered->b(), 80);
  EXPECT_EQ(recovered->a(), 64);
}

TEST(SerializationJson, NamedModeDefaultsToGenericChannelKeys) {
  serialization_options opts;
  opts.format = serialization_format::named;

  const core::rgbf_t input{0.25f, 0.5f, 0.75f};
  const auto json = to_json<fake_json>(input, opts);

  ASSERT_TRUE(json_adapter<fake_json>::is_object(json));
  EXPECT_TRUE(json_adapter<fake_json>::has_key(json, "ch0"));
  EXPECT_TRUE(json_adapter<fake_json>::has_key(json, "ch1"));
  EXPECT_TRUE(json_adapter<fake_json>::has_key(json, "ch2"));
  EXPECT_DOUBLE_EQ(json_adapter<fake_json>::get_double(json, "ch0"), 0.25);
  EXPECT_DOUBLE_EQ(json_adapter<fake_json>::get_double(json, "ch1"), 0.5);
  EXPECT_DOUBLE_EQ(json_adapter<fake_json>::get_double(json, "ch2"), 0.75);

  const auto recovered = from_json<fake_json, core::rgbf_t>(json);
  ASSERT_TRUE(recovered.has_value());
  EXPECT_FLOAT_EQ(recovered->r(), 0.25f);
  EXPECT_FLOAT_EQ(recovered->g(), 0.5f);
  EXPECT_FLOAT_EQ(recovered->b(), 0.75f);
}

TEST(SerializationJson, NamedModeSupportsCustomKeys) {
  serialization_options opts;
  opts.format = serialization_format::named;
  std::string names[] = {"red", "green", "blue", "alpha"};

  const core::rgba8_t input{12, 34, 56, 78};
  const auto json = to_json<fake_json>(input, names, opts);

  ASSERT_TRUE(json_adapter<fake_json>::is_object(json));
  EXPECT_TRUE(json_adapter<fake_json>::has_key(json, "red"));
  EXPECT_TRUE(json_adapter<fake_json>::has_key(json, "green"));
  EXPECT_TRUE(json_adapter<fake_json>::has_key(json, "blue"));
  EXPECT_TRUE(json_adapter<fake_json>::has_key(json, "alpha"));

  const auto recovered = from_json<fake_json, core::rgba8_t>(json, names);
  ASSERT_TRUE(recovered.has_value());
  EXPECT_EQ(recovered->r(), 12);
  EXPECT_EQ(recovered->g(), 34);
  EXPECT_EQ(recovered->b(), 56);
  EXPECT_EQ(recovered->a(), 78);
}

TEST(SerializationJson, FromJsonCompactWrongArraySizeReturnsNullopt) {
  // rgba8_t has 4 channels — deserializing an array with 3 elements should fail
  fake_json arr = json_adapter<fake_json>::make_array();
  json_adapter<fake_json>::push_back(arr, 0.0);
  json_adapter<fake_json>::push_back(arr, 0.5);
  json_adapter<fake_json>::push_back(arr, 1.0);  // only 3 — missing alpha

  EXPECT_FALSE((from_json<fake_json, core::rgba8_t>(arr).has_value()));
  EXPECT_FALSE((from_json_compact<fake_json, core::rgba8_t>(arr).has_value()));

  // 5 elements for a 4-channel color should also fail
  json_adapter<fake_json>::push_back(arr, 0.0);
  json_adapter<fake_json>::push_back(arr, 0.0);
  EXPECT_FALSE((from_json<fake_json, core::rgba8_t>(arr).has_value()));
}

TEST(SerializationJson, ToJsonNamedNullNamesThrows) {
  const core::rgba8_t color{255, 0, 0, 255};
  EXPECT_THROW(to_json_named<fake_json>(color, nullptr), std::invalid_argument);
}

TEST(SerializationJson, FromJsonNamedNullNamesReturnsNullopt) {
  fake_json obj = json_adapter<fake_json>::make_object();
  EXPECT_FALSE((from_json_named<fake_json, core::rgba8_t>(obj, nullptr).has_value()));
}

TEST(SerializationJson, ToJsonTwoArgNullNamesThrows) {
  const core::rgba8_t color{255, 0, 0, 255};
  serialization_options opts;
  opts.format = serialization_format::named;
  EXPECT_THROW(to_json<fake_json>(color, nullptr, opts), std::invalid_argument);
}

TEST(SerializationJson, FromJsonTwoArgNullNamesReturnsNullopt) {
  fake_json obj = json_adapter<fake_json>::make_object();
  EXPECT_FALSE((from_json<fake_json, core::rgba8_t>(obj, nullptr).has_value()));
}

// ===== MessagePack Adapter Tests =====

TEST(SerializationMsgpack, PackColorUsesCompactArrayFlow) {
  fake_msgpack_packer packer;
  pack_color(packer, core::rgbf_t{0.1f, 0.2f, 0.3f});

  EXPECT_EQ(packer.packed_mode, fake_msgpack_packer::mode::array);
  ASSERT_EQ(packer.numbers.size(), 3u);
  EXPECT_NEAR(packer.numbers[0], 0.1, 1e-6);
  EXPECT_NEAR(packer.numbers[1], 0.2, 1e-6);
  EXPECT_NEAR(packer.numbers[2], 0.3, 1e-6);
}

TEST(SerializationMsgpack, PackColorNamedUsesProvidedKeys) {
  fake_msgpack_packer packer;
  std::string names[] = {"L", "a", "b"};

  pack_color_named(packer, core::oklab_t{0.5f, 0.1f, -0.2f}, names);

  EXPECT_EQ(packer.packed_mode, fake_msgpack_packer::mode::map);
  ASSERT_EQ(packer.key_values.size(), 3u);
  EXPECT_EQ(packer.key_values[0].first, "L");
  EXPECT_EQ(packer.key_values[1].first, "a");
  EXPECT_EQ(packer.key_values[2].first, "b");
  EXPECT_NEAR(packer.key_values[0].second, 0.5, 1e-6);
  EXPECT_NEAR(packer.key_values[1].second, 0.1, 1e-6);
  EXPECT_NEAR(packer.key_values[2].second, -0.2, 1e-6);
}

TEST(SerializationMsgpack, UnpackColorRoundTripsCompactValues) {
  fake_msgpack_array_view view{{1.0, 127.0 / 255.0, 80.0 / 255.0, 64.0 / 255.0}};
  const auto recovered = unpack_color<fake_msgpack_array_view, core::rgba8_t>(view);

  ASSERT_TRUE(recovered.has_value());
  EXPECT_EQ(recovered->r(), 255);
  EXPECT_EQ(recovered->g(), 127);
  EXPECT_EQ(recovered->b(), 80);
  EXPECT_EQ(recovered->a(), 64);
}

TEST(SerializationMsgpack, UnpackColorNamedRoundTripsProvidedKeys) {
  fake_msgpack_map_view view{{{"red", 12.0 / 255.0}, {"green", 34.0 / 255.0}, {"blue", 56.0 / 255.0}}};
  std::string names[] = {"red", "green", "blue"};

  const auto recovered = unpack_color_named<fake_msgpack_map_view, core::rgb8_t>(view, names);

  ASSERT_TRUE(recovered.has_value());
  EXPECT_EQ(recovered->r(), 12);
  EXPECT_EQ(recovered->g(), 34);
  EXPECT_EQ(recovered->b(), 56);
}

TEST(SerializationMsgpack, UnpackColorWrongElementCountReturnsNullopt) {
  // rgba8_t has 4 channels — 3-element array should fail
  fake_msgpack_array_view short_view{{1.0, 127.0 / 255.0, 80.0 / 255.0}};
  EXPECT_FALSE((unpack_color<fake_msgpack_array_view, core::rgba8_t>(short_view).has_value()));

  // 5-element array for a 4-channel color should also fail
  fake_msgpack_array_view long_view{{0.0, 0.0, 0.0, 0.0, 0.0}};
  EXPECT_FALSE((unpack_color<fake_msgpack_array_view, core::rgba8_t>(long_view).has_value()));
}

TEST(SerializationMsgpack, PackColorNamedNullNamesThrows) {
  fake_msgpack_packer packer;
  EXPECT_THROW(pack_color_named(packer, core::rgb8_t{255, 0, 0}, nullptr), std::invalid_argument);
}

TEST(SerializationMsgpack, UnpackColorNamedNullNamesReturnsNullopt) {
  fake_msgpack_map_view view{{}};
  EXPECT_FALSE((unpack_color_named<fake_msgpack_map_view, core::rgb8_t>(view, nullptr).has_value()));
}
