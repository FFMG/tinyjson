// Licensed to Florent Guelfucci under one or more agreements.
// Florent Guelfucci licenses this file to you under the MIT license.
// See the LICENSE file in the project root for more information.
#include <gtest/gtest.h>
#ifndef TJ_INCLUDE_STD_STRING
#define TJ_INCLUDE_STD_STRING 1
#endif // !TJ_INCLUDE_STD_STRING
#include "../src/TinyJSON.h"

#include <filesystem>
#include <fstream>
#include <regex>
#include <map>
#include <iostream>
#include <string>
#include <random>
#include <chrono>

std::string generateRandomString(size_t length) {
  const std::string characters = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
  std::random_device rd;
  std::mt19937 generator(rd());
  std::uniform_int_distribution<> distribution(0, characters.size() - 1);

  std::string randomString;
  for (size_t i = 0; i < length; ++i) {
    randomString += characters[distribution(generator)];
  }
  return randomString;
}

int generateRandomNumber(int min, int max) {
  std::random_device rd; // Seed for the random number generator
  std::mt19937 generator(rd()); // Mersenne Twister random number generator
  std::uniform_int_distribution<> distribution(min, max); // Uniform distribution within [min, max]

  return distribution(generator);
}

bool matches_file(const std::filesystem::directory_entry& file, const std::string& pattern) 
{
  const auto& filename = std::filesystem::path(file.path()).filename().string();

  // Convert the pattern to a regex string
  std::string regexPattern = std::regex_replace(pattern, std::regex("\\*"), ".*");

  // Create a regex object
  std::regex regex(regexPattern);

  // Test if the filename matches the pattern
  return std::regex_match(filename, regex);
}

TEST(JSONchecker, AllFiles)
{
  const std::filesystem::path path{ "../../../../tests/data/JSON_checker/" };
  for (const auto& file : std::filesystem::directory_iterator{ path })
  {
    if (file.is_directory())
    {
      continue;
    }
    if(std::filesystem::path(file.path()).extension() != ".json")
    {
      continue;
    }

    bool is_fail = false;
    if (matches_file(file, "fail*"))
    {
      is_fail = true;
    }
    else if (matches_file(file, "pass*"))
    {
      is_fail = false;
    }
    else
    {
      continue;
    }

    TinyJSON::parse_options options;
    options.max_depth = 20;   //  for the one test case
    options.specification = TinyJSON::parse_options::rfc4627;   //  for the other test case
    if (is_fail)
    {
      // we expect a failure here
      options.throw_exception = false;
    }
    else
    {
      options.throw_exception = true;
    }
    
    const auto& filename = file.path().string();
    try
    {
      auto json = TinyJSON::TJ::parse_file(filename.c_str(), options);
      if (is_fail && json != nullptr)
      {
        EXPECT_TRUE(false) << "Expected Fail: " << std::filesystem::path(file.path()).filename();
      }
      delete json;
    }
    catch(TinyJSON::TJParseException ex)
    {
      if (!is_fail)
      {
        EXPECT_TRUE(false) << "Expected Pass: " << std::filesystem::path(file.path()).filename() << "\nException: " << ex.what();
      }
    }
  }
}

TEST(JSONchecker, LargeShallowObjectCheck)
{
  // Get the start time
  auto start1 = std::chrono::high_resolution_clock::now();

  // create an empty object and add some items to it.
  auto object = new TinyJSON::TJValueObject();

  // then add a lot of items
  std::map<std::string, int> data;
  const int numbers_to_add = 10000;
  for (auto i = 0; i < numbers_to_add; ++i)
  {
    auto key = generateRandomString(10);  //  long string to prevent colisions.
    auto value = generateRandomNumber(0, 5000);
    object->set(key.c_str(), value);
    data[ key ] = value;
  }

  // we don't check for file size in case of colisions.
  
  auto end1 = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> duration1 = end1 - start1;
  GTEST_LOG_(INFO) << "Insert: " << duration1.count() << " seconds";

  auto start2 = std::chrono::high_resolution_clock::now();

  // then search each and every item
  for (auto d : data)
  {
    auto key = d.first;
    auto value = d.second;
    auto tj = object->try_get_value(key.c_str());
    ASSERT_NE(nullptr, tj);
    auto number = dynamic_cast<const TinyJSON::TJValueNumberInt*>(tj);
    ASSERT_NE(nullptr, number);
    ASSERT_EQ(value, number->get_number());
  }

  auto end2 = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> duration2 = end2 - start2;
  GTEST_LOG_(INFO) << "Search: " << duration2.count() << " seconds";

  delete object;
}

