// Licensed to Florent Guelfucci under one or more agreements.
// Florent Guelfucci licenses this file to you under the MIT license.
// See the LICENSE file in the project root for more information.
#include <gtest/gtest.h>
#include "../src/TinyJSON.h"
#include "testshelper.h"
#include <cmath>

using namespace TinyJSON;

// These tests are inspired by the official JSON5 test suite:
// https://github.com/json5/json5-tests/tree/master/numbers

TEST(TJJSON5Numbers, PlusSignInteger)
{
  parse_options options;
  options.specification = parse_options::json5_1_0_0;

  const TJCHAR* json = TJCHARPREFIX("+42");
  auto* tj = TJ::parse(json, options);
  ASSERT_NE(nullptr, tj);
  EXPECT_EQ(42, tj->get_number<int>());
  delete tj;
}

TEST(TJJSON5Numbers, PlusSignFloat)
{
  parse_options options;
  options.specification = parse_options::json5_1_0_0;

  const TJCHAR* json = TJCHARPREFIX("+0.5");
  auto* tj = TJ::parse(json, options);
  ASSERT_NE(nullptr, tj);
  EXPECT_DOUBLE_EQ(0.5, tj->get_float<double>());
  delete tj;
}

TEST(TJJSON5Numbers, PlusSignFloatLeadingDecimal)
{
  parse_options options;
  options.specification = parse_options::json5_1_0_0;

  const TJCHAR* json = TJCHARPREFIX("+.5");
  auto* tj = TJ::parse(json, options);
  ASSERT_NE(nullptr, tj);
  EXPECT_DOUBLE_EQ(0.5, tj->get_float<double>());
  delete tj;
}

TEST(TJJSON5Numbers, LeadingDecimal)
{
  parse_options options;
  options.specification = parse_options::json5_1_0_0;

  const TJCHAR* json = TJCHARPREFIX(".5");
  auto* tj = TJ::parse(json, options);
  ASSERT_NE(nullptr, tj);
  EXPECT_DOUBLE_EQ(0.5, tj->get_float<double>());
  delete tj;
}

TEST(TJJSON5Numbers, TrailingDecimal)
{
  parse_options options;
  options.specification = parse_options::json5_1_0_0;

  const TJCHAR* json = TJCHARPREFIX("5.");
  auto* tj = TJ::parse(json, options);
  ASSERT_NE(nullptr, tj);
  EXPECT_DOUBLE_EQ(5.0, tj->get_float<double>());
  delete tj;
}

TEST(TJJSON5Numbers, HexadecimalLowerX)
{
  parse_options options;
  options.specification = parse_options::json5_1_0_0;

  const TJCHAR* json = TJCHARPREFIX("0x42");
  auto* tj = TJ::parse(json, options);
  ASSERT_NE(nullptr, tj);
  EXPECT_EQ(0x42, tj->get_number<int>());
  delete tj;
}

TEST(TJJSON5Numbers, HexadecimalUpperX)
{
  parse_options options;
  options.specification = parse_options::json5_1_0_0;

  const TJCHAR* json = TJCHARPREFIX("0XFF");
  auto* tj = TJ::parse(json, options);
  ASSERT_NE(nullptr, tj);
  EXPECT_EQ(255, tj->get_number<int>());
  delete tj;
}

TEST(TJJSON5Numbers, HexadecimalPlusSign)
{
  parse_options options;
  options.specification = parse_options::json5_1_0_0;

  const TJCHAR* json = TJCHARPREFIX("+0x1");
  auto* tj = TJ::parse(json, options);
  ASSERT_NE(nullptr, tj);
  EXPECT_EQ(1, tj->get_number<int>());
  delete tj;
}

TEST(TJJSON5Numbers, HexadecimalMinusSign)
{
  parse_options options;
  options.specification = parse_options::json5_1_0_0;

  const TJCHAR* json = TJCHARPREFIX("-0x1");
  auto* tj = TJ::parse(json, options);
  ASSERT_NE(nullptr, tj);
  EXPECT_EQ(-1, tj->get_number<int>());
  delete tj;
}

TEST(TJJSON5Numbers, NaN)
{
  parse_options options;
  options.specification = parse_options::json5_1_0_0;

  const TJCHAR* json = TJCHARPREFIX("NaN");
  auto* tj = TJ::parse(json, options);
  ASSERT_NE(nullptr, tj);
  EXPECT_TRUE(std::isnan(static_cast<double>(tj->get_float<double>())));
  delete tj;
}

TEST(TJJSON5Numbers, Infinity)
{
  parse_options options;
  options.specification = parse_options::json5_1_0_0;

  const TJCHAR* json = TJCHARPREFIX("Infinity");
  auto* tj = TJ::parse(json, options);
  ASSERT_NE(nullptr, tj);
  EXPECT_TRUE(std::isinf(static_cast<double>(tj->get_float<double>())));
  EXPECT_GT(tj->get_float<double>(), 0);
  delete tj;
}

TEST(TJJSON5Numbers, MinusInfinity)
{
  parse_options options;
  options.specification = parse_options::json5_1_0_0;

  const TJCHAR* json = TJCHARPREFIX("-Infinity");
  auto* tj = TJ::parse(json, options);
  ASSERT_NE(nullptr, tj);
  EXPECT_TRUE(std::isinf(static_cast<double>(tj->get_float<double>())));
  EXPECT_LT(tj->get_float<double>(), 0);
  delete tj;
}

TEST(TJJSON5Numbers, PlusInfinity)
{
  parse_options options;
  options.specification = parse_options::json5_1_0_0;

  const TJCHAR* json = TJCHARPREFIX("+Infinity");
  auto* tj = TJ::parse(json, options);
  ASSERT_NE(nullptr, tj);
  EXPECT_TRUE(std::isinf(static_cast<double>(tj->get_float<double>())));
  EXPECT_GT(tj->get_float<double>(), 0);
  delete tj;
}

TEST(TJJSON5Numbers, ExponentPlus)
{
  parse_options options;
  options.specification = parse_options::json5_1_0_0;

  const TJCHAR* json = TJCHARPREFIX("1e+2");
  auto* tj = TJ::parse(json, options);
  ASSERT_NE(nullptr, tj);
  EXPECT_DOUBLE_EQ(100.0, tj->get_float<double>());
  delete tj;
}

TEST(TJJSON5Numbers, ExponentMinusLeadingDecimal)
{
  parse_options options;
  options.specification = parse_options::json5_1_0_0;

  const TJCHAR* json = TJCHARPREFIX(".5e-1");
  auto* tj = TJ::parse(json, options);
  ASSERT_NE(nullptr, tj);
  EXPECT_DOUBLE_EQ(0.05, tj->get_float<double>());
  delete tj;
}

TEST(TJJSON5Numbers, ExponentTrailingDecimal)
{
  parse_options options;
  options.specification = parse_options::json5_1_0_0;

  const TJCHAR* json = TJCHARPREFIX("5.e1");
  auto* tj = TJ::parse(json, options);
  ASSERT_NE(nullptr, tj);
  EXPECT_DOUBLE_EQ(50.0, tj->get_float<double>());
  delete tj;
}
