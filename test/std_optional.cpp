
#include "yaml.hpp"

#include <gtest/gtest.h>

int main(int argc, char ** argv) {
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

namespace dr {

TEST(YamlTest, array) {
	std::array<int, 2> original{{1, 2}};
	std::string encoded = YAML::Dump(YAML::Node{original});
	std::array<int, 2> decoded = YAML::Load(encoded).as<std::array<int, 2>>();
	ASSERT_EQ(decoded, original);

	decoded = YAML::Load("[1, 2]").as<std::array<int, 2>>();
	ASSERT_EQ(decoded, original);
}

}
