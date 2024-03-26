/// Catch
#include <catch2/catch_test_macros.hpp>

/// Fizyr
#include "yaml.hpp"

namespace dr {

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
