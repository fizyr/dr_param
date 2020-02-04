#pragma once
#include "xmlrpc.hpp"

#include <ros/param.h>
#include <ros/node_handle.h>

#ifndef DR_PARAM_WARN
#ifdef DR_WARN
#define DR_PARAM_WARN DR_WARN
#else
#include <ros/console.h>
#define DR_PARAM_WARN ROS_WARN_STREAM
#endif
#endif

#include <stdexcept>

/*
 * This header contains functions to read complex structures from the ROS parameter server.
 *
 * Use of this header is DEPRECATED.
 * Using the ROS parameter server is bad and should be avoided.
 * Use something more sane like a YAML configuration file instead.
 */


namespace dr {

/// Load a parameter from the ROS parameter service.
/**
 * Requires dr::fromXmlRpc<T> or dr::ConvertXmlRpc<T> to be specialized for T.
 *
 * \throws Any exception thrown by dr::fromXmlRpc<T>.
 * \return True if the parameter was found, false otherwise.
 */
template<typename T>
[[deprecated("Using the ROS parameter server is a bad idead. Consider using something else.")]]
bool loadParam(
	std::string const & key, ///< The key of the parameter to load.
	T & result               ///<[out] Output variable for the result.
) {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
	XmlRpc::XmlRpcValue value;
	if (!ros::param::get(key, value)) return false;
	try {
		result = fromXmlRpc<T>(value);
	} catch(std::exception const & e) {
		std::throw_with_nested(std::runtime_error("Failed to load parameter `" + key + "': " + e.what()));
	} catch (...) {
		std::throw_with_nested(std::runtime_error("Failed to load parameter `" + key + "'"));
	}
#pragma GCC diagnostic pop
}

/// Get a parameter from the ROS parameter service.
/**
 * Requires dr::fromXmlRpc<T> or dr::ConvertXmlRpc<T> to be specialized for T.
 *
 * \throws If the parameter can not be found or any exception thrown by dr::fromXmlRpc<T>.
 * \return The loaded parameter.
 */
template<typename T>
[[deprecated("Using the ROS parameter server is a bad idead. Consider using something else.")]]
T getParam(
	std::string const & key ///< The key of the parameter to load.
) {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
	XmlRpc::XmlRpcValue value;
	if (!ros::param::get(key, value)) throw std::runtime_error("ROS parameter not found: " + key);
	try {
		return fromXmlRpc<T>(value);
	} catch(std::exception const & e) {
		std::throw_with_nested(std::runtime_error("Failed to load parameter `" + key + "': " + e.what()));
	} catch (...) {
		std::throw_with_nested(std::runtime_error("Failed to load parameter `" + key + "'"));
	}
#pragma GCC diagnostic pop
}

/// Get a parameter from the ROS parameter service or a fallback value.
/**
 * Requires dr::fromXmlRpc<T> or dr::ConvertXmlRpc<T> to be specialized for T.
 *
 * \throws Any exception thrown by dr::fromXmlRpc<T>.
 * \return The loaded parameter or the fallback value if the parameter was not found.
 */
template<typename T>
[[deprecated("Using the ROS parameter server is a bad idead. Consider using something else.")]]
T getParam(
	std::string const & key, ///< The key of the parameter to load.
	T const & fallback,      ///< The fallback value to return if the parameter can not be found.
	bool warn = true         ///< If true, log a warning when the fallback default value is used.
) {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
	XmlRpc::XmlRpcValue value;
	if (!ros::param::get(key, value)) {
		if (warn) DR_PARAM_WARN("Failed to find ROS parameter: " << key << ". Using fallback value.");
		return fallback;
	}

	try {
		return fromXmlRpc<T>(value);
	} catch(std::exception const & e) {
		std::throw_with_nested(std::runtime_error("Failed to load parameter `" + key + "': " + e.what()));
	} catch (...) {
		std::throw_with_nested(std::runtime_error("Failed to load parameter `" + key + "'"));
	}
#pragma GCC diagnostic pop
}

template<typename T>
[[deprecated("Using the ROS parameter server is a bad idead. Consider using something else.")]]
bool loadParam(ros::NodeHandle const & node, std::string const & key, T & result) {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
	return loadParam(node.resolveName(key), result);
#pragma GCC diagnostic pop
}

template<typename T>
[[deprecated("Using the ROS parameter server is a bad idead. Consider using something else.")]]
T getParam(ros::NodeHandle const & node, std::string const & key) {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
	return getParam<T>(node.resolveName(key));
#pragma GCC diagnostic pop
}

template<typename T>
[[deprecated("Using the ROS parameter server is a bad idead. Consider using something else.")]]
T getParam(ros::NodeHandle const & node, std::string const & key, T const & fallback, bool warn = true) {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
	return getParam<T>(node.resolveName(key), fallback, warn);
#pragma GCC diagnostic pop
}

/// Get a vector from the ROS parameter server.
template<typename T>
[[deprecated("Using the ROS parameter server is a bad idead. Consider using something else.")]]
std::vector<T> getParamList(
	ros::NodeHandle const & node, ///< The node handle to use for parameter name resolution.
	std::string const & name      ///< The parameter to retrieve.
) {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
	return getParam<std::vector<T>>(node, name);
#pragma GCC diagnostic pop
}

/// Get a vector from the ROS parameter server.
template<typename T>
[[deprecated("Using the ROS parameter server is a bad idead. Consider using something else.")]]
std::vector<T> getParamList(
	ros::NodeHandle const & node,    ///< The node handle to use for parameter name resolution.
	std::string const & name,        ///< The parameter to retrieve.
	std::vector<T> const & fallback, ///< The fallback value to return if the parameter is not found.
	bool warn = true                 ///< If true, log a warning when the parameter was not found.
) {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
	return getParam<std::vector<T>>(node, name, fallback, warn);
#pragma GCC diagnostic pop
}

/// Get an array from the ROS parameter server.
template<typename T, std::size_t N>
[[deprecated("Using the ROS parameter server is a bad idead. Consider using something else.")]]
std::array<T, N> getParamArray(
	ros::NodeHandle const & node,      ///< The node handle to use for parameter name resolution.
	std::string const & name           ///< The parameter to retrieve.
) {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
	return getParam<std::array<T, N>>(node, name);
#pragma GCC diagnostic pop
}

/// Get an array from the ROS parameter server.
template<typename T, std::size_t N>
[[deprecated("Using the ROS parameter server is a bad idead. Consider using something else.")]]
std::array<T, N> getParamArray(
	ros::NodeHandle const & node,      ///< The node handle to use for parameter name resolution.
	std::string const & name,          ///< The parameter to retrieve.
	std::array<T, N> const & fallback, ///< The fallback value to return if the parameter is not found.
	bool warn = true                   ///< If true, log a warning when the parameter was not found.
) {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
	return getParam<std::array<T, N>>(node, name, fallback, warn);
#pragma GCC diagnostic pop
}

/// Get a map from the ROS parameter server.
template<typename K, typename T>
[[deprecated("Using the ROS parameter server is a bad idead. Consider using something else.")]]
std::map<K, T> getParamMap(
	ros::NodeHandle const & node,      ///< The node handle to use for parameter name resolution.
	std::string const & name,          ///< The parameter to retrieve.
	std::map<K, T> const & fallback,   ///< The fallback value to return if the parameter is not found.
	bool warn = true                   ///< If true, log a warning when the parameter was not found.
) {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
	return getParam<std::map<K, T>>(node, name, fallback, warn);
#pragma GCC diagnostic pop
}

}
