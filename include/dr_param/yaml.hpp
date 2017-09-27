#pragma once
#include <dr_error/error_or.hpp>

#include <yaml-cpp/yaml.h>

#include <array>
#include <stdexcept>

namespace dr {

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
