#include "xmlrpc.hpp"

namespace dr {

bool xmlRpcAsBool(XmlRpc::XmlRpcValue const & value) {
	// TODO: Ugly hack to circumvent lack of const accessors.
	return const_cast<XmlRpc::XmlRpcValue &>(value);
}

int xmlRpcAsInt(XmlRpc::XmlRpcValue const & value) {
	// TODO: Ugly hack to circumvent lack of const accessors.
	return const_cast<XmlRpc::XmlRpcValue &>(value);
}

double xmlRpcAsDouble(XmlRpc::XmlRpcValue const & value) {
	// TODO: Ugly hack to circumvent lack of const accessors.
	return const_cast<XmlRpc::XmlRpcValue &>(value);
}

std::string const & xmlRpcAsString(XmlRpc::XmlRpcValue const & value) {
	// TODO: Ugly hack to circumvent lack of const accessors.
	return const_cast<XmlRpc::XmlRpcValue &>(value);
}

XmlRpc::XmlRpcValue const & xmlRpcAt(XmlRpc::XmlRpcValue const & value, std::string const & key) {
	// TODO: Ugly hack to circumvent lack of const accessors.
	if (!value.hasMember(key)) throw std::runtime_error("key `" + key + "' not found");
	return const_cast<XmlRpc::XmlRpcValue &>(value)[key];
}

XmlRpc::XmlRpcValue::ValueStruct::const_iterator xmlRpcBegin(XmlRpc::XmlRpcValue const & value) {
	// TODO: Ugly hack to circumvent lack of const accessors.
	return const_cast<XmlRpc::XmlRpcValue &>(value).begin();
}

XmlRpc::XmlRpcValue::ValueStruct::const_iterator xmlRpcEnd(XmlRpc::XmlRpcValue const & value) {
	// TODO: Ugly hack to circumvent lack of const accessors.
	return const_cast<XmlRpc::XmlRpcValue &>(value).end();
}

std::string xmlRpcTypeName(XmlRpc::XmlRpcValue::Type type) {
	switch (type) {
		case XmlRpc::XmlRpcValue::TypeArray:    return "array";
		case XmlRpc::XmlRpcValue::TypeBase64:   return "base64";
		case XmlRpc::XmlRpcValue::TypeBoolean:  return "boolean";
		case XmlRpc::XmlRpcValue::TypeDateTime: return "datetime";
		case XmlRpc::XmlRpcValue::TypeDouble:   return "double";
		case XmlRpc::XmlRpcValue::TypeInt:      return "int";
		case XmlRpc::XmlRpcValue::TypeInvalid:  return "invalid";
		case XmlRpc::XmlRpcValue::TypeString:   return "string";
		case XmlRpc::XmlRpcValue::TypeStruct:   return "struct";
	}

	return "unknown (" + std::to_string(type) + ")";
}

std::runtime_error makeXmlRpcTypeError(XmlRpc::XmlRpcValue::Type type, std::string const & target_type) {
	return std::runtime_error("Cannot convert XmlRpc type " + xmlRpcTypeName(type) + " to " + target_type + ".");
}

void ensureXmlRpcType(XmlRpc::XmlRpcValue const & value, XmlRpc::XmlRpcValue::Type wanted, std::string const & target_type) {
	if (value.getType() != wanted) throw makeXmlRpcTypeError(value.getType(), target_type);
}

template<>
bool fromXmlRpc<bool>(XmlRpc::XmlRpcValue const & value) {
	if (value.getType() == XmlRpc::XmlRpcValue::TypeBoolean) return bool(XmlRpc::XmlRpcValue(value));
	if (value.getType() == XmlRpc::XmlRpcValue::TypeInt)     return int(XmlRpc::XmlRpcValue(value));
	throw makeXmlRpcTypeError(value.getType(), "boolean");
}

template<>
int fromXmlRpc<int>(XmlRpc::XmlRpcValue const & value) {
	if (value.getType() == XmlRpc::XmlRpcValue::TypeInt) return int(XmlRpc::XmlRpcValue(value));
	throw makeXmlRpcTypeError(value.getType(), "integer");
}

template<>
double fromXmlRpc<double>(XmlRpc::XmlRpcValue const & value) {
	if (value.getType() == XmlRpc::XmlRpcValue::TypeDouble) return double(XmlRpc::XmlRpcValue(value));
	if (value.getType() == XmlRpc::XmlRpcValue::TypeInt)    return int(XmlRpc::XmlRpcValue(value));
	throw makeXmlRpcTypeError(value.getType(), "double");
}

template<>
float fromXmlRpc<float>(XmlRpc::XmlRpcValue const & value) {
	if (value.getType() == XmlRpc::XmlRpcValue::TypeDouble) return double(XmlRpc::XmlRpcValue(value));
	if (value.getType() == XmlRpc::XmlRpcValue::TypeInt)    return int(XmlRpc::XmlRpcValue(value));
	throw makeXmlRpcTypeError(value.getType(), "float");
}

template<>
std::string fromXmlRpc<std::string>(XmlRpc::XmlRpcValue const & value) {
	if (value.getType() == XmlRpc::XmlRpcValue::TypeString) return std::string(XmlRpc::XmlRpcValue(value));
	throw makeXmlRpcTypeError(value.getType(), "string");
}

}
