#pragma once
#include "yaml.hpp"

#include <dr_error/error_or.hpp>
#include <estd/convert/convert.hpp>

#include <memory>
#include <string>
#include <vector>
#include <optional>

namespace dr {

namespace detail {
	template<typename Parent, typename Child>
	struct ChildParser {
		static_assert(can_parse_yaml<Child>, "No yaml conversion defined for type Child");
		Child Parent::* member;

		std::optional<YamlError> operator() (YAML::Node const & child, Parent & target) const {
			dr::result<Child, YamlError> result = parseYaml<Child>(child);
			if (!result) return result.error_unchecked();
			target.*member = std::move(*result);
			return std::nullopt;
		}
	};
}

template<typename T>
class ChildDescription {
public:
	using Parser = std::function<std::optional<YamlError>(YAML::Node const & child, T & target)>;

	std::string key;
	std::string type_name;
	bool required;
	Parser parse;
};

template<typename Parent, typename Child>
ChildDescription<Parent> childDescription(std::string key, std::string type_name, Child Parent::* member, bool required = true) {
	return {
		std::move(key),
		std::move(type_name),
		required,
		detail::ChildParser<Parent, Child>{member}
	};
}

namespace detail {
	template<typename T>
	std::optional<std::tuple<std::size_t, ChildDescription<T> const &>>
	parseYamlStructFindDescription(std::string const & key, std::vector<ChildDescription<T>> const & children) {
		std::size_t index = 0;
		for (auto const & child : children) {
			if (child.key == key) return std::make_tuple(index, std::ref(child));
			++index;
		}
		return std::nullopt;
	}
}

template<typename T>
result<T, YamlError> parseYamlStruct(YAML::Node const & node, std::vector<ChildDescription<T>> const & children) {
	if (!node.IsMap()) return YamlError{"unexpected node type, expected map, got " + toString(node.Type())};

	T output;
	std::vector<bool> parsed(children.size(), false);

	for (auto child : node) {
		std::string const & key = child.first.Scalar();
		auto result = detail::parseYamlStructFindDescription(key, children);
		if (!result) return YamlError{"unknown property `" + key + "'"};

		auto & [index, description] = *result;
		if (auto error = description.parse(child, output)) return error->appendTrace({description.key, description.type_name, child.Type()});
		parsed[index] = true;
	}

	for (std::size_t i = 0; i < children.size(); ++i) {
		if (!parsed[i]) return YamlError{"missing property `" + children[i].key + "'"};
	}

	return {in_place_valid, std::move(output)};
}

template<typename T>
result<std::unique_ptr<T>, YamlError> parseYamlStructPtr(YAML::Node const & node, std::vector<ChildDescription<T>> const & children) {
	if (!node.IsMap()) return YamlError{"unexpected node type, expected map, got " + toString(node.Type())};

	auto output = std::make_unique<T>();
	std::vector<bool> parsed(children.size(), false);

	for (auto child : node) {
		std::string const & key = child.first.Scalar();
		auto result = detail::parseYamlStructFindDescription(key, children);
		if (!result) return YamlError{"unknown property `" + key + "'"};

		auto & [index, description] = *result;
		if (auto error = description.parse(child, *output)) return error->appendTrace({description.key, description.type_name, child.Type()});
		parsed[index] = true;
	}

	for (std::size_t i = 0; i < children.size(); ++i) {
		if (!parsed[i]) return YamlError{"missing property `" + children[i].key + "'"};
	}

	return {in_place_valid, std::move(output)};
}

}
