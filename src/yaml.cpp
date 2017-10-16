#include "yaml.hpp"

#include <dr_util/expand.hpp>

#include <fmt/format.h>

#include <boost/filesystem.hpp>

#include <cerrno>
#include <fstream>

namespace dr {

using namespace std::string_literals;

DetailedError expectMap(YAML::Node const & node) {
	if (!node) return DetailedError{std::errc::invalid_argument, fmt::format("no such node")};
	if (node.IsMap()) return {};
	return DetailedError{std::errc::invalid_argument, fmt::format("invalid node type at {}:{}: expected a map", node.Mark().line, node.Mark().column)};
}

DetailedError expectMap(YAML::Node const & node, std::size_t size) {
	if (auto error = expectMap(node)) return error;
	if (node.size() == size) return {};
	return DetailedError{std::errc::invalid_argument, fmt::format("invalid node size at {}:{}: expected {} child nodes, got {}", node.Mark().line, node.Mark().column, size, node.size())};
}

DetailedError expectSequence(YAML::Node const & node) {
	if (!node) return DetailedError{std::errc::invalid_argument, fmt::format("no such node")};
	if (node.IsSequence()) return {};
	return DetailedError{std::errc::invalid_argument, fmt::format("invalid node type at {}:{}: expected a list", node.Mark().line, node.Mark().column)};
}
DetailedError expectSequence(YAML::Node const & node, std::size_t size) {
	if (auto error = expectSequence(node)) return error;
	if (node.size() == size) return {};
	return DetailedError{std::errc::invalid_argument, fmt::format("invalid list size at {}:{}: expected {} elements, got {}", node.Mark().line, node.Mark().column, size, node.size())};
}

DetailedError expectScalar(YAML::Node const & node) {
	if (!node) return DetailedError{std::errc::invalid_argument, fmt::format("no such node")};
	if (node.IsScalar()) return {};
	return DetailedError{std::errc::invalid_argument, fmt::format("invalid node type at {}:{}: expected a scalar", node.Mark().line, node.Mark().column)};
}

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
