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

TEST(TestDump, EmptyObject) {
  auto json = TinyJSON::TinyJSON::parse("{}");
  ASSERT_NE(nullptr, json);

  const auto& text = json->dump(TinyJSON::formating::indented);
  ASSERT_NE(nullptr, text);

  ASSERT_STREQ("{}", text);

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

TEST(TestDump, EmptyObjectNoIndent) {
  auto json = TinyJSON::TinyJSON::parse("{}");
  ASSERT_NE(nullptr, json);

  const auto& text = json->dump(TinyJSON::formating::none);
  ASSERT_NE(nullptr, text);

  ASSERT_STREQ("{}", text);

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

TEST(TestDump, SimpleObjectWithNumbersNoIntent) {
  auto json = TinyJSON::TinyJSON::parse(R"({
  "a"  : 12,
  "b" : 14
})");
  ASSERT_NE(nullptr, json);

  const auto& text = json->dump(TinyJSON::formating::none);
  ASSERT_NE(nullptr, text);

  ASSERT_STREQ(R"({"a":12,"b":14})", text);
  delete json;
}

TEST(TestDump, SimpleObjectWithNumbers) {
  auto json = TinyJSON::TinyJSON::parse(R"({
  "a"  : 12,
  "b" : 14
})");
  ASSERT_NE(nullptr, json);

  const auto& text = json->dump(TinyJSON::formating::indented);
  ASSERT_NE(nullptr, text);

  ASSERT_STREQ(R"({
  "a": 12,
  "b": 14
})", text);
  delete json;
}

TEST(TestDump, ObjectInObjectWithNumbers) {
  auto json = TinyJSON::TinyJSON::parse(R"({
  "a"  : 12,
  "b" : {
    "aa"  : 12,
    "bb"  : 12
  }
})");
  ASSERT_NE(nullptr, json);

  const auto& text = json->dump(TinyJSON::formating::indented);
  ASSERT_NE(nullptr, text);

  ASSERT_STREQ(R"({
  "a": 12,
  "b": {
    "aa": 12,
    "bb": 12
  }
})", text);
  delete json;
}

TEST(TestDump, ArrayInArrayWithNumbers) {
  auto json = TinyJSON::TinyJSON::parse(R"([
  12,
  [12,13]
])");
  ASSERT_NE(nullptr, json);

  const auto& text = json->dump(TinyJSON::formating::indented);
  ASSERT_NE(nullptr, text);

  ASSERT_STREQ(R"([
  12,
  [
    12,
    13
  ]
])", text);
  delete json;
}

TEST(TestDump, ThreeDeepArrayWithNumbersAndString) {
  auto json = TinyJSON::TinyJSON::parse(R"([
  12,
  [13,
  [14,15,"Hello"]
  ],
  "World",3.1416
])");
  ASSERT_NE(nullptr, json);

  const auto& text = json->dump(TinyJSON::formating::indented);
  ASSERT_NE(nullptr, text);

  ASSERT_STREQ(R"([
  12,
  [
    13,
    [
      14,
      15,
      "Hello"
    ]
  ],
  "World",
  3.1416
])", text);
  delete json;
}

TEST(TestDump, ThreeDeepArrayWithNumbersAndStringNotIndented) {
  auto json = TinyJSON::TinyJSON::parse(R"([
  12,
  [13,
  [14,15,"Hello"]
  ],
  "World",3.1416
])");
  ASSERT_NE(nullptr, json);

  const auto& text = json->dump(TinyJSON::formating::none);
  ASSERT_NE(nullptr, text);

  ASSERT_STREQ(R"([12,[13,[14,15,"Hello"]],"World",3.1416])", text);
  delete json;
}

TEST(TestDump, ThreeDeepObjectWithNumbersAndString) {
  auto json = TinyJSON::TinyJSON::parse(R"({
  "a":12,
  "b":{"aa":13,
  "bb":{"aaa":14,"bbb":15,"ccc": "Hello"}
  },"c": "World"
})");
  ASSERT_NE(nullptr, json);

  const auto& text = json->dump(TinyJSON::formating::indented);
  ASSERT_NE(nullptr, text);

  ASSERT_STREQ(R"({
  "a": 12,
  "b": {
    "aa": 13,
    "bb": {
      "aaa": 14,
      "bbb": 15,
      "ccc": "Hello"
    }
  },
  "c": "World"
})", text);
  delete json;
}

TEST(TestDump, StringByItSelf) {
  std::vector<std::string> values;
  values.push_back(R"("")");
  values.push_back(R"("Hello")");
  values.push_back(R"("Hello With Spaces")");
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

TEST(TestDump, AFloatNumberByItSelf) {
  std::vector<std::string> values;
  values.push_back("3.1415926535897932384");
  values.push_back("1.00001");
  values.push_back("3.0141592653589793238");
  values.push_back("12.34");
  values.push_back("42.1254");
  values.push_back("0.1234");
  values.push_back("-42.789");
  values.push_back("-0.1234");
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
