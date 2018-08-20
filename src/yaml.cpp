#include "yaml.hpp"
#include "yaml_macros.hpp"

#include <fmt/format.h>

#include <algorithm>
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

std::optional<YamlError> expectMap(YAML::Node const & node) {
	if (!node) return YamlError{"no such node"};
	if (!node) return std::nullopt;
	if (node.IsMap()) return std::nullopt;
	return YamlError{fmt::format("invalid node type: expected map, got {}", toString(node.Type()))};
}

std::optional<YamlError> expectMap(YAML::Node const & node, std::size_t size) {
	if (auto error = expectMap(node)) return error;
	if (node.size() == size) return std::nullopt;
	return YamlError{fmt::format("invalid map size: expected {} child nodes, got {}",size, node.size())};
}

std::optional<YamlError> expectSequence(YAML::Node const & node) {
	if (!node) return YamlError{"no such node"};
	if (node.IsSequence()) return std::nullopt;
	return YamlError{fmt::format("invalid node type: expected list, got {}", toString(node.Type()))};
}
std::optional<YamlError> expectSequence(YAML::Node const & node, std::size_t size) {
	if (auto error = expectSequence(node)) return error;
	if (node.size() == size) return std::nullopt;
	return YamlError{fmt::format("invalid list size: expected {} elements, got {}", size, node.size())};
}

std::optional<YamlError> expectScalar(YAML::Node const & node) {
	if (!node) return YamlError{"no such node"};
	if (node.IsScalar()) return std::nullopt;
	return YamlError{fmt::format("invalid node type: expected scalar, got {}", toString(node.Type()))};
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

}

// New style YAML conversions.
using namespace dr;

namespace {
	template<typename T>
	YamlResult<T> convert_signed_integral(YAML::Node const & node) {
		if (auto error = expectScalar(node)) return *error;

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
		if (auto error = expectScalar(node)) return *error;

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
		if (auto error = expectScalar(node)) return *error;

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

DR_PARAM_DEFINE_YAML_DECODE(std::string, node) {
	if (auto error = expectScalar(node)) return *error;
	return node.Scalar();
}

DR_PARAM_DEFINE_YAML_DECODE(bool, node) {
	if (auto error = expectScalar(node)) return *error;

	std::string raw = node.Scalar();
	std::transform(raw.begin(), raw.end(), raw.begin(), [] (char c) { return std::tolower(c); });

	if (raw == "y" || raw == "yes" || raw == "true"  || raw == "on"  || raw == "1") return true;
	if (raw == "n" || raw == "no"  || raw == "false" || raw == "off" || raw == "0") return false;
	return dr::YamlError{"invalid boolean value: " + node.Scalar()};
}

YAML::Node estd::conversion<bool, YAML::Node>::perform(bool value) noexcept { return YAML::Node(value); }

DR_PARAM_DEFINE_YAML_DECODE(char     , node) { return convert_signed_integral<char     >(node); }
DR_PARAM_DEFINE_YAML_DECODE(short    , node) { return convert_signed_integral<short    >(node); }
DR_PARAM_DEFINE_YAML_DECODE(int      , node) { return convert_signed_integral<int      >(node); }
DR_PARAM_DEFINE_YAML_DECODE(long     , node) { return convert_signed_integral<long     >(node); }
DR_PARAM_DEFINE_YAML_DECODE(long long, node) { return convert_signed_integral<long long>(node); }

DR_PARAM_DEFINE_YAML_DECODE(unsigned char     , node) { return convert_unsigned_integral<unsigned char     >(node); }
DR_PARAM_DEFINE_YAML_DECODE(unsigned short    , node) { return convert_unsigned_integral<unsigned short    >(node); }
DR_PARAM_DEFINE_YAML_DECODE(unsigned int      , node) { return convert_unsigned_integral<unsigned int      >(node); }
DR_PARAM_DEFINE_YAML_DECODE(unsigned long     , node) { return convert_unsigned_integral<unsigned long     >(node); }
DR_PARAM_DEFINE_YAML_DECODE(unsigned long long, node) { return convert_unsigned_integral<unsigned long long>(node); }

DR_PARAM_DEFINE_YAML_DECODE(float      , node) { return convert_floating_point<float>      (node); }
DR_PARAM_DEFINE_YAML_DECODE(double     , node) { return convert_floating_point<double>     (node); }
DR_PARAM_DEFINE_YAML_DECODE(long double, node) { return convert_floating_point<long double>(node); }

YAML::Node estd::conversion<char,      YAML::Node>::perform(char      value) noexcept { return YAML::Node(value); }
YAML::Node estd::conversion<short,     YAML::Node>::perform(short     value) noexcept { return YAML::Node(value); }
YAML::Node estd::conversion<int,       YAML::Node>::perform(int       value) noexcept { return YAML::Node(value); }
YAML::Node estd::conversion<long,      YAML::Node>::perform(long      value) noexcept { return YAML::Node(value); }
YAML::Node estd::conversion<long long, YAML::Node>::perform(long long value) noexcept { return YAML::Node(value); }

YAML::Node estd::conversion<unsigned char,      YAML::Node>::perform(unsigned char      value) noexcept { return YAML::Node(value); }
YAML::Node estd::conversion<unsigned short,     YAML::Node>::perform(unsigned short     value) noexcept { return YAML::Node(value); }
YAML::Node estd::conversion<unsigned int,       YAML::Node>::perform(unsigned int       value) noexcept { return YAML::Node(value); }
YAML::Node estd::conversion<unsigned long,      YAML::Node>::perform(unsigned long      value) noexcept { return YAML::Node(value); }
YAML::Node estd::conversion<unsigned long long, YAML::Node>::perform(unsigned long long value) noexcept { return YAML::Node(value); }

YAML::Node estd::conversion<float,       YAML::Node>::perform(float       value) noexcept { return YAML::Node(value); }
YAML::Node estd::conversion<double,      YAML::Node>::perform(double      value) noexcept { return YAML::Node(value); }
YAML::Node estd::conversion<long double, YAML::Node>::perform(long double value) noexcept { return YAML::Node(value); }
