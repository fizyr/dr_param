
#include "yaml.hpp"

#include <gtest/gtest.h>

int main(int argc, char ** argv) {
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

namespace dr {

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
	ASSERT_EQ(*decoded, non_valid);
}

}
