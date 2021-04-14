/// Catch
#include <catch2/catch.hpp>

/// Fizyr
#include "yaml.hpp"
#include <estd/result/catch_string_conversions.hpp>

namespace dr {

TEST_CASE("array conversions", "[array]") {

	std::array<int, 2> original{{1, 2}};
	std::string encoded = YAML::Dump(encodeYaml(original));
	YamlResult<std::array<int, 2>> decoded = parseYaml<std::array<int, 2>>(YAML::Load(encoded));
	REQUIRE(decoded );
	REQUIRE(*decoded == original);

	decoded = parseYaml<std::array<int, 2>>(YAML::Load("[1, 2]"));
	REQUIRE(decoded);
	REQUIRE(*decoded == original);

	decoded = parseYaml<std::array<int, 2>>(YAML::Load("[1, 2, 3]"));
	REQUIRE(!decoded);
}

TEST_CASE("yaml node conversions", "[yaml_node]") {
	YAML::Node original;
	int number = 1;
	std::string string = "one";
	original["number"] = number;
	original["string"] = string;
	std::string encoded = YAML::Dump(encodeYaml(original));
	auto decoded = parseYaml<YAML::Node>(YAML::Load(encoded));
	REQUIRE(decoded);
	REQUIRE(decoded->size() == original.size());
	auto decoded_number = parseYaml<int>((*decoded)["number"]);
	auto decoded_string = parseYaml<std::string>((*decoded)["string"]);
	REQUIRE(decoded_number == number);
	REQUIRE(decoded_string == string);
}

TEST_CASE("merge yaml nodes", "[yaml_node]") {
	YAML::Node map_a = YAML::Load("{name: aap, list: [1 , 2, 3]}");
	YAML::Node map_b = YAML::Load("{list: [5], movie: book}");

	auto merged = mergeYamlNodes(map_a, map_b);
	REQUIRE(merged);
	REQUIRE(map_a["name"].as<std::string>() == "aap");
	REQUIRE(map_a["list"].size() == 1);
	REQUIRE(map_a["list"][0].as<int>() == 5);
	REQUIRE(map_a["movie"].as<std::string>() == "book");
}

TEST_CASE("merge yaml nodes recursive", "[yaml_node]") {
	YAML::Node map_a = YAML::Load("{name: aap, sub: {list: [1 , 2, 3], year: 2020}}");
	YAML::Node map_b = YAML::Load("{sub: {list: [5], year: 2019}}");

	auto merged = mergeYamlNodes(map_a, map_b);
	REQUIRE(merged);
	REQUIRE(map_a["name"].as<std::string>() == "aap");
	REQUIRE(map_a["sub"]["list"].size() == 1);
	REQUIRE(map_a["sub"]["list"][0].as<int>() == 5);
	REQUIRE(map_a["sub"]["year"].as<int>() == 2019);
}

TEST_CASE("merge nodes recursive", "[yaml_node]") {
	YAML::Node map_a = YAML::Load("{name: alpro, l2: [{q: 1},{w: 4}], l3: [1,2,3]}");
	YAML::Node map_b = YAML::Load("{name: oatly, l2: [{q: 2}, {w: 5}, {a: 8}], l3: [4]}");
	auto merged = mergeNodes(map_a, map_b);
	REQUIRE(merged);
	REQUIRE(map_a["name"].as<std::string>() == "oatly");
	REQUIRE(map_a["l2"][0]["q"].as<int>()  == 2);
	REQUIRE(map_a["l2"][1]["w"].as<int>()  == 5);
	REQUIRE(map_a["l2"][2]["a"].as<int>()  == 8);
	REQUIRE(map_a["l3"][0].as<int>()  == 4);
	
}

TEST_CASE("Merge into an empty YAML node", "[yaml_node]") {
	YAML::Node a;

	YAML::Node b;
	b["aap"] = 1;
	b["noot"] = 2;
	b["mies"] = 3;

	REQUIRE(mergeYamlNodes(a, b));

	CHECK(a.size() == 3);
	CHECK(a["aap"].as<int>() == 1);
	CHECK(a["noot"].as<int>() == 2);
	CHECK(a["mies"].as<int>() == 3);
}

TEST_CASE("Merge from an empty YAML node", "[yaml_node]") {
	YAML::Node a;
	a["aap"] = 1;
	a["noot"] = 2;
	a["mies"] = 3;

	REQUIRE(mergeYamlNodes(a, YAML::Node{}));

	CHECK(a.size() == 3);
	CHECK(a["aap"].as<int>() == 1);
	CHECK(a["noot"].as<int>() == 2);
	CHECK(a["mies"].as<int>() == 3);
}

}
