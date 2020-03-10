/// Catch
#include <catch2/catch.hpp>

/// Fizyr
#include "yaml.hpp"

namespace dr {

TEST_CASE("array conversions", "[array]") {

	std::array<int, 2> original{{1, 2}};
	std::string encoded = YAML::Dump(encodeYaml(original));
	YamlResult<std::array<int, 2>> decoded = parseYaml<std::array<int, 2>>(YAML::Load(encoded));
	REQUIRE(decoded );
	REQUIRE(*decoded == original);

	decoded = parseYaml<std::array<int, 2>>(YAML::Load("[1, 2]"));
	REQUIRE(decoded);
	REQUIRE(*decoded == original);

	decoded = parseYaml<std::array<int, 2>>(YAML::Load("[1, 2, 3]"));
	REQUIRE(!decoded);
}

TEST_CASE("yaml node conversions", "[yaml_node]") {
	YAML::Node original;
	int number = 1;
	std::string string = "one";
	original["number"] = number;
	original["string"] = string;
	std::string encoded = YAML::Dump(encodeYaml(original));
	auto decoded = parseYaml<YAML::Node>(YAML::Load(encoded));
	REQUIRE(decoded);
	REQUIRE(decoded->size() == original.size());
	auto decoded_number = parseYaml<int>((*decoded)["number"]);
	auto decoded_string = parseYaml<std::string>((*decoded)["string"]);
	REQUIRE(decoded_number == number);
	REQUIRE(decoded_string == string);
}

}
