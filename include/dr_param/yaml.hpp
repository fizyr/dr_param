#pragma once
#include <dr_error/error_or.hpp>

#include <yaml-cpp/yaml.h>
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
