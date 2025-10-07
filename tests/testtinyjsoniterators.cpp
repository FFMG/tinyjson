// Licensed to Florent Guelfucci under one or more agreements.
// Florent Guelfucci licenses this file to you under the MIT license.
// See the LICENSE file in the project root for more information.
#include <gtest/gtest.h>
#define TJ_USE_CHAR 1
#include "../src/TinyJSON.h"
#include <vector>

TEST(TJValueIterators, EmptyArray) 
{
  auto json = TinyJSON::TJ::parse("[]");
  ASSERT_NE(nullptr, json);

  for (auto& a : *json)
  {
    FAIL() << "This path should not be called as there is no data in the array!";
  }
  delete json;
}

TEST(TJValueIterators, EmptyObject)
{
  auto json = TinyJSON::TJ::parse("{}");
  ASSERT_NE(nullptr, json);

  for (auto& a : *json)
  {
    FAIL() << "This path should not be called as there is no data in the object!";
  }
  delete json;
}

TEST(TJValueIterators, ConstEmptyArray)
{
  auto json = TinyJSON::TJ::parse("[]");
  ASSERT_NE(nullptr, json);

  for (auto& a : *json)
  {
    FAIL() << "This path should not be called as there is no data in the array!";
  }
  delete json;
}

TEST(TJValueIterators, ConstEmptyObject)
{
  auto json = TinyJSON::TJ::parse("{}");
  ASSERT_NE(nullptr, json);

  for (auto& a : *json)
  {
    FAIL() << "This path should not be called as there is no data in the object!";
  }
  delete json;
}

TEST(TJValueIterators, IterateArray)
{
  auto json = TinyJSON::TJ::parse("[1,2,3]");
  ASSERT_NE(nullptr, json);

  std::vector<int> check = { 1,2,3 };
  int index = 0;

  for (auto& a : *json)
  {
    ASSERT_EQ(check[index++], a.get_number<int>());
  }
  delete json;
}

TEST(TJValueIterators, ConstIterateArray)
{
  auto json = TinyJSON::TJ::parse("[1,2,3]");
  ASSERT_NE(nullptr, json);

  std::vector<int> check = { 1,2,3 };
  int index = 0;

  for (auto& a : *json)
  {
    ASSERT_EQ(check[index++], a.get_number<int>());
  }
  ASSERT_EQ(check.size(), index);
  delete json;
}

TEST(TJValueIterators, IterateObject)
{
  auto json = TinyJSON::TJ::parse("{\"a\": 1,\"b\":2,\"c\":3}");
  ASSERT_NE(nullptr, json);

  std::vector<int> check = { 1,2,3 };
  int index = 0;

  for (auto& a : *json)
  {
    ASSERT_EQ(check[index++], a.get_number<int>());
  }
  ASSERT_EQ(check.size(), index);
  delete json;
}

TEST(TJValueIterators, IterateObjectWithRepeatValues)
{
  // 'a' is repeated.
  auto json = TinyJSON::TJ::parse("{\"a\": 1,\"a\":2,\"a\":3}");
  ASSERT_NE(nullptr, json);

  std::vector<int> check = { 3 };
  int index = 0;

  for (auto& a : *json)
  {
    ASSERT_EQ(check[index++], a.get_number<int>());
  }
  ASSERT_EQ(check.size(), index);
  delete json;
}

TEST(TJValueIterators, ConstIterateObject)
{
  auto json = TinyJSON::TJ::parse("{\"a\": 1,\"b\":2,\"c\":3}");
  ASSERT_NE(nullptr, json);

  std::vector<int> check = { 1,2,3 };
  int index = 0;

  for (const auto& a : *json)
  {
    ASSERT_EQ(check[index++], a.get_number<int>());
  }
  ASSERT_EQ(check.size(), index);
  delete json;
}

TEST(TJValueIterators, IterateANumber)
{
  auto json = TinyJSON::TJ::parse("1");
  ASSERT_NE(nullptr, json);
  int index = 0;

  for (auto& a : *json)
  {
    ASSERT_EQ(1, a.get_number<int>());
    ++index;
  }
  ASSERT_EQ(1, index);
  delete json;
}

TEST(TJValueIterators, IterateAFloat)
{
  auto json = TinyJSON::TJ::parse("2.5");
  ASSERT_NE(nullptr, json);
  int index = 0;

  for (auto& a : *json)
  {
    ASSERT_EQ(2.5, a.get_float<double>());
    ++index;
  }
  ASSERT_EQ(1, index);
  delete json;
}

TEST(TJValueIterators, IterateAString)
{
  auto json = TinyJSON::TJ::parse("\"Hello\"");
  ASSERT_NE(nullptr, json);
  int index = 0;

  for (auto& a : *json)
  {
    ASSERT_STREQ("Hello", a.get_string());
    ++index;
  }
  ASSERT_EQ(1, index);
  delete json;
}
