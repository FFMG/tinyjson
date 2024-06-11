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
