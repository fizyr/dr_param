#pragma once
#define DR_PARAM_DECLARE_YAML_CONVERSION(TYPE) template<> struct conversion<::YAML::Node, ::dr::YamlResult<TYPE>> {\
	static ::dr::YamlResult<TYPE> perform(::YAML::Node const &) noexcept; \
}

#define DR_PARAM_DEFINE_YAML_CONVERSION(TYPE, NODE) ::dr::YamlResult<TYPE> conversion<YAML::Node, ::dr::YamlResult<TYPE>>::perform(::YAML::Node const & NODE) noexcept

#define DR_PARAM_YAML_CONVERSION(TYPE, NODE) DR_PARAM_DECLARE_YAML_CONVERSION(TYPE); \
DR_PARAM_DEFINE_YAML_CONVERSION(TYPE, NODE)
