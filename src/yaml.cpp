#include "yaml.hpp"

#include <cerrno>
#include <fstream>

namespace dr {
	ErrorOr<YAML::Node> readYamlFile(std::string const & path) {
		std::ifstream file(path);
		if (!file.good()) {
			int error = errno;
			return DetailedError{{error, std::generic_category()}, "failed to open " + path + ": " + std::strerror(error)};
		}
		std::stringstream buffer;
		buffer << file.rdbuf();
		return YAML::Load(buffer.str());
	}
}
