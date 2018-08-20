#pragma once
#include "yaml.hpp"
#include "decompose.hpp"

#include <dr_error/error_or.hpp>
#include <estd/convert/convert.hpp>
#include <estd/tuple/for_each.hpp>
#include <estd/tuple/tuple.hpp>
#include <estd/tuple/transform.hpp>

#include <functional>
#include <memory>
#include <string>
#include <vector>
#include <optional>

namespace dr {

/// Marker to indicate if the default conversion from YAML::Node to YamlResult<T> should work by decomposition.
/**
 * Defaults to true if T can be decomposed with dr::param::decompose<T>().
 * Otherwise, defaults to false.
 *
 * This is mainly intended to explicitly disable YAML conversion through dr::param::decompose() without
 * providing a more specialized conversion.
 */
template<typename T>
struct enable_yaml_conversion_with_decompose : std::integral_constant<bool, param::can_decompose<T>> {};

/// Convert a decomposable type to YAML::Node.
/**
 * The resulting node is a map with each member in de decomposition of T.
 */
template<typename T>
YAML::Node encodeDecomposableAsYaml(T const & object) {
	static_assert(param::can_decompose<T>, "no decomposition available for type T");
	YAML::Node result;

	// Make tuple with member information.
	auto members = param::decompose<T>();

	estd::for_each(members, [&] (auto const & member) {
		result[member.name] = estd::convert<YAML::Node>(*member.access(object));
	});

	return result;
}

/// Convert a YAML::Node to a decomposable type.
/**
 * The YAML node must be a map with each required member in the decomposition of T.
 * The YAML node may not contain any children not listed in the decomposition of T.
 */
template<typename T>
std::optional<YamlError> parseDecomposableFromYaml(YAML::Node const & node, T & object) {
	static_assert(param::can_decompose<T>, "no decomposition available for type T");
	if (auto error = expectMap(node)) return error;

	// Make tuple of the member info with flag to remember if it was parsed.
	auto members = estd::tuple_transform_decay(param::decompose<T>(), [] (auto const & member) {
		return std::make_tuple(std::move(member), false);
	});

	// Loop over all child nodes in the YAML struct.
	for (auto child : node) {
		// Extract struct key and value.
		std::string const & key  = child.first.Scalar();
		YAML::Node const & value = child.second;

		// Storage for possible errors that may occur when parsing a child node.
		std::optional<YamlError> error = std::nullopt;

		// Try each of the decomposed member description for a matching name.
		std::size_t found_at = estd::for_each(members, [&] (auto & description) mutable {
			auto const & member_info = std::get<0>(description);
			bool & parsed            = std::get<1>(description);

			// Compare the YAML key with the member name.
			// If it doesn't match, continue with the next decomposed member.
			if (key != member_info.name) return true;

			// Get a reference to actual member, and it's type.
			auto member = member_info.access(object);
			using member_type     = std::decay_t<std::remove_pointer_t<decltype(member)>>;

			// Try parsing the member from the YAML value.
			auto result = parseYaml<member_type>(value);
			if (!result) {
				error = result.error().appendTrace({member_info.name, member_info.type, value.Type()});
			} else {
				*member = std::move(*result);
				parsed = true;
			}

			// Stop looping over the decomposed members, we already got a match.
			return false;
		});

		// If we looped over all decomposed members without finding a match, the property is unknown.
		if (found_at == estd::size(members)) return YamlError{"unknown property `" + key + "'"};

		// If we did find it but parsing it failed, return that error.
		if (error) return error;
	}

	// Check if all required decomposed members were actually parsed.
	std::optional<YamlError> error;
	estd::for_each(members, [&] (auto const & description) {
		auto const & member_info = std::get<0>(description);
		bool parsed              = std::get<1>(description);
		if (!parsed && member_info.required) {
			error = YamlError{"missing property `" + member_info.name + "'"};
			return false;
		}
		return true;
	});

	return error;
}

}

// Default conversion to YAML::Node.
template<typename T>
struct estd::conversion<T, YAML::Node> {
	static constexpr bool possible = dr::enable_yaml_conversion_with_decompose<T>::value;

	static YAML::Node perform(T const & object) {
		return dr::encodeDecomposableAsYaml(object);
	}
};

// Default conversion from YAML::Node.
template<typename T>
struct estd::conversion<YAML::Node, dr::YamlResult<T>> {
	static constexpr bool possible = dr::enable_yaml_conversion_with_decompose<T>::value;

	static dr::YamlResult<T> perform(YAML::Node const & node) {
		T result;
		if (auto error = dr::parseDecomposableFromYaml(node, result)) return std::move(*error);
		return std::move(result);
	}
};
