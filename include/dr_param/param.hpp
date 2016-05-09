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


namespace dr {

/// Load a parameter from the ROS parameter service.
/**
 * Requires dr::fromXmlRpc<T> or dr::ConvertXmlRpc<T> to be specialized for T.
 *
 * \throws Any exception thrown by dr::fromXmlRpc<T>.
 * \return True if the parameter was found, false otherwise.
 */
template<typename T>
bool loadParam(
	std::string const & key, ///< The key of the parameter to load.
	T & result               ///<[out] Output variable for the result.
) {
	XmlRpc::XmlRpcValue value;
	if (!ros::param::get(key, value)) return false;
	try {
		result = fromXmlRpc<T>(value);
	} catch(std::exception const & e) {
		std::throw_with_nested(std::runtime_error("Failed to load parameter `" + key + "': " + e.what()));
	} catch (...) {
		std::throw_with_nested(std::runtime_error("Failed to load parameter `" + key + "'"));
	}
}

/// Get a parameter from the ROS parameter service.
/**
 * Requires dr::fromXmlRpc<T> or dr::ConvertXmlRpc<T> to be specialized for T.
 *
 * \throws If the parameter can not be found or any exception thrown by dr::fromXmlRpc<T>.
 * \return The loaded parameter.
 */
template<typename T>
T getParam(
	std::string const & key ///< The key of the parameter to load.
) {
	XmlRpc::XmlRpcValue value;
	if (!ros::param::get(key, value)) throw std::runtime_error("ROS parameter not found: " + key);
	try {
		return fromXmlRpc<T>(value);
	} catch(std::exception const & e) {
		std::throw_with_nested(std::runtime_error("Failed to load parameter `" + key + "': " + e.what()));
	} catch (...) {
		std::throw_with_nested(std::runtime_error("Failed to load parameter `" + key + "'"));
	}
}

/// Get a parameter from the ROS parameter service or a fallback value.
/**
 * Requires dr::fromXmlRpc<T> or dr::ConvertXmlRpc<T> to be specialized for T.
 *
 * \throws Any exception thrown by dr::fromXmlRpc<T>.
 * \return The loaded parameter or the fallback value if the parameter was not found.
 */
template<typename T>
T getParam(
	std::string const & key, ///< The key of the parameter to load.
	T const & fallback,      ///< The fallback value to return if the parameter can not be found.
	bool warn = true         ///< If true, log a warning when the fallback default value is used.
) {
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
}

template<typename T>
bool loadParam(ros::NodeHandle const & node, std::string const & key, T & result) {
	return loadParam(node.resolveName(key), result);
}

template<typename T>
T getParam(ros::NodeHandle const & node, std::string const & key) {
	return getParam<T>(node.resolveName(key));
}

template<typename T>
T getParam(ros::NodeHandle const & node, std::string const & key, T const & fallback, bool warn = true) {
	return getParam<T>(node.resolveName(key), fallback, warn);
}

/// Get a vector from the ROS parameter server.
template<typename T>
std::vector<T> getParamList(
	ros::NodeHandle const & node, ///< The node handle to use for parameter name resolution.
	std::string const & name      ///< The parameter to retrieve.
) {
	return getParam<std::vector<T>>(node, name);
}

/// Get a vector from the ROS parameter server.
template<typename T>
std::vector<T> getParamList(
	ros::NodeHandle const & node,    ///< The node handle to use for parameter name resolution.
	std::string const & name,        ///< The parameter to retrieve.
	std::vector<T> const & fallback, ///< The fallback value to return if the parameter is not found.
	bool warn = true                 ///< If true, log a warning when the parameter was not found.
) {
	return getParam<std::vector<T>>(node, name, fallback, warn);
}

/// Get an array from the ROS parameter server.
template<typename T, std::size_t N>
std::array<T, N> getParamArray(
	ros::NodeHandle const & node,      ///< The node handle to use for parameter name resolution.
	std::string const & name           ///< The parameter to retrieve.
) {
	return getParam<std::array<T, N>>(node, name);
}

/// Get an array from the ROS parameter server.
template<typename T, std::size_t N>
std::array<T, N> getParamArray(
	ros::NodeHandle const & node,      ///< The node handle to use for parameter name resolution.
	std::string const & name,          ///< The parameter to retrieve.
	std::array<T, N> const & fallback, ///< The fallback value to return if the parameter is not found.
	bool warn = true                   ///< If true, log a warning when the parameter was not found.
) {
	return getParam<std::array<T, N>>(node, name, fallback, warn);
}

/// Get a map from the ROS parameter server.
template<typename K, typename T>
std::map<K, T> getParamMap(
	ros::NodeHandle const & node,      ///< The node handle to use for parameter name resolution.
	std::string const & name,          ///< The parameter to retrieve.
	std::map<K, T> const & fallback,   ///< The fallback value to return if the parameter is not found.
	bool warn = true                   ///< If true, log a warning when the parameter was not found.
) {
	return getParam<std::map<K, T>>(node, name, fallback, warn);
}

}
