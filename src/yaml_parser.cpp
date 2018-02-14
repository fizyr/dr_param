#include "yaml_parser.hpp"

#include <optional>

namespace dr {

std::optional<YamlError> parseYamlStruct(YAML::Node const & node, std::vector<ElemDescription> const & children) {
	if (!node.IsMap()) return YamlError{"unexpected node type, expected map, got " + toString(node.Type())};

	std::vector<bool> parsed(children.size(), false);

	for (auto child : node) {
		std::string const & key  = child.first.Scalar();
		YAML::Node const & value = child.second;

		auto result = detail::findMaybe(children, [&] (auto const & a) { return a.key == key; });
		if (!result) return YamlError{"unknown property `" + key + "'"};

		auto & [index, description] = *result;
		if (auto error = description.parse(value)) return error->appendTrace({description.key, description.type_name, value.Type()});
		parsed[index] = true;
	}

	for (std::size_t i = 0; i < children.size(); ++i) {
		if (!parsed[i] && children[i].required) return YamlError{"missing property `" + children[i].key + "'"};
	}

	return std::nullopt;
}

}
