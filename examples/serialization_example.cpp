/**
 * @file serialization_example.cpp
 * @brief Example: serializing colors to JSON using the serialization module.
 *
 * Demonstrates how to specialize json_adapter for nlohmann::json and use
 * to_json / from_json to serialize and deserialize color objects.
 */

#include <colorcpp/colorcpp.hpp>
#include <iostream>

// nlohmann-json: in a real project this comes from your package manager or CMake FetchContent.
// For this example we include it directly; the CMakeLists.txt fetches it automatically.
#include <nlohmann/json.hpp>

using namespace colorcpp;
using namespace colorcpp::operations::conversion;
using namespace colorcpp::io::serialization;
using namespace colorcpp::io::ansi;

// =============================================================================
// Step 1: Specialize json_adapter for nlohmann::json
// =============================================================================
// This is the ONLY integration point needed. Once defined, all to_json / from_json
// overloads work automatically for every color type.

template <>
struct colorcpp::io::serialization::json_adapter<nlohmann::json> {
  using json_type = nlohmann::json;
  static nlohmann::json make_array() { return nlohmann::json::array(); }
  static nlohmann::json make_object() { return nlohmann::json::object(); }

  static void push_back(nlohmann::json& arr, double v) { arr.push_back(v); }

  static void set(nlohmann::json& obj, std::string_view key, const nlohmann::json& val) { obj[std::string(key)] = val; }
  static void set(nlohmann::json& obj, std::string_view key, double v) { obj[std::string(key)] = v; }
  static void set(nlohmann::json& obj, std::string_view key, std::string_view v) {
    obj[std::string(key)] = std::string(v);
  }

  static bool is_array(const nlohmann::json& j) { return j.is_array(); }
  static bool is_object(const nlohmann::json& j) { return j.is_object(); }
  static std::size_t array_size(const nlohmann::json& j) { return j.size(); }

  static double get_double(const nlohmann::json& j, std::size_t index) { return j[index].get<double>(); }
  static double get_double(const nlohmann::json& j, std::string_view key) { return j[std::string(key)].get<double>(); }

  static bool has_key(const nlohmann::json& j, std::string_view key) { return j.contains(std::string(key)); }
};

// =============================================================================
// Step 2: Use to_json / from_json
// =============================================================================

static void section(const char* title) {
  std::cout << '\n' << bold() << "── " << title << " ──" << reset() << '\n';
}

int main() {
  // --- Compact format (array) ---
  section("Compact format (array)");

  rgba8_t coral = constants::coral;
  nlohmann::json j_compact = to_json<nlohmann::json>(coral);
  std::cout << "coral compact: " << j_compact.dump(2) << "\n";

  // Deserialize back
  auto recovered = from_json<nlohmann::json, rgba8_t>(j_compact);
  if (recovered) {
    std::cout << "recovered: ";
    print_color(std::cout, *recovered);
    std::cout << "  (" << std::hex << *recovered << std::dec << ")\n";
  }

  // --- Named format (object) ---
  section("Named format (object)");

  serialization_options opts;
  opts.format = serialization_format::named;
  nlohmann::json j_named = to_json<nlohmann::json>(coral, opts);
  std::cout << "coral named: " << j_named.dump(2) << "\n";

  // Deserialize with auto-detect (works for both compact and named)
  auto recovered_named = from_json<nlohmann::json, rgba8_t>(j_named);
  if (recovered_named) {
    std::cout << "recovered: ";
    print_color(std::cout, *recovered_named);
    std::cout << "  (" << std::hex << *recovered_named << std::dec << ")\n";
  }

  // --- Custom channel names ---
  section("Custom channel names");

  std::string names[] = {"red", "green", "blue", "alpha"};
  nlohmann::json j_custom = to_json<nlohmann::json>(coral, names, opts);
  std::cout << "coral custom names: " << j_custom.dump(2) << "\n";

  auto recovered_custom = from_json<nlohmann::json, rgba8_t>(j_custom, names);
  if (recovered_custom) {
    std::cout << "recovered: ";
    print_color(std::cout, *recovered_custom);
    std::cout << "  (" << std::hex << *recovered_custom << std::dec << ")\n";
  }

  // --- Different color spaces ---
  section("Different color spaces");

  oklch_t oklch = color_cast<oklch_t>(coral);
  nlohmann::json j_oklch = to_json<nlohmann::json>(oklch);
  std::cout << "oklch compact: " << j_oklch.dump(2) << "\n";

  hsl_float_t hsl = color_cast<hsl_float_t>(coral);
  nlohmann::json j_hsl = to_json<nlohmann::json>(hsl, opts);
  std::cout << "hsl named: " << j_hsl.dump(2) << "\n";

  // --- Array of colors ---
  section("Array of colors");

  nlohmann::json palette = nlohmann::json::array();
  rgba8_t colors[] = {constants::red, constants::green, constants::blue, constants::coral};
  for (const auto& c : colors) {
    palette.push_back(to_json<nlohmann::json>(c));
  }
  std::cout << "palette: " << palette.dump(2) << "\n";
  std::cout << "swatches: ";
  print_palette(std::cout, colors, sizeof colors / sizeof colors[0], 3);

  return 0;
}
