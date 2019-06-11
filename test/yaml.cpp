/// Catch
#include <catch2/catch.hpp>

/// Fizyr
#include "yaml.hpp"

namespace dr {

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
