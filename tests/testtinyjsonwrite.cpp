// Licensed to Florent Guelfucci under one or more agreements.
// Florent Guelfucci licenses this file to you under the MIT license.
// See the LICENSE file in the project root for more information.
#include <fstream>
#include <cstring>
#include <gtest/gtest.h>
#define TJ_USE_CHAR 1
#include "../src/TinyJSON.h"

class TestWrite : public ::testing::Test {
protected:
  const char* filename = "test_json.json";

  void TearDown() override {
    // Remove the test file after each test
    std::remove(filename);
  }

  bool file_exists(const char* filename) {
    std::ifstream file(filename);
    return file.good();
  }

  std::string read_fileContent(const char* filename) 
  {
    std::ifstream file(filename, std::ios::in | std::ios::binary);
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    return content;
  }

  bool has_Utf8BOM(const char* filename)
  {
    // Open the file with std::ifstream
    std::ifstream inFile(filename, std::ios::in | std::ios::binary);

    // Check if the file was successfully opened
    if (!inFile) {
      std::cerr << "Error opening file: " << filename << std::endl;
      return false;
    }

    // Read the first three bytes
    char bom[3] = { 0 };
    inFile.read(bom, 3);

    // Check if the read was successful
    if (!inFile) {
      std::cerr << "Error reading file: " << filename << std::endl;
      return false;
    }

    // UTF-8 BOM
    const char utf8BOM[3] = { '\xEF', '\xBB', '\xBF' };

    // Check if the first three bytes match the UTF-8 BOM
    return std::memcmp(bom, utf8BOM, 3) == 0;
  }
};

TEST_F(TestWrite, FileIsCreated) 
{
  auto json = TinyJSON::TJ::parse("{}");
  ASSERT_NE(nullptr, json);
  ASSERT_TRUE(TinyJSON::TJ::write_file(filename, *json));
  ASSERT_TRUE(file_exists(filename));
  delete json;
}

TEST_F(TestWrite, EmptyObject)
{
  auto json = TinyJSON::TJ::parse("{}");
  ASSERT_NE(nullptr, json);
  ASSERT_TRUE(TinyJSON::TJ::write_file(filename, *json));
  ASSERT_STREQ("{}", read_fileContent(filename).c_str());
  delete json;
}

TEST_F(TestWrite, EmptyArray)
{
  auto json = TinyJSON::TJ::parse("[]");
  ASSERT_NE(nullptr, json);
  ASSERT_TRUE(TinyJSON::TJ::write_file(filename, *json));
  ASSERT_STREQ("[]", read_fileContent(filename).c_str());
  delete json;
}

TEST_F(TestWrite, ArrayWithValuesIsIndented)
{
  auto json = TinyJSON::TJ::parse("[1,2,3,4]");
  ASSERT_NE(nullptr, json);
  ASSERT_TRUE(TinyJSON::TJ::write_file(filename, *json));
  ASSERT_STREQ(R"([
  1,
  2,
  3,
  4
])", read_fileContent(filename).c_str());
  delete json;
}

TEST_F(TestWrite, ArrayWithValuesNotIndented)
{
  TinyJSON::write_options options = {};
  options.write_formating = TinyJSON::formating::minify;
  auto json = TinyJSON::TJ::parse("[1,2,3,4]");
  ASSERT_NE(nullptr, json);
  ASSERT_TRUE(TinyJSON::TJ::write_file(filename, *json, options));
  ASSERT_STREQ(R"([1,2,3,4])", read_fileContent(filename).c_str());
  delete json;
}

TEST_F(TestWrite, ArrayWithValuesNotIndentedWithUtf8Bom)
{
  TinyJSON::write_options options = {};
  options.write_formating = TinyJSON::formating::minify;
  options.byte_order_mark = TinyJSON::write_options::utf8;
  auto json = TinyJSON::TJ::parse("[1,2,3,4]");
  ASSERT_NE(nullptr, json);
  ASSERT_TRUE(TinyJSON::TJ::write_file(filename, *json, options));

  // make sure we have the BOM
  ASSERT_TRUE(has_Utf8BOM(filename));

  // then try and parse it again.
  TinyJSON::parse_options options_parse = {};
  options_parse.throw_exception = true;
  auto parse = TinyJSON::TJ::parse_file(filename, options_parse);
  ASSERT_NE(nullptr, parse);
  ASSERT_STREQ(R"([1,2,3,4])", parse->dump(TinyJSON::formating::minify));
  delete json;
  delete parse;
}

TEST_F(TestWrite, ObjectWithValuesNotIndentedWithUtf8Bom)
{
  TinyJSON::write_options options = {};
  options.write_formating = TinyJSON::formating::minify;
  options.byte_order_mark = TinyJSON::write_options::utf8;
  auto json = TinyJSON::TJ::parse(R"({"a":12, "b" : {}})");
  ASSERT_NE(nullptr, json);
  ASSERT_TRUE(TinyJSON::TJ::write_file(filename, *json, options));

  // make sure we have the BOM
  ASSERT_TRUE(has_Utf8BOM(filename));

  // then try and parse it again.
  TinyJSON::parse_options options_parse = {};
  options_parse.throw_exception = true;
  auto parse = TinyJSON::TJ::parse_file(filename, options_parse);
  ASSERT_NE(nullptr, parse);
  ASSERT_STREQ(R"({"a":12,"b":{}})", parse->dump(TinyJSON::formating::minify));
  delete json;
  delete parse;
}
