/// Catch
#include <catch2/catch.hpp>

/// Fizyr
#include "yaml_preprocess.hpp"

/*
#include <gtest/gtest.h>

int main(int argc, char ** argv) {
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

namespace dr {

#define STRINGIFY(TEXT) #TEXT
#define STRINGIFY_MACRO(TEXT) STRINGIFY(TEXT)

std::string data_path = STRINGIFY_MACRO(TEST_DATA);

TEST(YamlPreprocess, expand_simple) {
	YAML::Node node = YAML::Load("thing: !expand $test");
	preprocessYamlWithFilePath(node, "/example/location", {{"test", "aap"}});
	ASSERT_EQ(node["thing"].as<std::string>(), "aap");

	node = YAML::Load("thing: !expand $test/noot");
	preprocessYamlWithFilePath(node, "/example/location", {{"test", "aap"}});
	ASSERT_EQ(node["thing"].as<std::string>(), "aap/noot");

	node = YAML::Load("thing: !expand aap/$test/mies");
	preprocessYamlWithFilePath(node, "/example/location", {{"test", "noot"}});
	ASSERT_EQ(node["thing"].as<std::string>(), "aap/noot/mies");
}

TEST(YamlPreprocess, expand_brackets) {
	YAML::Node node = YAML::Load("thing: !expand ${test}");
	preprocessYamlWithFilePath(node, "/example/location", {{"test", "aap"}});
	ASSERT_EQ(node["thing"].as<std::string>(), "aap");

	node = YAML::Load("thing: !expand ${test}noot");
	preprocessYamlWithFilePath(node, "/example/location", {{"test", "aap"}});
	ASSERT_EQ(node["thing"].as<std::string>(), "aapnoot");

	node = YAML::Load("thing: !expand aap${test}mies");
	preprocessYamlWithFilePath(node, "/example/location", {{"test", "noot"}});
	ASSERT_EQ(node["thing"].as<std::string>(), "aapnootmies");
}

TEST(YamlPreprocess, expand_dir) {
	YAML::Node node;

	node = YAML::Load("thing: !expand $DIR");
	preprocessYamlWithFilePath(node, "/example/location", {});
	ASSERT_EQ(node["thing"].as<std::string>(), "/example");

	node = YAML::Load("thing: !expand $DIR");
	preprocessYamlWithDirectoryPath(node, "/example/location/", {});
}

TEST(YamlPreprocess, expand_dir_empty) {
	YAML::Node node;

	node = YAML::Load("thing: !expand $DIR/other.yaml");
	preprocessYamlWithFilePath(node, "file.yaml", {});
	ASSERT_EQ(node["thing"].as<std::string>(), "./other.yaml");

	node = YAML::Load("thing: !expand $DIR/other.yaml");
	preprocessYamlWithDirectoryPath(node, "", {});
	ASSERT_EQ(node["thing"].as<std::string>(), "./other.yaml");
}

TEST(YamlPreprocess, expand_file) {
	YAML::Node node;

	node = YAML::Load("thing: !expand $FILE");
	preprocessYamlWithFilePath(node, "path/file.yaml", {});
	ASSERT_EQ(node["thing"].as<std::string>(), "path/file.yaml");

	node = YAML::Load("thing: !expand $FILE");
	preprocessYamlWithDirectoryPath(node, "", {});
	ASSERT_EQ(node["thing"].as<std::string>(), "");
}

TEST(YamlPreprocess, include) {
	auto node = preprocessYamlFile(data_path + "/include.yaml", {});
	ASSERT_TRUE(node) << node.error_or().format();
	ASSERT_EQ((*node)["b"]["foo"].as<std::string>(), "bar");
}

TEST(YamlPreprocess, include_recursive) {
	auto node = preprocessYamlFile(data_path + "/recursive_include.yaml", {});
	ASSERT_TRUE(node) << node.error_or().format();
	ASSERT_EQ((*node)["a"]["b"]["foo"].as<std::string>(), "bar");
}

}
*/

