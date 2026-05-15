// Licensed to Florent Guelfucci under one or more agreements.
// Florent Guelfucci licenses this file to you under the MIT license.
// See the LICENSE file in the project root for more information.
#include <gtest/gtest.h>
#include "../src/TinyJSON.h"
#include "testshelper.h"

using namespace TinyJSON;

// These tests are inspired by the official JSON5 test suite:
// https://github.com/json5/json5-tests/tree/master/objects

TEST(TJJSON5Objects, UnquotedKeys)
{
  const TJCHAR* json = TJCHARPREFIX("{hello: \"world\", _: 1, $: 2, a1: 3}");
  parse_options options;
  options.specification = parse_options::json5_1_0_0;
  auto* tj = TJ::parse(json, options);
  ASSERT_NE(nullptr, tj);
  auto* jobj = dynamic_cast<TJValueObject*>(tj);
  ASSERT_NE(nullptr, jobj);
  EXPECT_STREQ(TJCHARPREFIX("world"), jobj->get_string(TJCHARPREFIX("hello")));
  EXPECT_EQ(1, jobj->get_number<int>(TJCHARPREFIX("_")));
  EXPECT_EQ(2, jobj->get_number<int>(TJCHARPREFIX("$")));
  EXPECT_EQ(3, jobj->get_number<int>(TJCHARPREFIX("a1")));
  delete tj;
}

TEST(TJJSON5Objects, ReservedWordKeys)
{
  const TJCHAR* json = TJCHARPREFIX("{while: true, function: 1}");
  parse_options options;
  options.specification = parse_options::json5_1_0_0;
  auto* tj = TJ::parse(json, options);
  ASSERT_NE(nullptr, tj);
  auto* jobj = dynamic_cast<TJValueObject*>(tj);
  ASSERT_NE(nullptr, jobj);
  EXPECT_TRUE(jobj->get_boolean(TJCHARPREFIX("while")));
  EXPECT_EQ(1, jobj->get_number<int>(TJCHARPREFIX("function")));
  delete tj;
}

TEST(TJJSON5Objects, SingleQuotedKeys)
{
  const TJCHAR* json = TJCHARPREFIX("{'key': \"value\"}");
  parse_options options;
  options.specification = parse_options::json5_1_0_0;
  auto* tj = TJ::parse(json, options);
  ASSERT_NE(nullptr, tj);
  auto* jobj = dynamic_cast<TJValueObject*>(tj);
  ASSERT_NE(nullptr, jobj);
  EXPECT_STREQ(TJCHARPREFIX("value"), jobj->get_string(TJCHARPREFIX("key")));
  delete tj;
}

TEST(TJJSON5Objects, TrailingComma)
{
  const TJCHAR* json = TJCHARPREFIX("{\"a\": 1,}");
  parse_options options;
  options.specification = parse_options::json5_1_0_0;
  auto* tj = TJ::parse(json, options);
  ASSERT_NE(nullptr, tj);
  auto* jobj = dynamic_cast<TJValueObject*>(tj);
  ASSERT_NE(nullptr, jobj);
  ASSERT_EQ(1u, jobj->get_number_of_items());
  delete tj;
}

TEST(TJJSON5Objects, IllegalUnquotedKeyStartRejected)
{
  const TJCHAR* json = TJCHARPREFIX("{1a: 1}");
  parse_options options;
  options.specification = parse_options::json5_1_0_0;
  auto* tj = TJ::parse(json, options);
  ASSERT_EQ(nullptr, tj);
}