bool object_shallow()
{
  // create an empty object and add some items to it.
  auto object = new TinyJSON::TJValueObject();

  // then add a lot of items
  std::map<std::string, int> data;
  const int numbers_to_add = 10;
  for (auto i = 0; i < numbers_to_add; ++i)
  {
    auto key = generateRandomString(20);  //  long string to prevent colisions.
    auto value = generateRandomNumber(0, 5000);
    object->set(key.c_str(), value);
    data[key] = value;
  }

  // then search each and every item
  for (auto d : data)
  {
    auto key = d.first;
    auto value = d.second;
    auto tj = object->try_get_value(key.c_str());
    auto number = dynamic_cast<const TinyJSON::TJValueNumberInt*>(tj);
    if (number == nullptr)
    {
      std::cout << "  Error! Values was not found!\n";
      return false;
    }
    if (number->get_number() != value)
    {
      std::cout << "  Error! Values mismatch\n";
      return false;
    }
  }
  delete object;
  return true;
}

TEST(JSONchecker, object_shallow)
{
  ASSERT_TRUE(object_shallow());
}

TEST(JSONchecker, CaseSensitiveCaseEdgeCases)
{
  auto object = new TinyJSON::TJValueObject();
  object->set("a1", 1);
  object->set("b2", 2);
  object->set("c3", 3);
  object->set("A4", 4);

  const TinyJSON::TJValueNumberInt* a = nullptr;
  a = dynamic_cast<const TinyJSON::TJValueNumberInt *>(object->try_get_value("a1", false)); //  case is correct
  ASSERT_NE(nullptr, a);
  ASSERT_EQ(1, a->get_number());

  // case sensitive search, (default)
  a = dynamic_cast<const TinyJSON::TJValueNumberInt *>(object->try_get_value("a1", true)); //  case is correct
  ASSERT_NE(nullptr, a);
  ASSERT_EQ(1, a->get_number());
  a = dynamic_cast<const TinyJSON::TJValueNumberInt *>(object->try_get_value("A1", true)); // case is wrong 
  ASSERT_EQ(nullptr, a);
  a = dynamic_cast<const TinyJSON::TJValueNumberInt *>(object->try_get_value("A1", false)); // case is wrong .... but we don't care
  ASSERT_NE(nullptr, a);
  ASSERT_EQ(1, a->get_number());

  a = dynamic_cast<const TinyJSON::TJValueNumberInt *>(object->try_get_value("b2", true));
  ASSERT_NE(nullptr, a);
  ASSERT_EQ(2, a->get_number());
  a = dynamic_cast<const TinyJSON::TJValueNumberInt *>(object->try_get_value("c3", true));
  ASSERT_NE(nullptr, a);
  ASSERT_EQ(3, a->get_number());

  a = dynamic_cast<const TinyJSON::TJValueNumberInt *>(object->try_get_value("A4", true));  // case is good
  ASSERT_NE(nullptr, a);
  ASSERT_EQ(4, a->get_number());
  a = dynamic_cast<const TinyJSON::TJValueNumberInt *>(object->try_get_value("A4", false)); // case is good ... but we don't care
  ASSERT_NE(nullptr, a);
  ASSERT_EQ(4, a->get_number());
  a = dynamic_cast<const TinyJSON::TJValueNumberInt *>(object->try_get_value("a4", false)); // case is wrong ... but we don't care
  ASSERT_NE(nullptr, a);
  ASSERT_EQ(4, a->get_number());

  delete object;
}
