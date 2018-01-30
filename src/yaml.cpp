#include "yaml.hpp"

#include <dr_util/expand.hpp>

#include <fmt/format.h>

#include <boost/filesystem.hpp>

#include <cerrno>
#include <fstream>

namespace dr {

using namespace std::string_literals;

YamlError::YamlError(std::string message, std::vector<YamlNodeDescription> trace) :
	message{std::move(message)},
	trace{std::move(trace)} {}

YamlError & YamlError::appendTrace(YamlNodeDescription description) & {
	trace.push_back(std::move(description));
	return *this;
}

YamlError && YamlError::appendTrace(YamlNodeDescription description) && {
	trace.push_back(std::move(description));
	return std::move(*this);
}

std::string YamlError::formatTrace() const {
	if (trace.empty()) return "";

	std::string result = trace.back().name;

	for (auto i = trace.rbegin(); i != trace.rend(); ++i) {
		// Skip the first node (already dealt with it).
		if (i == trace.rbegin()) continue;
		auto prev = i - 1;
		if (prev->node_type == YAML::NodeType::Sequence) {
			result.push_back('[');
			result.append(i->name);
			result.push_back(']');
		} else {
			result.push_back('.');
			result.append(i->name);
		}
	}

	return result;
}

std::string YamlError::format() const {
	if (trace.empty()) return message;
	return formatTrace() + ": " + message;
}

std::string toString(YAML::NodeType::value type) {
	switch (type) {
		case YAML::NodeType::Map:       return "map";
		case YAML::NodeType::Null:      return "null";
		case YAML::NodeType::Scalar:    return "scalar";
		case YAML::NodeType::Sequence:  return "sequence";
		case YAML::NodeType::Undefined: return "undefined";
	}
	return "unknown";
}

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
		return DetailedError{{error, std::generic_category()}, path};
	}
	std::stringstream buffer;
	buffer << file.rdbuf();
	return YAML::Load(buffer.str());
}

// New style YAML conversions.

YamlResult<std::string> convert(YAML::Node const & node, ParseYaml<std::string>) {
	if (!node.IsScalar()) return YamlError{"unexpected node type, expected scalar, got " + toString(node.Type())};
	return node.Scalar();
}

YamlResult<bool> convert(YAML::Node const & node, ParseYaml<bool>) {
	if (!node.IsScalar()) return YamlError{"unexpected node type, expected scalar, got " + toString(node.Type())};

	std::string raw = node.Scalar();
	std::transform(raw.begin(), raw.end(), raw.begin(), [] (char c) { return std::tolower(c); });

	if (raw == "y" || raw == "yes" || raw == "true"  || raw == "on"   || raw == "1") return true;
	if (raw == "n" || raw == "no"  || raw == "false" || raw == "ooff" || raw == "0") return false;
	return YamlError{"invalid boolean value: " + node.Scalar()};
}

namespace {
	template<typename T>
	YamlResult<T> convert_signed_integral(YAML::Node const & node) {
		if (!node.IsScalar()) return YamlError{"unexpected node type, expected scalar, got " + toString(node.Type())};

		std::string const & raw = node.Scalar();
		std::size_t parsed = 0;
		long long value = 0;
		try {
			value = std::stoll(raw, &parsed);
		} catch (std::invalid_argument &e) {
			return YamlError{"invalid integer value: " + raw};
		} catch (std::range_error const & e) {
			return YamlError{"integer value out of range: " + raw};
		}

		if (parsed != raw.size()) return YamlError{"invalid integer value: " + raw};
		if (value > std::numeric_limits<T>::max())    YamlError{"integer value out of range: " + raw};
		if (value < std::numeric_limits<T>::lowest()) YamlError{"integer value out of range: " + raw};
		return T(value);
	}

	template<typename T>
	YamlResult<T> convert_unsigned_integral(YAML::Node const & node) {
		if (!node.IsScalar()) return YamlError{"unexpected node type, expected scalar, got " + toString(node.Type())};

		std::string const & raw = node.Scalar();
		std::size_t parsed = 0;
		unsigned long long value = 0;
		try {
			value = std::stoull(raw, &parsed);
		} catch (std::invalid_argument &e) {
			return YamlError{"invalid integer value: " + raw};
		} catch (std::range_error const & e) {
			return YamlError{"integer value out of range: " + raw};
		}

		if (parsed != raw.size()) return YamlError{"invalid integer value: " + raw};
		if (value > std::numeric_limits<T>::max())    YamlError{"integer value out of range: " + raw};
		if (value < std::numeric_limits<T>::lowest()) YamlError{"integer value out of range: " + raw};
		return T(value);
	}

	template<typename T>
	YamlResult<T> convert_floating_point(YAML::Node const & node) {
		if (!node.IsScalar()) return YamlError{"unexpected node type, expected scalar, got " + toString(node.Type())};

		std::string const & raw = node.Scalar();
		std::size_t parsed = 0;
		long double value = 0;
		try {
			value = std::stold(raw, &parsed);
		} catch (std::invalid_argument &e) {
			return YamlError{"invalid floating point value: " + raw};
		} catch (std::range_error const & e) {
			return YamlError{"floating point value out of range: " + raw};
		}

		if (parsed != raw.size()) return YamlError{"invalid floating point value: " + raw};
		return T(value);
	}
}

YamlResult<short>     convert(YAML::Node const & node, ParseYaml<short>)     { return convert_signed_integral<short    >(node); }
YamlResult<int>       convert(YAML::Node const & node, ParseYaml<int>)       { return convert_signed_integral<int      >(node); }
YamlResult<long>      convert(YAML::Node const & node, ParseYaml<long>)      { return convert_signed_integral<long     >(node); }
YamlResult<long long> convert(YAML::Node const & node, ParseYaml<long long>) { return convert_signed_integral<long long>(node); }

YamlResult<unsigned short>     convert(YAML::Node const & node, ParseYaml<unsigned short>)     { return convert_unsigned_integral<unsigned short    >(node); }
YamlResult<unsigned int>       convert(YAML::Node const & node, ParseYaml<unsigned int>)       { return convert_unsigned_integral<unsigned int      >(node); }
YamlResult<unsigned long>      convert(YAML::Node const & node, ParseYaml<unsigned long>)      { return convert_unsigned_integral<unsigned long     >(node); }
YamlResult<unsigned long long> convert(YAML::Node const & node, ParseYaml<unsigned long long>) { return convert_unsigned_integral<unsigned long long>(node); }

YamlResult<float>       convert(YAML::Node const & node, ParseYaml<float>)       { return convert_floating_point<float>      (node); }
YamlResult<double>      convert(YAML::Node const & node, ParseYaml<double>)      { return convert_floating_point<double>     (node); }
YamlResult<long double> convert(YAML::Node const & node, ParseYaml<long double>) { return convert_floating_point<long double>(node); }

}