namespace dr {

#define STRINGIFY(TEXT) #TEXT
#define STRINGIFY_MACRO(TEXT) STRINGIFY(TEXT)

std::string data_path = STRINGIFY_MACRO(TEST_DATA);

TEST_CASE("YamlPreprocess 0", "expand_simple") {
	YAML::Node node = YAML::Load("thing: !expand $test");
	preprocessYamlWithFilePath(node, "/example/location", {{"test", "aap"}});
	REQUIRE(node["thing"].as<std::string>() == "aap");

	node = YAML::Load("thing: !expand $test/noot");
	preprocessYamlWithFilePath(node, "/example/location", {{"test", "aap"}});
	REQUIRE(node["thing"].as<std::string>() == "aap/noot");

	node = YAML::Load("thing: !expand aap/$test/mies");
	preprocessYamlWithFilePath(node, "/example/location", {{"test", "noot"}});
	REQUIRE(node["thing"].as<std::string>() == "aap/noot/mies");
}

TEST_CASE("YamlPreprocess 1", "expand_brackets") {
	YAML::Node node = YAML::Load("thing: !expand ${test}");
	preprocessYamlWithFilePath(node, "/example/location", {{"test", "aap"}});
	REQUIRE(node["thing"].as<std::string>() == "aap");

	node = YAML::Load("thing: !expand ${test}noot");
	preprocessYamlWithFilePath(node, "/example/location", {{"test", "aap"}});
	REQUIRE(node["thing"].as<std::string>() == "aapnoot");

	node = YAML::Load("thing: !expand aap${test}mies");
	preprocessYamlWithFilePath(node, "/example/location", {{"test", "noot"}});
	REQUIRE(node["thing"].as<std::string>() == "aapnootmies");
}

TEST_CASE("YamlPreprocess 2", "expand_dir") {
	YAML::Node node;

	node = YAML::Load("thing: !expand $DIR");
	preprocessYamlWithFilePath(node, "/example/location", {});
	REQUIRE(node["thing"].as<std::string>() == "/example");

	node = YAML::Load("thing: !expand $DIR");
	preprocessYamlWithDirectoryPath(node, "/example/location/", {});
}

TEST_CASE("YamlPreprocess 3", "expand_dir_empty") {
	YAML::Node node;

	node = YAML::Load("thing: !expand $DIR/other.yaml");
	preprocessYamlWithFilePath(node, "file.yaml", {});
	REQUIRE(node["thing"].as<std::string>() == "./other.yaml");

	node = YAML::Load("thing: !expand $DIR/other.yaml");
	preprocessYamlWithDirectoryPath(node, "", {});
	REQUIRE(node["thing"].as<std::string>() == "./other.yaml");
}

TEST_CASE("YamlPreprocess 4", "expand_file") {
	YAML::Node node;

	node = YAML::Load("thing: !expand $FILE");
	preprocessYamlWithFilePath(node, "path/file.yaml", {});
	REQUIRE(node["thing"].as<std::string>() == "path/file.yaml");

	node = YAML::Load("thing: !expand $FILE");
	preprocessYamlWithDirectoryPath(node, "", {});
	REQUIRE(node["thing"].as<std::string>() == "");
}

TEST_CASE("YamlPreprocess 5", "include") {
	auto node = preprocessYamlFile(data_path + "/include.yaml", {});
	REQUIRE(node); // << node.error_or().format();                              REV
	REQUIRE((*node)["b"]["foo"].as<std::string>() == "bar");
}

TEST_CASE("YamlPreprocess 6", "include_recursive") {
	auto node = preprocessYamlFile(data_path + "/recursive_include.yaml", {});
	REQUIRE(node); // << node.error_or().format();                              REV
	REQUIRE((*node)["a"]["b"]["foo"].as<std::string>() == "bar");
}

}
