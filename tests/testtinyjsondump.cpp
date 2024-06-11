// Licensed to Florent Guelfucci under one or more agreements.
// Florent Guelfucci licenses this file to you under the MIT license.
// See the LICENSE file in the project root for more information.
#include <gtest/gtest.h>
#include "../src/TinyJSON.h"

TEST(TestDump, EmptyArray) {
  auto json = TinyJSON::TinyJSON::parse("[]");
  ASSERT_NE(nullptr, json);

  const auto& text = json->dump(TinyJSON::formating::indented);
  ASSERT_NE(nullptr, text);

  ASSERT_STREQ("[]", text);

  delete json;
}

TEST(TestDump, EmptyArrayNoIndent) {
  auto json = TinyJSON::TinyJSON::parse("[]");
  ASSERT_NE(nullptr, json);

  const auto& text = json->dump(TinyJSON::formating::none);
  ASSERT_NE(nullptr, text);

  ASSERT_STREQ("[]", text);

  delete json;
}

TEST(TestDump, EmptyArrayOfNumbersNoIndent) {
  auto json = TinyJSON::TinyJSON::parse("[12,13,14]");
  ASSERT_NE(nullptr, json);

  const auto& text = json->dump(TinyJSON::formating::none);
  ASSERT_NE(nullptr, text);

  ASSERT_STREQ("[12,13,14]", text);
  delete json;
}

TEST(TestDump, EmptyArrayOfNumbers) {
  auto json = TinyJSON::TinyJSON::parse("[12,13,14]");
  ASSERT_NE(nullptr, json);

  const auto& text = json->dump(TinyJSON::formating::indented);
  ASSERT_NE(nullptr, text);

  ASSERT_STREQ(R"([
  12,
  13,
  14
])", text);

  delete json;
}

TEST(TestDump, AnIntNumberByItSelf) {
  std::vector<std::string> values;
  values.push_back("12");
  values.push_back("42");
  values.push_back("0");
  values.push_back("-42");
  values.push_back("-1");
  for (auto& value : values)
  {
    auto json = TinyJSON::TinyJSON::parse(value.c_str());
    ASSERT_NE(nullptr, json);

    const auto& text = json->dump(TinyJSON::formating::indented);
    ASSERT_NE(nullptr, text);

    ASSERT_STREQ(value.c_str(), text);
    delete json;
  }
}

TEST(TestDump, AnIntNumberByItSelf2) {
  std::vector<std::string> values;
  values.push_back("12");
  values.push_back("42");
  values.push_back("0");
  values.push_back("-42");
  values.push_back("-1");
  for (auto& value : values)
  {
    auto json = new TinyJSON::TJValueNumberInt(std::atoll(value.c_str()));
    const auto& text = json->dump(TinyJSON::formating::indented);
    ASSERT_NE(nullptr, text);

    ASSERT_STREQ(value.c_str(), text);
    delete json;
  }
}

TEST(TestDump, EmptyArrayOfVariousValues) {
  auto json = TinyJSON::TinyJSON::parse("[ 12,true,null, false ]");
  ASSERT_NE(nullptr, json);

  const auto& text = json->dump(TinyJSON::formating::indented);
  ASSERT_NE(nullptr, text);

  ASSERT_STREQ(R"([
  12,
  true,
  null,
  false
])", text);

  delete json;
}

TEST(TestDump, EmptyArrayOfVariousNotIndented) {
  auto json = TinyJSON::TinyJSON::parse(R"([
  12,
  true,
  null,
  false
])");
  ASSERT_NE(nullptr, json);

  const auto& text = json->dump(TinyJSON::formating::none);
  ASSERT_NE(nullptr, text);

  ASSERT_STREQ("[12,true,null,false]", text);

  delete json;
}

TEST(TestDump, BooleanByItSelf) {
  std::vector<std::string> values;
  values.push_back("true");
  values.push_back("false");
  for (auto& value : values)
  {
    auto json = TinyJSON::TinyJSON::parse(value.c_str());
    ASSERT_NE(nullptr, json);

    const auto& text = json->dump(TinyJSON::formating::indented);
    ASSERT_NE(nullptr, text);

    ASSERT_STREQ(value.c_str(), text);
    delete json;
  }
}
