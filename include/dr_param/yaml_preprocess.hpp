#pragma once
#include <estd/result.hpp>

#include <yaml-cpp/yaml.h>

#include <map>
#include <string>

/**
 * This header defines utilities to parse a YAML file with some preprocessing.
 *
 * The preprocessing can be used to include other YAML files or to perform parameter substitution.
 */

namespace dr {

/// Load a YAML file and preprocess it.
/**
 * Preprocessing supports a number of tags on YAML nodes:
 *
 *   !include "path"
 *     Preprocess and include another YAML file.
 *     If the path is relative, it is interpreted relative to the parent directory of the file being loaded.
 *     Variables in the path are expanded.
 *
 *   !expand "string with $variables in it"
 *     Expand a string with variables in it.
 *     The variables are taken from the `variables` parameter.
 *     Variables can take the form $var or ${var}.
 *
 *     The variables "$DIR" and "$FILE" are always available when processing files,
 *     and contain the parent directory and file path of the file being processed.
 */
estd::result<YAML::Node, estd::error> preprocessYamlFile(
	std::string const & path,
	std::map<std::string, std::string> variables
);

/// Preprocess a YAML node with path information.
estd::result<void, estd::error> preprocessYamlWithFilePath(YAML::Node & root,
	std::string const & file,
	std::map<std::string, std::string> variables
);

/// Preprocess a YAML node with only a directory as context.
/*
 * The directory is used to resolve relative include paths in the YAML.
 */
estd::result<void, estd::error> preprocessYamlWithDirectoryPath(YAML::Node & root,
	std::string const & directory,
	std::map<std::string, std::string> variables
);

}
