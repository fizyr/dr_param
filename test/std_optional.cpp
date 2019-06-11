/// Catch
#include <catch2/catch.hpp>

/// Fizyr
#include "yaml.hpp"

// #include <gtest/gtest.h>

/*
int main(int argc, char ** argv) {
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
*/

namespace dr {

/*
TEST(YamlTest, optional) {
	std::optional<int> original{7};
	std::string encoded = YAML::Dump(encodeYaml(original));
	YamlResult<std::optional<int>> decoded = parseYaml<std::optional<int>>(YAML::Load(encoded));
	ASSERT_TRUE(decoded);
	ASSERT_EQ(*decoded, original);

	decoded = parseYaml<std::optional<int>>(YAML::Load("7"));
	ASSERT_TRUE(decoded);
	ASSERT_EQ(*decoded, original);

	decoded = parseYaml<std::optional<int>>(YAML::Load("[1, 2, 3]"));
	ASSERT_FALSE(decoded);

	std::optional<int> empty;
	encoded = YAML::Dump(encodeYaml(empty));
	decoded = parseYaml<std::optional<int>>(YAML::Load(encoded));
	ASSERT_TRUE(decoded);
	ASSERT_FALSE(decoded->has_value());
	ASSERT_EQ(*decoded, empty);
}
*/

TEST_CASE("Yaml Test", "optional") {
    
	std::optional<int> original{7};
	std::string encoded = YAML::Dump(encodeYaml(original));
	YamlResult<std::optional<int>> decoded = parseYaml<std::optional<int>>(YAML::Load(encoded));
    REQUIRE(decoded);
	REQUIRE(*decoded == original);

	decoded = parseYaml<std::optional<int>>(YAML::Load("7"));
    REQUIRE(decoded);
	REQUIRE(*decoded == original);

	decoded = parseYaml<std::optional<int>>(YAML::Load("[1, 2, 3]"));
    REQUIRE(!decoded);

	std::optional<int> empty;
	encoded = YAML::Dump(encodeYaml(empty));
	decoded = parseYaml<std::optional<int>>(YAML::Load(encoded));
    REQUIRE(decoded);
    REQUIRE(!(decoded->has_value()));
	REQUIRE(*decoded == empty);
}

}
