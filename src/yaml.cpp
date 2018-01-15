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

namespace estd {

dr::ErrorOr<std::string> convert(YAML::Node const & node, Parse<std::string, dr::DetailedError>) {
	if (auto error = dr::expectScalar(node)) return error;
	return node.Scalar();
}

dr::ErrorOr<bool> convert(YAML::Node const & node, Parse<bool, dr::DetailedError>) {
	if (auto error = dr::expectScalar(node)) return error;

	std::string raw = node.Scalar();
	std::transform(raw.begin(), raw.end(), raw.begin(), [] (char c) { return std::tolower(c); });

	if (raw == "y" || raw == "yes" || raw == "true"  || raw == "on"   || raw == "1") return true;
	if (raw == "n" || raw == "no"  || raw == "false" || raw == "ooff" || raw == "0") return false;
	return dr::DetailedError{std::errc::invalid_argument, "invalid boolean value: " + node.Scalar()};
}

namespace {
	template<typename T>
	dr::ErrorOr<T> convert_signed_integral(YAML::Node const & node) {
		if (auto error = dr::expectScalar(node)) return error;

		std::string const & raw = node.Scalar();
		std::size_t parsed = 0;
		long long value = 0;
		try {
			value = std::stoll(raw, &parsed);
		} catch (std::invalid_argument &e) {
			return dr::DetailedError{std::errc::invalid_argument, "invalid integer value: " + raw};
		} catch (std::range_error const & e) {
			return dr::DetailedError{std::errc::invalid_argument, "integer value out of range: " + raw};
		}

		if (parsed != raw.size()) return dr::DetailedError{std::errc::invalid_argument, "invalid integer value: " + raw};
		if (value > std::numeric_limits<T>::max())    dr::DetailedError{std::errc::invalid_argument, "integer value out of range: " + raw};
		if (value < std::numeric_limits<T>::lowest()) dr::DetailedError{std::errc::invalid_argument, "integer value out of range: " + raw};
		return T(value);
	}

	template<typename T>
	dr::ErrorOr<T> convert_unsigned_integral(YAML::Node const & node) {
		if (auto error = dr::expectScalar(node)) return error;

		std::string const & raw = node.Scalar();
		std::size_t parsed = 0;
		unsigned long long value = 0;
		try {
			value = std::stoull(raw, &parsed);
		} catch (std::invalid_argument &e) {
			return dr::DetailedError{std::errc::invalid_argument, "invalid integer value: " + raw};
		} catch (std::range_error const & e) {
			return dr::DetailedError{std::errc::invalid_argument, "integer value out of range: " + raw};
		}

		if (parsed != raw.size()) return dr::DetailedError{std::errc::invalid_argument, "invalid integer value: " + raw};
		if (value > std::numeric_limits<T>::max())    dr::DetailedError{std::errc::invalid_argument, "integer value out of range: " + raw};
		if (value < std::numeric_limits<T>::lowest()) dr::DetailedError{std::errc::invalid_argument, "integer value out of range: " + raw};
		return T(value);
	}

	template<typename T>
	dr::ErrorOr<T> convert_floating_point(YAML::Node const & node) {
		if (auto error = dr::expectScalar(node)) return error;

		std::string const & raw = node.Scalar();
		std::size_t parsed = 0;
		unsigned long long value = 0;
		try {
			value = std::stold(raw, &parsed);
		} catch (std::invalid_argument &e) {
			return dr::DetailedError{std::errc::invalid_argument, "invalid floating point value: " + raw};
		} catch (std::range_error const & e) {
			return dr::DetailedError{std::errc::invalid_argument, "floating point value out of range: " + raw};
		}

		if (parsed != raw.size()) return dr::DetailedError{std::errc::invalid_argument, "invalid floating point value: " + raw};
		return T(value);
	}
}

dr::ErrorOr<short>     convert(YAML::Node const & node, Parse<short,     dr::DetailedError>) { return convert_signed_integral<short    >(node); }
dr::ErrorOr<int>       convert(YAML::Node const & node, Parse<int,       dr::DetailedError>) { return convert_signed_integral<int      >(node); }
dr::ErrorOr<long>      convert(YAML::Node const & node, Parse<long,      dr::DetailedError>) { return convert_signed_integral<long     >(node); }
dr::ErrorOr<long long> convert(YAML::Node const & node, Parse<long long, dr::DetailedError>) { return convert_signed_integral<long long>(node); }

dr::ErrorOr<unsigned short>     convert(YAML::Node const & node, Parse<unsigned short,     dr::DetailedError>) { return convert_unsigned_integral<unsigned short    >(node); }
dr::ErrorOr<unsigned int>       convert(YAML::Node const & node, Parse<unsigned int,       dr::DetailedError>) { return convert_unsigned_integral<unsigned int      >(node); }
dr::ErrorOr<unsigned long>      convert(YAML::Node const & node, Parse<unsigned long,      dr::DetailedError>) { return convert_unsigned_integral<unsigned long     >(node); }
dr::ErrorOr<unsigned long long> convert(YAML::Node const & node, Parse<unsigned long long, dr::DetailedError>) { return convert_unsigned_integral<unsigned long long>(node); }

dr::ErrorOr<float>       convert(YAML::Node const & node, Parse<float,       dr::DetailedError>) { return convert_floating_point<float>      (node); }
dr::ErrorOr<double>      convert(YAML::Node const & node, Parse<double,      dr::DetailedError>) { return convert_floating_point<double>     (node); }
dr::ErrorOr<long double> convert(YAML::Node const & node, Parse<long double, dr::DetailedError>) { return convert_floating_point<long double>(node); }

}
