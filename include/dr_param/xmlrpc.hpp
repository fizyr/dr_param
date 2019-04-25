#pragma once

#include <xmlrpcpp/XmlRpcValue.h>

#include <array>
#include <string>
#include <vector>

/*
 * This header contains utilities to convert complex structs to/from XMLRPC representation.
 * It is mainly used by param.hpp to load parameters from the ROS parameter server.
 *
 * Use of this header is DEPRECATED.
 * Using XMLRPC or the ROS parameter server is bad and should be avoided.
 * Use something more sane like a YAML configuration file instead.
 */

namespace dr {

/// Access an XmlRpc value as a boolean.
bool xmlRpcAsBool(XmlRpc::XmlRpcValue const & value);

/// Access an XmlRpc value as an int.
int xmlRpcAsInt(XmlRpc::XmlRpcValue const & value);

/// Access an XmlRpc value as a double.
double xmlRpcAsDouble(XmlRpc::XmlRpcValue const & value);

/// Access an XmlRpc value as a string.
std::string const & xmlRpcAsString(XmlRpc::XmlRpcValue const & value);

/// Get a member of an XmlRpcValue struct.
XmlRpc::XmlRpcValue const & xmlRpcAt(XmlRpc::XmlRpcValue const & value, std::string const & key);

/// Get an iterator to the first member of an XmlRpcValue struct.
XmlRpc::XmlRpcValue::ValueStruct::const_iterator xmlRpcBegin(XmlRpc::XmlRpcValue const & value);

/// Get an end iterator for the members of an XmlRpcValue struct.
XmlRpc::XmlRpcValue::ValueStruct::const_iterator xmlRpcEnd(XmlRpc::XmlRpcValue const & value);

/// Convert an XmlRpcValue::Type to a string.
std::string xmlRpcTypeName(XmlRpc::XmlRpcValue::Type type);

/// Make a runtime error for an unsupported XmlRpcValue::Type.
std::runtime_error makeXmlRpcTypeError(XmlRpc::XmlRpcValue::Type type, std::string const & target_type);

/// Ensure an XmlRpcValue is the correct type.
void ensureXmlRpcType(XmlRpc::XmlRpcValue const & value, XmlRpc::XmlRpcValue::Type wanted, std::string const & target_type);

/// Struct to convert an XmlRpc value to a T.
/**
 * Must be explicitly specialized for supported types.
 *
 * Specializations must implement a member to do the conversion:
 *   static T convert(XmlRpc::XmlRpcValue const &);
 */
template<typename T>
struct ConvertXmlRpc;

/// Load a value from a XmlRpcValue.
/**
 * Needs static T ConvertXmlRpc<T>::convert(XmlRpc::XmlRpcValue const &) to be defined.
 *
 * \throws Anything ConvertXmlRpc<T>::convert() throws.
 * \return The loaded value.
 */
template<typename T>
[[deprecated]]
T fromXmlRpc(
	XmlRpc::XmlRpcValue const & value ///< The XmlRpcValue to load from.
) {
	return ConvertXmlRpc<T>::convert(value);
}

template<>
[[deprecated]]
bool fromXmlRpc<bool>(XmlRpc::XmlRpcValue const & value);

template<>
[[deprecated]]
int fromXmlRpc<int>(XmlRpc::XmlRpcValue const & value);

template<>
[[deprecated]]
double fromXmlRpc<double>(XmlRpc::XmlRpcValue const & value);

template<>
[[deprecated]]
float fromXmlRpc<float>(XmlRpc::XmlRpcValue const & value);

template<>
[[deprecated]]
std::string fromXmlRpc<std::string>(XmlRpc::XmlRpcValue const & value);

template<typename T>
struct [[deprecated]] ConvertXmlRpc<std::vector<T>> {
	static std::vector<T> convert(XmlRpc::XmlRpcValue const & value) {
		ensureXmlRpcType(value, XmlRpc::XmlRpcValue::TypeArray, "vector");

		std::vector<T> result;
		result.reserve(value.size());

		for (int i = 0; i < value.size(); ++i) {
			result.push_back(fromXmlRpc<T>(value[i]));
		}

		return result;
	}
};

template<typename T>
struct [[deprecated]] ConvertXmlRpc<std::map<std::string, T>> {
	static std::map<std::string, T> convert(XmlRpc::XmlRpcValue const & value) {
		ensureXmlRpcType(value, XmlRpc::XmlRpcValue::TypeStruct, "map");

		std::map<std::string, T> result;

		for (XmlRpc::XmlRpcValue::ValueStruct::const_iterator i = xmlRpcBegin(value); i != xmlRpcEnd(value); ++i) {
			result.insert({i->first, fromXmlRpc<T>(i->second)});
		}

		return result;
	}
};

template<typename T, std::size_t N>
struct [[deprecated]] ConvertXmlRpc<std::array<T, N>> {
	static std::array<T, N> convert(XmlRpc::XmlRpcValue const & value) {
		ensureXmlRpcType(value, XmlRpc::XmlRpcValue::TypeArray, "array");
		if (value.size() != N) throw std::runtime_error("Wrong size: " + std::to_string(value.size()) + " (expected " + std::to_string(N) + ")");

		std::array<T, N> result;
		for (std::size_t i = 0; i < N; ++i) {
			result[i] = fromXmlRpc<T>(value[i]);
		}

		return result;
	}
};

}
