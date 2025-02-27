#include "yaml.hpp"
#include "yaml_preprocess.hpp"

#include <dr_util/expand.hpp>

#include <boost/filesystem.hpp>

namespace dr {

using namespace std::string_literals;

namespace {
	namespace fs = boost::filesystem;

	struct PathInfo {
		fs::path dir;
		std::optional<fs::path> file;

		static PathInfo forFile(std::string const & file) {
			return PathInfo{fs::path{file}.parent_path(), fs::path{file}};
		}

		static PathInfo forDirectory(std::string const & dir) {
			return PathInfo{fs::path{dir}, std::nullopt};
		}
	};

	struct Work {
		PathInfo path_info;
		std::vector<YAML::Node> nodes;
	};

	void updateVariables(std::map<std::string, std::string> & variables, PathInfo const & path_info) {
		variables["DIR"] = path_info.dir.empty() ? "." : path_info.dir.lexically_normal().native();
		if (path_info.file) variables["FILE"] = path_info.file->lexically_normal().native();
		else variables.erase("FILE");
	}

	estd::result<void, estd::error> includeFile(YAML::Node & node, std::vector<Work> & work, PathInfo const & path_info, std::map<std::string, std::string> const & variables) {
		if (!node.IsScalar()) return estd::error{std::errc::invalid_argument, "!include needs a string"};

		// Expand variables in path and normalize path.
		boost::filesystem::path path = expandVariables(node.as<std::string>(), variables);
		if (path.empty()) return estd::error{std::errc::invalid_argument, "tried to include empty path"};
		if (path.is_relative()) path = path_info.dir / path;
		// TOOD: should we just use lexically_normal() instead?
		auto normal_path = boost::filesystem::canonical(path);

		// Parse node, process tags and overwrite original.
		node.SetTag("");
		node = readYamlFile(normal_path.native()).value();

		// Queue node for reprocessing.
		work.push_back(Work{PathInfo{normal_path.parent_path(), normal_path}, {node}});

		return estd::in_place_valid;
	}

	estd::result<void, estd::error> expandVars(YAML::Node & node, std::map<std::string, std::string> const & variables) {
		if (!node.IsScalar()) return estd::error{std::errc::invalid_argument, "!expand needs a string"};
		node.SetTag("");
		node = expandVariables(node.as<std::string>(), variables);
		return estd::in_place_valid;
	}

	estd::result<bool, estd::error> processSingle(YAML::Node & node, std::vector<Work> & work, PathInfo const & path_info, std::map<std::string, std::string> const & variables) {
		if (node.Tag() == "!include") {
			estd::result<void, estd::error> result = includeFile(node, work, path_info, variables);
			if (!result) return result.error_unchecked();
			return true;
		}
		if (node.Tag() == "!expand") {
			estd::result<void, estd::error> result = expandVars(node, variables);
			if (!result) return result.error_unchecked();
			return true;
		}
		return false;
	}

	estd::result<void, estd::error> processRecursive(YAML::Node & root, PathInfo const & path_info, std::map<std::string, std::string> variables) {
		std::vector<Work> work;
		work.push_back(Work{path_info, {root}});

		while (!work.empty()) {
			Work current_work = std::move(work.back());
			work.pop_back();
			updateVariables(variables, current_work.path_info);

			while (!current_work.nodes.empty()) {
				YAML::Node node = current_work.nodes.back();
				current_work.nodes.pop_back();

				// Tag handlers must queue processed (child) nodes themselves, possibly with different PathInfo.
				estd::result<bool, estd::error> changed = processSingle(node, work, current_work.path_info, variables);
				if (!changed) return changed.error_unchecked();
				if (*changed) continue;

				// Process children.
				if (node.IsMap())      for (YAML::iterator i = node.begin(); i != node.end(); ++i) current_work.nodes.push_back(i->second);
				if (node.IsSequence()) for (YAML::iterator i = node.begin(); i != node.end(); ++i) current_work.nodes.push_back(*i);
			}
		}
		return estd::in_place_valid;
	}
}

estd::result<void, estd::error> preprocessYamlWithFilePath(YAML::Node & root, std::string const & file, std::map<std::string, std::string> variables) {
	return processRecursive(root, PathInfo::forFile(file), std::move(variables));
}

estd::result<void, estd::error> preprocessYamlWithDirectoryPath(YAML::Node & root, std::string const & directory, std::map<std::string, std::string> variables) {
	return processRecursive(root, PathInfo::forDirectory(directory), std::move(variables));
}

estd::result<YAML::Node, estd::error> preprocessYamlFile(std::string const & path, std::map<std::string, std::string> variables) {
	estd::result<YAML::Node, estd::error> node = readYamlFile(path);
	if (!node) return node.error_unchecked();

	estd::result<void, estd::error> result = preprocessYamlWithFilePath(*node, path, std::move(variables));
	if (!result) return result.error_unchecked();

	return *node;
}



}
