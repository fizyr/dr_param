#pragma once
#include <estd/result.hpp>

#include <yaml-cpp/yaml.h>

#include <map>
#include <string>

namespace dr {

estd::result<YAML::Node, estd::error> preprocessYamlFile(
	std::string const & path,
	std::map<std::string, std::string> variables
);

estd::result<void, estd::error> preprocessYamlWithFilePath(YAML::Node & root,
	std::string const & file,
	std::map<std::string, std::string> variables
);

estd::result<void, estd::error> preprocessYamlWithDirectoryPath(YAML::Node & root,
	std::string const & directory,
	std::map<std::string, std::string> variables
);

}
