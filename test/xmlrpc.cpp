/// Catch
#include <catch2/catch.hpp>

/// Fizyr
#include "xmlrpc.hpp"

#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

namespace dr {

TEST_CASE("XmlrpcTest 0", "ensureXmlRpcType") {
	XmlRpc::XmlRpcValue val_bool   = true;
	XmlRpc::XmlRpcValue val_int    = 0;
	XmlRpc::XmlRpcValue val_double = 3.14;
	XmlRpc::XmlRpcValue val_string = "Klaatu barada nikto";

	XmlRpc::XmlRpcValue val_list;
	val_list[0] = 41;

	XmlRpc::XmlRpcValue val_struct;
	val_struct["klaatu"] = "barada nikto";

	REQUIRE_NOTHROW(ensureXmlRpcType(val_bool, XmlRpc::XmlRpcValue::TypeBoolean, ""));
	REQUIRE_NOTHROW(ensureXmlRpcType(val_int, XmlRpc::XmlRpcValue::TypeInt, ""));
	REQUIRE_NOTHROW(ensureXmlRpcType(val_double, XmlRpc::XmlRpcValue::TypeDouble, ""));
	REQUIRE_NOTHROW(ensureXmlRpcType(val_string, XmlRpc::XmlRpcValue::TypeString, ""));
	REQUIRE_NOTHROW(ensureXmlRpcType(val_list, XmlRpc::XmlRpcValue::TypeArray, ""));
	REQUIRE_NOTHROW(ensureXmlRpcType(val_struct, XmlRpc::XmlRpcValue::TypeStruct, ""));

	REQUIRE_THROWS_AS(ensureXmlRpcType(val_bool, XmlRpc::XmlRpcValue::TypeInt, ""), std::runtime_error);
	REQUIRE_THROWS_AS(ensureXmlRpcType(val_int, XmlRpc::XmlRpcValue::TypeDouble, ""), std::runtime_error);
	REQUIRE_THROWS_AS(ensureXmlRpcType(val_double, XmlRpc::XmlRpcValue::TypeString, ""), std::runtime_error);
	REQUIRE_THROWS_AS(ensureXmlRpcType(val_string, XmlRpc::XmlRpcValue::TypeArray, ""), std::runtime_error);
	REQUIRE_THROWS_AS(ensureXmlRpcType(val_list, XmlRpc::XmlRpcValue::TypeStruct, ""), std::runtime_error);
	REQUIRE_THROWS_AS(ensureXmlRpcType(val_struct, XmlRpc::XmlRpcValue::TypeBoolean, ""), std::runtime_error);
}

TEST_CASE("XmlrpcTest 1", "convertBoolean") {
	XmlRpc::XmlRpcValue val_true   = true;
	XmlRpc::XmlRpcValue val_false  = false;
	XmlRpc::XmlRpcValue val_int_0  = 0;
	XmlRpc::XmlRpcValue val_int_1  = 1;
	XmlRpc::XmlRpcValue val_int_42 = 42;
	XmlRpc::XmlRpcValue val_double = 3.14;
	XmlRpc::XmlRpcValue val_string = "Klaatu barada nikto";

	REQUIRE( true == fromXmlRpc<bool>(val_true));
	REQUIRE(false == fromXmlRpc<bool>(val_false));
	REQUIRE(false == fromXmlRpc<bool>(val_int_0));
	REQUIRE( true == fromXmlRpc<bool>(val_int_1));
	REQUIRE( true == fromXmlRpc<bool>(val_int_42));

	REQUIRE_THROWS_AS(fromXmlRpc<bool>(val_double), std::exception);
	REQUIRE_THROWS_AS(fromXmlRpc<bool>(val_string), std::exception);
}

TEST_CASE("XmlrpcTest 2", "convertInteger") {
	XmlRpc::XmlRpcValue val_bool   = true;
	XmlRpc::XmlRpcValue val_int    = 42;
	XmlRpc::XmlRpcValue val_double = 3.14;
	XmlRpc::XmlRpcValue val_string = "Klaatu barada nikto";

	REQUIRE(42 == fromXmlRpc<int>(val_int));

	REQUIRE_THROWS_AS(fromXmlRpc<int>(val_bool),   std::exception);
	REQUIRE_THROWS_AS(fromXmlRpc<int>(val_double), std::exception);
	REQUIRE_THROWS_AS(fromXmlRpc<int>(val_string), std::exception);
}

TEST_CASE("XmlrpcTest 3", "convertDouble") {
	XmlRpc::XmlRpcValue val_bool   = true;
	XmlRpc::XmlRpcValue val_int    = 42;
	XmlRpc::XmlRpcValue val_double = 3.14;
	XmlRpc::XmlRpcValue val_string = "Klaatu barada nikto";
	
	REQUIRE_THAT(fromXmlRpc<double>(val_double), Catch::WithinULP(3.14, 0));
	REQUIRE(fromXmlRpc<double>(val_int) == 42);

	REQUIRE_THROWS_AS(fromXmlRpc<double>(val_bool),   std::exception);
	REQUIRE_THROWS_AS(fromXmlRpc<double>(val_string), std::exception);
}

TEST_CASE("XmlrpcTest 4", "convertString") {
	XmlRpc::XmlRpcValue val_bool   = true;
	XmlRpc::XmlRpcValue val_int    = 42;
	XmlRpc::XmlRpcValue val_double = 3.14;
	XmlRpc::XmlRpcValue val_string = "Klaatu barada nikto";

	REQUIRE("Klaatu barada nikto" == fromXmlRpc<std::string>(val_string));

	REQUIRE_THROWS_AS(fromXmlRpc<std::string>(val_bool),   std::exception);
	REQUIRE_THROWS_AS(fromXmlRpc<std::string>(val_int),    std::exception);
	REQUIRE_THROWS_AS(fromXmlRpc<std::string>(val_double), std::exception);
}

TEST_CASE("XmlrpcTest 5", "convertVectorInt") {
	XmlRpc::XmlRpcValue val;
	val[0] = 7;
	val[1] = 4;
	val[2] = 1;

	std::vector<int> result = fromXmlRpc<std::vector<int>>(val);
	REQUIRE((std::vector<int>{7, 4, 1}) == fromXmlRpc<std::vector<int>>(val));
}

TEST_CASE("XmlrpcTest 6", "convertVectorString") {
	XmlRpc::XmlRpcValue val;
	val[0] = "klaatu";
	val[1] = "barada";
	val[2] = "nikto";

	REQUIRE((std::vector<std::string>{"klaatu", "barada", "nikto"}) == fromXmlRpc<std::vector<std::string>>(val));
}

TEST_CASE("XmlrpcTest 7", "convertVectorInvalid") {
	XmlRpc::XmlRpcValue val;
	val[0] = 7;
	val[1] = 4;
	val[2] = 1;

	REQUIRE_THROWS_AS(fromXmlRpc<std::vector<std::string>>(val), std::exception);
}

TEST_CASE("XmlrpcTest 8", "convertMapInt") {
	XmlRpc::XmlRpcValue val;
	val["aap"]  = 0;
	val["noot"] = 1;
	val["mies"] = 2;

	REQUIRE((std::map<std::string, int>{{"aap", 0}, {"noot", 1}, {"mies", 2}}) == (fromXmlRpc<std::map<std::string, int>>(val)));
}

TEST_CASE("XmlrpcTest 9", "convertMapString") {
	XmlRpc::XmlRpcValue val;
	val["aap"]  = "wim";
	val["noot"] = "zus";
	val["mies"] = "jet";

	REQUIRE((std::map<std::string, std::string>{{"aap", "wim"}, {"noot", "zus"}, {"mies", "jet"}}) == (fromXmlRpc<std::map<std::string, std::string>>(val)));
}

TEST_CASE("XmlrpcTest 10", "convertMapInvalid") {
	XmlRpc::XmlRpcValue val;
	val["aap"]  = 0;
	val["noot"] = 1;
	val["mies"] = 2;

	REQUIRE_THROWS_AS((fromXmlRpc<std::map<std::string, std::string>>(val)), std::exception);
}

TEST_CASE("XmlrpcTest 11", "convertArrayInt") {
	XmlRpc::XmlRpcValue val;
	val[0] = 7;
	val[1] = 4;
	val[2] = 1;

	REQUIRE((std::array<int, 3>{{7, 4, 1}}) == (fromXmlRpc<std::array<int, 3>>(val)));
}

TEST_CASE("XmlrpcTest 12", "convertArrayString") {
	XmlRpc::XmlRpcValue val;
	val[0] = "klaatu";
	val[1] = "barada";
	val[2] = "nikto";

	REQUIRE((std::array<std::string, 3>{{"klaatu", "barada", "nikto"}}) == (fromXmlRpc<std::array<std::string, 3>>(val)));
}

TEST_CASE("XmlrpcTest 13", "convertArrayInvalidType") {
	XmlRpc::XmlRpcValue val;
	val[0] = 7;
	val[1] = 4;
	val[2] = 1;

	REQUIRE_THROWS_AS((fromXmlRpc<std::array<std::string, 3>>(val)), std::exception);
}

TEST_CASE("XmlrpcTest 14", "convertArrayInvalidSize") {
	XmlRpc::XmlRpcValue val;
	val[0] = 7;
	val[1] = 4;
	val[2] = 1;

	REQUIRE_THROWS_AS((fromXmlRpc<std::array<int, 2>>(val)), std::exception);
	REQUIRE_THROWS_AS((fromXmlRpc<std::array<int, 4>>(val)), std::exception);
}

}
