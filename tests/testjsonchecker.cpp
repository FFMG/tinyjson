// Licensed to Florent Guelfucci under one or more agreements.
// Florent Guelfucci licenses this file to you under the MIT license.
// See the LICENSE file in the project root for more information.
#include <gtest/gtest.h>
#define TJ_USE_CHAR 1
#include "../src/TinyJSON.h"

#include <filesystem>
#include <fstream>
#include <regex>

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

    const auto& filename = file.path().string();
    auto json = TinyJSON::TinyJSON::parse_file(filename.c_str());
    if (is_fail)
    {
      EXPECT_EQ(nullptr, json);
      if (nullptr != json)
      {
        std::cout << "Expected Fail: " << std::filesystem::path(file.path()).filename() << '\n';
      }
    }
    else
    {
      EXPECT_NE(nullptr, json);
      if (nullptr == json)
      {
        std::cout << "Expected Pass: " << std::filesystem::path(file.path()).filename() << '\n';
      }
    }
    delete json;
  }
}