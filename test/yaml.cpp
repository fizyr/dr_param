
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
TEST(YamlTest, array) {
	std::array<int, 2> original{{1, 2}};
	std::string encoded = YAML::Dump(encodeYaml(original));
	YamlResult<std::array<int, 2>> decoded = parseYaml<std::array<int, 2>>(YAML::Load(encoded));
	ASSERT_TRUE(decoded);
	ASSERT_EQ(*decoded, original);

	decoded = parseYaml<std::array<int, 2>>(YAML::Load("[1, 2]"));
	ASSERT_TRUE(decoded);
	ASSERT_EQ(*decoded, original);

	decoded = parseYaml<std::array<int, 2>>(YAML::Load("[1, 2, 3]"));
	ASSERT_FALSE(decoded);
}
*/

TEST_CASE( "Yaml Test", "[array]" ) {

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

}
