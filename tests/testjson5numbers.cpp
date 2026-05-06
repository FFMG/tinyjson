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

TEST(TJJSON5Numbers, PlusSign)
{
  parse_options options;
  options.specification = parse_options::json5_1_0_0;

  const TJCHAR* json1 = TJCHARPREFIX("+42");
  auto* tj1 = TJ::parse(json1, options);
  ASSERT_NE(nullptr, tj1);
  EXPECT_EQ(42, tj1->get_number<int>());
  delete tj1;

  const TJCHAR* json2 = TJCHARPREFIX("+0.5");
  auto* tj2 = TJ::parse(json2, options);
  ASSERT_NE(nullptr, tj2);
  EXPECT_DOUBLE_EQ(0.5, tj2->get_float<double>());
  delete tj2;

  const TJCHAR* json3 = TJCHARPREFIX("+.5");
  auto* tj3 = TJ::parse(json3, options);
  ASSERT_NE(nullptr, tj3);
  EXPECT_DOUBLE_EQ(0.5, tj3->get_float<double>());
  delete tj3;
}

TEST(TJJSON5Numbers, LeadingTrailingDecimal)
{
  parse_options options;
  options.specification = parse_options::json5_1_0_0;

  const TJCHAR* json1 = TJCHARPREFIX(".5");
  auto* tj1 = TJ::parse(json1, options);
  ASSERT_NE(nullptr, tj1);
  EXPECT_DOUBLE_EQ(0.5, tj1->get_float<double>());
  delete tj1;

  const TJCHAR* json2 = TJCHARPREFIX("5.");
  auto* tj2 = TJ::parse(json2, options);
  ASSERT_NE(nullptr, tj2);
  EXPECT_DOUBLE_EQ(5.0, tj2->get_float<double>());
  delete tj2;
}

TEST(TJJSON5Numbers, Hexadecimal)
{
  parse_options options;
  options.specification = parse_options::json5_1_0_0;

  const TJCHAR* json1 = TJCHARPREFIX("0x42");
  auto* tj1 = TJ::parse(json1, options);
  ASSERT_NE(nullptr, tj1);
  EXPECT_EQ(0x42, tj1->get_number<int>());
  delete tj1;

  const TJCHAR* json2 = TJCHARPREFIX("0XFF");
  auto* tj2 = TJ::parse(json2, options);
  ASSERT_NE(nullptr, tj2);
  EXPECT_EQ(255, tj2->get_number<int>());
  delete tj2;

  const TJCHAR* json3 = TJCHARPREFIX("+0x1");
  auto* tj3 = TJ::parse(json3, options);
  ASSERT_NE(nullptr, tj3);
  EXPECT_EQ(1, tj3->get_number<int>());
  delete tj3;

  const TJCHAR* json4 = TJCHARPREFIX("-0x1");
  auto* tj4 = TJ::parse(json4, options);
  ASSERT_NE(nullptr, tj4);
  EXPECT_EQ(-1, tj4->get_number<int>());
  delete tj4;
}

TEST(TJJSON5Numbers, SpecialValues)
{
  parse_options options;
  options.specification = parse_options::json5_1_0_0;

  const TJCHAR* json1 = TJCHARPREFIX("NaN");
  auto* tj1 = TJ::parse(json1, options);
  ASSERT_NE(nullptr, tj1);
  EXPECT_TRUE(std::isnan(static_cast<double>(tj1->get_float<double>())));
  delete tj1;

  const TJCHAR* json2 = TJCHARPREFIX("Infinity");
  auto* tj2 = TJ::parse(json2, options);
  ASSERT_NE(nullptr, tj2);
  EXPECT_TRUE(std::isinf(static_cast<double>(tj2->get_float<double>())));
  EXPECT_GT(tj2->get_float<double>(), 0);
  delete tj2;

  const TJCHAR* json3 = TJCHARPREFIX("-Infinity");
  auto* tj3 = TJ::parse(json3, options);
  ASSERT_NE(nullptr, tj3);
  EXPECT_TRUE(std::isinf(static_cast<double>(tj3->get_float<double>())));
  EXPECT_LT(tj3->get_float<double>(), 0);
  delete tj3;

  const TJCHAR* json4 = TJCHARPREFIX("+Infinity");
  auto* tj4 = TJ::parse(json4, options);
  ASSERT_NE(nullptr, tj4);
  EXPECT_TRUE(std::isinf(static_cast<double>(tj4->get_float<double>())));
  EXPECT_GT(tj4->get_float<double>(), 0);
  delete tj4;
}

TEST(TJJSON5Numbers, Exponents)
{
  parse_options options;
  options.specification = parse_options::json5_1_0_0;

  const TJCHAR* json1 = TJCHARPREFIX("1e+2");
  auto* tj1 = TJ::parse(json1, options);
  ASSERT_NE(nullptr, tj1);
  EXPECT_DOUBLE_EQ(100.0, tj1->get_float<double>());
  delete tj1;

  const TJCHAR* json2 = TJCHARPREFIX(".5e-1");
  auto* tj2 = TJ::parse(json2, options);
  ASSERT_NE(nullptr, tj2);
  EXPECT_DOUBLE_EQ(0.05, tj2->get_float<double>());
  delete tj2;

  const TJCHAR* json3 = TJCHARPREFIX("5.e1");
  auto* tj3 = TJ::parse(json3, options);
  ASSERT_NE(nullptr, tj3);
  EXPECT_DOUBLE_EQ(50.0, tj3->get_float<double>());
  delete tj3;
}
