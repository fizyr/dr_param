#pragma once
#include <dr_error/error_or.hpp>

#include <yaml-cpp/yaml.h>
#include <estd/convert/convert.hpp>

#include <algorithm>
#include <array>
#include <locale>
#include <stdexcept>
#include <string>

namespace dr {

DetailedError expectMap(YAML::Node const & node);
DetailedError expectMap(YAML::Node const & node, std::size_t size);
DetailedError expectSequence(YAML::Node const & node);
DetailedError expectSequence(YAML::Node const & node, std::size_t size);
DetailedError expectScalar(YAML::Node const & node);

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

namespace YAML {
	template<typename T, std::size_t N>
	struct convert<std::array<T, N>> {
		static Node encode(std::array<T, N> const & array) {
			Node result;
			for (T const & val : array) result.push_back(Node{val});
			return result;
		}
		static bool decode(Node const & node, std::array<T, N> & result) {
			if (!node.IsSequence()) return false;
			if (node.size() != N) return false;
			for (std::size_t i = 0; i < N; ++i) {
				if (!convert<T>::decode(node[i], result[i])) return false;
			}
			return true;
		}
	};
}

namespace estd {

// conversions for primitives
dr::ErrorOr<std::string> convert(YAML::Node const & node, Parse<std::string, dr::DetailedError>);
dr::ErrorOr<bool>        convert(YAML::Node const & node, Parse<bool,        dr::DetailedError>);

dr::ErrorOr<short>     convert(YAML::Node const & node, Parse<short,     dr::DetailedError>);
dr::ErrorOr<int>       convert(YAML::Node const & node, Parse<int,       dr::DetailedError>);
dr::ErrorOr<long>      convert(YAML::Node const & node, Parse<long,      dr::DetailedError>);
dr::ErrorOr<long long> convert(YAML::Node const & node, Parse<long long, dr::DetailedError>);

dr::ErrorOr<unsigned short>     convert(YAML::Node const & node, Parse<unsigned short,     dr::DetailedError>);
dr::ErrorOr<unsigned int>       convert(YAML::Node const & node, Parse<unsigned int,       dr::DetailedError>);
dr::ErrorOr<unsigned long>      convert(YAML::Node const & node, Parse<unsigned long,      dr::DetailedError>);
dr::ErrorOr<unsigned long long> convert(YAML::Node const & node, Parse<unsigned long long, dr::DetailedError>);

dr::ErrorOr<float>       convert(YAML::Node const & node, Parse<float,       dr::DetailedError>);
dr::ErrorOr<double>      convert(YAML::Node const & node, Parse<double,      dr::DetailedError>);
dr::ErrorOr<long double> convert(YAML::Node const & node, Parse<long double, dr::DetailedError>);

// conversion for std::array
template<typename T, std::size_t N>
std::enable_if_t<can_parse_v<YAML::Node, T, dr::DetailedError>, dr::ErrorOr<std::array<T, N>>>
convert(YAML::Node const & node, Parse<std::vector<T>, dr::DetailedError>) {
	if (auto error = dr::expectSequence(node, N)) return error;

	std::array<T, N> result;

	int index = 0;
	for (YAML::iterator i = node.begin(); i != node.end(); ++i) {
		if (index >= N) return dr::DetailedError{std::errc::invalid_argument, "sequence too long, expected " + std::to_string(N) + ", now at index " + std::to_string(index)};
		dr::ErrorOr<T> element = parse<T, dr::DetailedError>(*i);
		if (!element) return element.error_unchecked().prefixed("at child node " + std::to_string(index) + ": ");
		result[index++] = std::move(*element);
	}

	return result;
}

// conversion for std::vector
template<typename T>
std::enable_if_t<can_parse_v<YAML::Node, T, dr::DetailedError>, dr::ErrorOr<std::vector<T>>>
convert(YAML::Node const & node, Parse<std::vector<T>, dr::DetailedError>) {
	if (auto error = dr::expectSequence(node)) return error;

	std::vector<T> result;
	result.reserve(node.size());

	int index = 0;
	for (YAML::const_iterator i = node.begin(); i != node.end(); ++i) {
		dr::ErrorOr<T> element = parse<T, dr::DetailedError>(*i);
		if (!element) return element.error_unchecked().prefixed("at child node " + std::to_string(index) + ": ");
		result.push_back(std::move(*element));
		++index;
	}

	return result;
}

// conversion for std::map<std::string, T>
template<typename T>
std::enable_if_t<can_parse_v<YAML::Node, T, dr::DetailedError>, dr::ErrorOr<std::map<std::string, T>>>
convert(YAML::Node const & node, Parse<std::map<std::string, T>, dr::ErrorOr<std::map<std::string, T>>>) {
	if (auto error = dr::expectMap(node)) return error;

	std::map<std::string, T> result;

	for (YAML::const_iterator i = node.begin(); i != node.end(); ++i) {
		std::string const & name = i->first.Scalar();
		dr::ErrorOr<T> element = parse<T, dr::DetailedError>(i->second);
		if (!element) return element.error_unchecked().prefixed("at child node '" + name + "': ");
		result.insert(i->first.Scalar(), std::move(*element));
	}

	return result;
}

}
