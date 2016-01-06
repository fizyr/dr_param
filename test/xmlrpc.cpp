#include "xmlrpc.hpp"

#include <gtest/gtest.h>

int main(int argc, char ** argv) {
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

namespace dr {

TEST(XmlrpcTest, ensureXmlRpcType) {
	XmlRpc::XmlRpcValue val_bool   = true;
	XmlRpc::XmlRpcValue val_int    = 0;
	XmlRpc::XmlRpcValue val_double = 3.14;
	XmlRpc::XmlRpcValue val_string = "Klaatu barada nikto";

	XmlRpc::XmlRpcValue val_list;
	val_list[0] = 41;

	XmlRpc::XmlRpcValue val_struct;
	val_struct["klaatu"] = "barada nikto";

	ASSERT_NO_THROW(ensureXmlRpcType(val_bool, XmlRpc::XmlRpcValue::TypeBoolean, ""));
	ASSERT_NO_THROW(ensureXmlRpcType(val_int, XmlRpc::XmlRpcValue::TypeInt, ""));
	ASSERT_NO_THROW(ensureXmlRpcType(val_double, XmlRpc::XmlRpcValue::TypeDouble, ""));
	ASSERT_NO_THROW(ensureXmlRpcType(val_string, XmlRpc::XmlRpcValue::TypeString, ""));
	ASSERT_NO_THROW(ensureXmlRpcType(val_list, XmlRpc::XmlRpcValue::TypeArray, ""));
	ASSERT_NO_THROW(ensureXmlRpcType(val_struct, XmlRpc::XmlRpcValue::TypeStruct, ""));

	ASSERT_THROW(ensureXmlRpcType(val_bool, XmlRpc::XmlRpcValue::TypeInt, ""), std::runtime_error);
	ASSERT_THROW(ensureXmlRpcType(val_int, XmlRpc::XmlRpcValue::TypeDouble, ""), std::runtime_error);
	ASSERT_THROW(ensureXmlRpcType(val_double, XmlRpc::XmlRpcValue::TypeString, ""), std::runtime_error);
	ASSERT_THROW(ensureXmlRpcType(val_string, XmlRpc::XmlRpcValue::TypeArray, ""), std::runtime_error);
	ASSERT_THROW(ensureXmlRpcType(val_list, XmlRpc::XmlRpcValue::TypeStruct, ""), std::runtime_error);
	ASSERT_THROW(ensureXmlRpcType(val_struct, XmlRpc::XmlRpcValue::TypeBoolean, ""), std::runtime_error);
}

TEST(XmlrpcTest, convertBoolean) {
	XmlRpc::XmlRpcValue val_true   = true;
	XmlRpc::XmlRpcValue val_false  = false;
	XmlRpc::XmlRpcValue val_int_0  = 0;
	XmlRpc::XmlRpcValue val_int_1  = 1;
	XmlRpc::XmlRpcValue val_int_42 = 42;
	XmlRpc::XmlRpcValue val_double = 3.14;
	XmlRpc::XmlRpcValue val_string = "Klaatu barada nikto";

	ASSERT_EQ(true,  fromXmlRpc<bool>(val_true));
	ASSERT_EQ(false, fromXmlRpc<bool>(val_false));
	ASSERT_EQ(false, fromXmlRpc<bool>(val_int_0));
	ASSERT_EQ(true,  fromXmlRpc<bool>(val_int_1));
	ASSERT_EQ(true,  fromXmlRpc<bool>(val_int_42));

	ASSERT_THROW(fromXmlRpc<bool>(val_double), std::exception);
	ASSERT_THROW(fromXmlRpc<bool>(val_string), std::exception);
}

TEST(XmlrpcTest, convertInteger) {
	XmlRpc::XmlRpcValue val_bool   = true;
	XmlRpc::XmlRpcValue val_int    = 42;
	XmlRpc::XmlRpcValue val_double = 3.14;
	XmlRpc::XmlRpcValue val_string = "Klaatu barada nikto";

	ASSERT_EQ(42, fromXmlRpc<int>(val_int));

	ASSERT_THROW(fromXmlRpc<int>(val_bool),   std::exception);
	ASSERT_THROW(fromXmlRpc<int>(val_double), std::exception);
	ASSERT_THROW(fromXmlRpc<int>(val_string), std::exception);
}

TEST(XmlrpcTest, convertDouble) {
	XmlRpc::XmlRpcValue val_bool   = true;
	XmlRpc::XmlRpcValue val_int    = 42;
	XmlRpc::XmlRpcValue val_double = 3.14;
	XmlRpc::XmlRpcValue val_string = "Klaatu barada nikto";

	ASSERT_DOUBLE_EQ(3.14, fromXmlRpc<double>(val_double));
	ASSERT_DOUBLE_EQ(42, fromXmlRpc<double>(val_int));

	ASSERT_THROW(fromXmlRpc<double>(val_bool),   std::exception);
	ASSERT_THROW(fromXmlRpc<double>(val_string), std::exception);
}

TEST(XmlrpcTest, convertString) {
	XmlRpc::XmlRpcValue val_bool   = true;
	XmlRpc::XmlRpcValue val_int    = 42;
	XmlRpc::XmlRpcValue val_double = 3.14;
	XmlRpc::XmlRpcValue val_string = "Klaatu barada nikto";

	ASSERT_EQ("Klaatu barada nikto", fromXmlRpc<std::string>(val_string));

	ASSERT_THROW(fromXmlRpc<std::string>(val_bool),   std::exception);
	ASSERT_THROW(fromXmlRpc<std::string>(val_int),    std::exception);
	ASSERT_THROW(fromXmlRpc<std::string>(val_double), std::exception);
}

TEST(XmlrpcTest, convertVectorInt) {
	XmlRpc::XmlRpcValue val;
	val[0] = 7;
	val[1] = 4;
	val[2] = 1;

	std::vector<int> result = fromXmlRpc<std::vector<int>>(val);
	ASSERT_EQ((std::vector<int>{7, 4, 1}), fromXmlRpc<std::vector<int>>(val));
}

TEST(XmlrpcTest, convertVectorString) {
	XmlRpc::XmlRpcValue val;
	val[0] = "klaatu";
	val[1] = "barada";
	val[2] = "nikto";

	ASSERT_EQ((std::vector<std::string>{"klaatu", "barada", "nikto"}), fromXmlRpc<std::vector<std::string>>(val));
}

TEST(XmlrpcTest, convertVectorInvalid) {
	XmlRpc::XmlRpcValue val;
	val[0] = 7;
	val[1] = 4;
	val[2] = 1;

	ASSERT_THROW(fromXmlRpc<std::vector<std::string>>(val), std::exception);
}

TEST(XmlrpcTest, convertMapInt) {
	XmlRpc::XmlRpcValue val;
	val["aap"]  = 0;
	val["noot"] = 1;
	val["mies"] = 2;

	ASSERT_EQ((std::map<std::string, int>{{"aap", 0}, {"noot", 1}, {"mies", 2}}), (fromXmlRpc<std::map<std::string, int>>(val)));
}

TEST(XmlrpcTest, convertMapString) {
	XmlRpc::XmlRpcValue val;
	val["aap"]  = "wim";
	val["noot"] = "zus";
	val["mies"] = "jet";

	ASSERT_EQ((std::map<std::string, std::string>{{"aap", "wim"}, {"noot", "zus"}, {"mies", "jet"}}), (fromXmlRpc<std::map<std::string, std::string>>(val)));
}

TEST(XmlrpcTest, convertMapInvalid) {
	XmlRpc::XmlRpcValue val;
	val["aap"]  = 0;
	val["noot"] = 1;
	val["mies"] = 2;

	ASSERT_THROW((fromXmlRpc<std::map<std::string, std::string>>(val)), std::exception);
}

TEST(XmlrpcTest, convertArrayInt) {
	XmlRpc::XmlRpcValue val;
	val[0] = 7;
	val[1] = 4;
	val[2] = 1;

	ASSERT_EQ((std::array<int, 3>{{7, 4, 1}}), (fromXmlRpc<std::array<int, 3>>(val)));
}

TEST(XmlrpcTest, convertArrayString) {
	XmlRpc::XmlRpcValue val;
	val[0] = "klaatu";
	val[1] = "barada";
	val[2] = "nikto";

	ASSERT_EQ((std::array<std::string, 3>{{"klaatu", "barada", "nikto"}}), (fromXmlRpc<std::array<std::string, 3>>(val)));
}

TEST(XmlrpcTest, convertArrayInvalidType) {
	XmlRpc::XmlRpcValue val;
	val[0] = 7;
	val[1] = 4;
	val[2] = 1;

	ASSERT_THROW((fromXmlRpc<std::array<std::string, 3>>(val)), std::exception);
}

TEST(XmlrpcTest, convertArrayInvalidSize) {
	XmlRpc::XmlRpcValue val;
	val[0] = 7;
	val[1] = 4;
	val[2] = 1;

	ASSERT_THROW((fromXmlRpc<std::array<int, 2>>(val)), std::exception);
	ASSERT_THROW((fromXmlRpc<std::array<int, 4>>(val)), std::exception);
}

}
