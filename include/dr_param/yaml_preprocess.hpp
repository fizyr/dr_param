#pragma once
#include <dr_error/error_or.hpp>

#include <yaml-cpp/yaml.h>

#include <map>
#include <string>

namespace dr {

ErrorOr<YAML::Node> preprocessYamlFile(
	std::string const & path,
	std::map<std::string, std::string> variables
);

ErrorOr<void> preprocessYamlWithFilePath(YAML::Node & root,
	std::string const & file,
	std::map<std::string, std::string> variables
);

ErrorOr<void> preprocessYamlWithDirectoryPath(YAML::Node & root,
	std::string const & directory,
	std::map<std::string, std::string> variables
);

}
