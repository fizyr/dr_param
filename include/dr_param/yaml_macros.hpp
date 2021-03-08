#pragma once
#include "./yaml.hpp"

/**
 * This header contains macros to more easily declare and/or define YAML conversions.
 */

/// Declare a YAML decoding conversion.
/**
 * This macro must be invoked from the global namespace.
 *
 * Typical usage:
 *   DR_PARAM_DECLARE_YAML_DECODE(MyStruct);
 */
#define DR_PARAM_DECLARE_YAML_DECODE(TYPE) template<> struct estd::conversion<::YAML::Node, ::dr::YamlResult<TYPE>> {\
	static ::dr::YamlResult<TYPE> perform(::YAML::Node const &); \
}

/// Declare a YAML encoding conversion.
/**
 * This macro must be invoked from the global namespace.
 *
 * Typical usage:
 *   DR_PARAM_DECLARE_YAML_ENCODE(MyStruct);
 */
#define DR_PARAM_DECLARE_YAML_ENCODE(TYPE) template<> struct estd::conversion<TYPE, ::YAML::Node> {\
	static ::YAML::Node perform(TYPE const &); \
}

/// Declare a YAML decoding conversion.
/**
 * Deprecated: Use DR_PARAM_DECLARE_YAML_DECODE.
 *
 * This macro must be invoked from the global namespace.
 */
#define DR_PARAM_DECLARE_YAML_CONVERSION(TYPE) _Pragma("GCC warning \"'DR_PARAM_DECLARE_YAML_CONVERSION' macro is deprecated\"") DR_PARAM_DECLARE_YAML_DECODE(TYPE)

/// Define a YAML encoding conversion.
/**
 * This macro must be invoked from the global namespace.
 * The conversion MUST already be declared in order to use this macro.
 * That can be done using the DR_PARAM_DECLARE_YAML_ENCODE.
 *
 * Typical usage:
 *   DR_PARAM_DEFINE_YAML_ENCODE(MyStruct, value) {
 *     // convert value, return a YAML::Node
 *   }
 */
#define DR_PARAM_DEFINE_YAML_ENCODE(TYPE, VALUE) inline ::YAML::Node estd::conversion<TYPE, YAML::Node>::perform(TYPE const & VALUE)

/// Define a YAML decoding conversion.
/**
 * This macro must be invoked from the global namespace.
 * The conversion MUST already be declared in order to use this macro.
 * That can be done using the DR_PARAM_DECLARE_YAML_DECODE.
 *
 * Typical usage:
 *   DR_PARAM_DEFINE_YAML_DECODE(MyStruct, node) {
 *     // Parse node (YAML::Node) into a value.
 *     // Return a dr::YamlResult<MyStruct>
 *   }
 */
#define DR_PARAM_DEFINE_YAML_DECODE(TYPE, NODE) inline ::dr::YamlResult<TYPE> estd::conversion<YAML::Node, ::dr::YamlResult<TYPE>>::perform(::YAML::Node const & NODE)

/// Define a YAML decoding conversion.
/**
 * Deprecated: Use DR_PARAM_DEFINE_YAML_DECODE.
 *
 * This macro must be invoked from the global namespace.
 */
#define DR_PARAM_DEFINE_YAML_CONVERSION(TYPE, NODE) _Pragma("GCC warning \"'DR_PARAM_DEFINE_YAML_CONVERSION' macro is deprecated\"") DR_PARAM_DEFINE_YAML_DECODE(TYPE, NODE)

/// Declare and define a YAML encoding conversion.
/**
 * This macro must be invoked from the global namespace.
 *
 * Typical usage:
 *   DR_PARAM_YAML_ENCODE(MyStruct, value) {
 *     // convert value, return a YAML::Node
 *   }
 */
#define DR_PARAM_YAML_ENCODE(TYPE, VALUE) DR_PARAM_DECLARE_YAML_ENCODE(TYPE); \
DR_PARAM_DEFINE_YAML_ENCODE(TYPE, VALUE)

/// Declare and define a YAML decoding conversion.
/**
 * This macro must be invoked from the global namespace.
 *
 * Typical usage:
 *   DR_PARAM_YAML_DECODE(MyStruct, node) {
 *     // Parse node (YAML::Node) into a value.
 *     // Return a dr::YamlResult<MyStruct>
 *   }
 */
#define DR_PARAM_YAML_DECODE(TYPE, NODE) DR_PARAM_DECLARE_YAML_DECODE(TYPE); \
DR_PARAM_DEFINE_YAML_DECODE(TYPE, NODE)

/// Declare and define a YAML decoding conversion.
/**
 * Deprecated: Use DR_PARAM_YAML_DECODE.
 *
 * This macro must be invoked from the global namespace.
 */
#define DR_PARAM_YAML_CONVERSION(TYPE, NODE) _Pragma("GCC warning \"'DR_PARAM_YAML_CONVERSION' macro is deprecated\"") DR_PARAM_YAML_DECODE(TYPE)
