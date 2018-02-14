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
	/// Function object that parses a YAML node and stores the result in a predetermined object.
	template<typename Target>
	struct ElemParser {
		static_assert(can_parse_yaml<Target>, "No yaml conversion defined for type Target");
		Target * target;

		std::optional<YamlError> operator() (YAML::Node const & child) const {
			dr::result<Target, YamlError> result = parseYaml<Target>(child);
			if (!result) return result.error_unchecked();
			*target = std::move(*result);
			return std::nullopt;
		}
	};

	/// Function object that parses a YAML node an object, and stores the result in a predetermined member of the object.
	template<typename Parent, typename Child>
	struct ChildParser {
		static_assert(can_parse_yaml<Child>, "No yaml conversion defined for type Child");
		Child Parent::* member;

		ElemParser<Child> operator() (Parent & parent) const {
			return ElemParser<Child>{&(parent.*member)};
		}
	};
}

struct ElemDescription {
	using Parser = std::function<std::optional<YamlError>(YAML::Node const & child)>;

	std::string key;
	std::string type_name;
	bool required;
	Parser parse;
};

template<typename T>
class ChildDescription {
public:
	using Parser     = std::function<std::optional<YamlError>(YAML::Node const & child)>;
	using MakeParser = std::function<Parser(T & output)>;

	std::string key;
	std::string type_name;
	bool required;
	MakeParser make_parser;

	/// Allow implicit conversion to ChildDescription<D> for D derived from T.
	template<typename D, typename = std::enable_if_t<std::is_base_of_v<T, D>>>
	operator ChildDescription<D>() const {
		return {key, type_name, required, make_parser};
	}

	ElemDescription intoElemDescription(T & output) const {
		return {key, type_name, required, make_parser(output)};
	}
};

template<typename Target>
ElemDescription elementDescription(std::string key, std::string type_name, Target * target, bool required = true) {
	return {std::move(key), std::move(type_name), required, detail::ElemParser<Target>{target}};
}

template<typename Parent, typename Child>
ChildDescription<Parent> childDescription(std::string key, std::string type_name, Child Parent::* member, bool required = true) {
	return {std::move(key), std::move(type_name), required, detail::ChildParser<Parent, Child>{member}};
}

namespace detail {
	template<typename T, typename Predicate>
	std::optional<std::tuple<std::size_t, T const &>>
	findMaybe(std::vector<T> const & vector, Predicate && predicate) {
		std::size_t index = 0;
		for (auto const & elem : vector) {
			if (predicate(elem)) return std::make_tuple(index, std::ref(elem));
		}
		return std::nullopt;
	}

	template<typename T>
	std::optional<std::tuple<std::size_t, ChildDescription<T> const &>>
	parseYamlStructFindDescription(std::string const & key, std::vector<ChildDescription<T>> const & children) {
		return findMaybe(children, [&] (auto const & a) { return a.key == key; });
	}
}

std::optional<YamlError> parseYamlStruct(YAML::Node const & node, std::vector<ElemDescription> const & children);

template<typename T>
result<T, YamlError> parseYamlStruct(YAML::Node const & node, std::vector<ChildDescription<T>> const & children) {
	if (!node.IsMap()) return YamlError{"unexpected node type, expected map, got " + toString(node.Type())};

	T output;
	std::vector<ElemDescription> descriptions;
	descriptions.reserve(children.size());
	for (auto const & child : children) descriptions.push_back(child.intoElemDescription(output));
	if (auto error = parseYamlStruct(node, descriptions)) return std::move(*error);
	return output;
}

template<typename T>
result<std::unique_ptr<T>, YamlError> parseYamlStructPtr(YAML::Node const & node, std::vector<ChildDescription<T>> const & children) {
	if (!node.IsMap()) return YamlError{"unexpected node type, expected map, got " + toString(node.Type())};

	auto output = std::make_unique<T>();
	std::vector<ElemDescription> descriptions;
	descriptions.reserve(children.size());
	for (auto const & child : children) descriptions.push_back(child.intoElemDescription(output));
	if (auto error = parseYamlStruct(node, descriptions)) return std::move(*error);
	return output;
}

}
