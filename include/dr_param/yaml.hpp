#pragma once
#include <dr_error/error_or.hpp>

#include <yaml-cpp/yaml.h>
#include <estd/convert/convert.hpp>
#include <estd/convert/traits.hpp>

#include <algorithm>
#include <locale>
#include <optional>
#include <stdexcept>
#include <string>

namespace dr {

/// Description of a YAML node in a node tree.
struct YamlNodeDescription {
	std::string name;
	std::string user_type;
	YAML::NodeType::value node_type;
};

/// An error that occured during the conversion of a node tree to an object.
struct YamlError {
	/// A human readable description of the error.
	std::string message;

	/// A trace through the node tree to the root node.
	std::vector<YamlNodeDescription> trace;

	/// Create a new YAML error.
	explicit YamlError(std::string message, std::vector<YamlNodeDescription> node_path = {});

	/// Append a node description to the trace.
	YamlError  & appendTrace(YamlNodeDescription description) &;
	YamlError && appendTrace(YamlNodeDescription description) &&;

	/// Format the node trace as a string.
	std::string formatTrace() const;

	/// Format the whole error as a string.
	std::string format() const;
};

/// Result type used by YAML parsing functions.
template<typename T>
using YamlResult = dr::result<T, YamlError>;

/// Trait to check if a type can be converted from a YAML::Node.
template<typename T>
constexpr bool can_parse_yaml = estd::can_convert<YAML::Node const &, YamlResult<T>>;

/// Parse a YAML::Node into a type T.
template<typename T>
YamlResult<T> parseYaml(YAML::Node const & node) {
	static_assert(can_parse_yaml<T>, "No yaml conversion defined for T");
	return estd::parse<T, YamlError>(node);
}

std::optional<YamlError> expectMap(YAML::Node const & node);
std::optional<YamlError> expectMap(YAML::Node const & node, std::size_t size);
std::optional<YamlError> expectSequence(YAML::Node const & node);
std::optional<YamlError> expectSequence(YAML::Node const & node, std::size_t size);
std::optional<YamlError> expectScalar(YAML::Node const & node);

std::string toString(YAML::NodeType::value);

template<typename T>
void setIfExists(T & output, YAML::Node const & node, std::string const & key) {
	if (node[key]) output = node[key].as<T>();
}

ErrorOr<YAML::Node> readYamlFile(std::string const & path);

template<typename T>
ErrorOr<T> convertChild(YAML::Node const & node, std::string const & key) {
	if (!node[key]) return DetailedError{std::errc::invalid_argument, "no such key: " + key};
	try {
		return node[key].as<T>();
	} catch (std::system_error const & e) {
		return DetailedError{e.code(), std::string{"failed to convert node: "} + e.what()};
	} catch (std::exception const & e) {
		return DetailedError{std::errc::invalid_argument, std::string{"failed to convert node: "} + e.what()};
	}
}

}

namespace estd {

#define DECLARE_YAML_CONVERSION(TYPE) template<> struct conversion<YAML::Node, dr::YamlResult<TYPE>> { \
	static dr::YamlResult<TYPE> perform(YAML::Node const &) noexcept; \
}

DECLARE_YAML_CONVERSION(std::string);
DECLARE_YAML_CONVERSION(bool);
DECLARE_YAML_CONVERSION(char);
DECLARE_YAML_CONVERSION(short);
DECLARE_YAML_CONVERSION(int);
DECLARE_YAML_CONVERSION(long);
DECLARE_YAML_CONVERSION(long long);
DECLARE_YAML_CONVERSION(unsigned char);
DECLARE_YAML_CONVERSION(unsigned short);
DECLARE_YAML_CONVERSION(unsigned int);
DECLARE_YAML_CONVERSION(unsigned long);
DECLARE_YAML_CONVERSION(unsigned long long);
DECLARE_YAML_CONVERSION(float);
DECLARE_YAML_CONVERSION(double);
DECLARE_YAML_CONVERSION(long double);

#undef DECLARE_YAML_CONVERSION

// conversion for std::array
template<typename T, std::size_t N>
struct conversion<YAML::Node, dr::YamlResult<std::array<T, N>>> {
	static constexpr bool impossible = !dr::can_parse_yaml<T>;

	static dr::YamlResult<std::array<T, N>> perform(YAML::Node const & node) noexcept {
		if (auto error = dr::expectSequence(node, N)) return *error;

		std::array<T, N> result;

		int index = 0;
		for (YAML::const_iterator i = node.begin(); i != node.end(); ++i) {
			if (index >= N) return dr::YamlError{"sequence too long, expected " + std::to_string(N) + ", now at index " + std::to_string(index)};
			dr::YamlResult<T> element = dr::parseYaml<T>(*i);
			if (!element) return element.error().appendTrace({std::to_string(index), "", i->Type()});
			result[index++] = std::move(*element);
		}

		return result;
	}
};

// conversion for std::vector
template<typename T>
struct conversion<YAML::Node, dr::YamlResult<std::vector<T>>> {
	static constexpr bool impossible = !dr::can_parse_yaml<T>;

	static dr::YamlResult<std::vector<T>> perform(YAML::Node const & node) {
		if (node.IsNull()) return std::vector<T>{};
		if (auto error = dr::expectSequence(node)) return *error;

		std::vector<T> result;
		result.reserve(node.size());

		int index = 0;
		for (YAML::const_iterator i = node.begin(); i != node.end(); ++i) {
			dr::YamlResult<T> element = dr::parseYaml<T>(*i);
			if (!element) return element.error().appendTrace({std::to_string(index), "", i->Type()});
			result.push_back(std::move(*element));
			++index;
		}

		return result;
	}
};

namespace detail {
	// conversion for std::map<Key, Value>
	template<typename Key, typename Value>
	dr::YamlResult<std::map<Key, Value>> parseYamlMap(YAML::Node const & node) {
		if (auto error = dr::expectMap(node)) return *error;

		std::map<Key, Value> result;

		for (YAML::const_iterator i = node.begin(); i != node.end(); ++i) {
			std::string const & name = i->first.Scalar();

			dr::YamlResult<Key> key = dr::parseYaml<Key>(i->first);
			if (!key) return key.error().appendTrace({name, "", i->first.Type()});

			dr::YamlResult<Value> value = dr::parseYaml<Value>(i->second);
			if (!value) return value.error().appendTrace({name, "", i->second.Type()});

			result.emplace(std::move(*key), std::move(*value));
		}

		return result;
	}
}

// conversion for std::map<std::string, T>
template<typename T>
struct conversion<YAML::Node, dr::YamlResult<std::map<std::string, T>>> {
	static constexpr bool impossible = !dr::can_parse_yaml<T>;

	static dr::YamlResult<std::map<std::string, T>> perform(YAML::Node const & node) {
		return detail::parseYamlMap<std::string, T>(node);
	}
};

// conversion for std::map<std::string, T>
template<typename T>
struct conversion<YAML::Node, dr::YamlResult<std::map<int, T>>> {
	static constexpr bool impossible = !dr::can_parse_yaml<T>;

	static dr::YamlResult<std::map<int, T>> perform(YAML::Node const & node) {
		return detail::parseYamlMap<int, T>(node);
	}
};

}
