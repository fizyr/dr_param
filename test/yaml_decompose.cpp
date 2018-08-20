#include "yaml.hpp"
#include "yaml_decompose.hpp"
#include "decompose_macros.hpp"

#include <gtest/gtest.h>

int main(int argc, char ** argv) {
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

namespace dr {
	struct Struct {
		int a;
		bool b;
		std::string c;
	};

	class Class {
		int member_;

	public:
		int       & member()       { return member_; }
		int const & member() const { return member_; }
	};
}

DR_PARAM_DEFINE_STRUCT_DECOMPOSITION(dr::Struct,
	(a, "int", "", true)
	(b, "int", "", true)
	(c, "int", "", true)
);

DR_PARAM_DEFINE_DECOMPOSITION(dr::Class,
	("member", "int", "", true, [] (auto & v) { return &v.member();} )
);

namespace dr {

TEST(YamlParser, decompose_struct) {
	YAML::Node node = YAML::Load("{a: 7, b: true, c: \"aap noot mies\"}");
	YamlResult<Struct> foo = parseYaml<Struct>(node);
	ASSERT_TRUE(foo);
	ASSERT_EQ(foo->a, 7);
	ASSERT_EQ(foo->b, true);
	ASSERT_EQ(foo->c, "aap noot mies");
}

TEST(YamlParser, decompose_class) {
	YAML::Node node = YAML::Load("{member: 7}");
	YamlResult<Class> foo = parseYaml<Class>(node);
	ASSERT_TRUE(foo);
	ASSERT_EQ(foo->member(), 7);
}

}
