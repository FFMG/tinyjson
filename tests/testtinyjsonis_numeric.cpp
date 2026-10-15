// Licensed to Florent Guelfucci under one or more agreements.
// Florent Guelfucci licenses this file to you under the MIT license.
// See the LICENSE file in the project root for more information.
#include <gtest/gtest.h>
#define TJ_USE_CHAR 1
#include "../src/TinyJSON.h"

using namespace TinyJSON;

TEST(TestIsNumeric, NumericValue) {
  auto* num = TJ::parse("123");
  ASSERT_NE(nullptr, num);
  EXPECT_TRUE(num->is_numeric(false));
  EXPECT_TRUE(num->is_numeric(true));
  delete num;
}

TEST(TestIsNumeric, NumericString) {
  auto* num_str = TJ::parse("\"123\"");
  ASSERT_NE(nullptr, num_str);
  EXPECT_TRUE(num_str->is_numeric(false));
  EXPECT_FALSE(num_str->is_numeric(true));
  delete num_str;
}

TEST(TestIsNumeric, FloatNumericString) {
  auto* float_str = TJ::parse("\"12.34\"");
  ASSERT_NE(nullptr, float_str);
  EXPECT_TRUE(float_str->is_numeric(false));
  EXPECT_FALSE(float_str->is_numeric(true));
  delete float_str;
}

TEST(TestIsNumeric, ScientificNumericString) {
  auto* sci_str = TJ::parse("\"-0.5e10\"");
  ASSERT_NE(nullptr, sci_str);
  EXPECT_TRUE(sci_str->is_numeric(false));
  EXPECT_FALSE(sci_str->is_numeric(true));
  delete sci_str;
}

TEST(TestIsNumeric, NonNumericString) {
  auto* alpha_str = TJ::parse("\"abc\"");
  ASSERT_NE(nullptr, alpha_str);
  EXPECT_FALSE(alpha_str->is_numeric(false));
  EXPECT_FALSE(alpha_str->is_numeric(true));
  delete alpha_str;
}

TEST(TestIsNumeric, BooleanValue) {
  auto* boolean = TJ::parse("true");
  ASSERT_NE(nullptr, boolean);
  EXPECT_FALSE(boolean->is_numeric(false));
  EXPECT_FALSE(boolean->is_numeric(true));
  delete boolean;
}

TEST(TestIsNumeric, NullValue) {
  auto* nul = TJ::parse("null");
  ASSERT_NE(nullptr, nul);
  EXPECT_FALSE(nul->is_numeric(false));
  EXPECT_FALSE(nul->is_numeric(true));
  delete nul;
}

TEST(TestIsNumeric, ObjectValue) {
  auto* obj = TJ::parse("{}");
  ASSERT_NE(nullptr, obj);
  EXPECT_FALSE(obj->is_numeric(false));
  EXPECT_FALSE(obj->is_numeric(true));
  delete obj;
}

TEST(TestIsNumeric, ArrayValue) {
  auto* arr = TJ::parse("[]");
  ASSERT_NE(nullptr, arr);
  EXPECT_FALSE(arr->is_numeric(false));
  EXPECT_FALSE(arr->is_numeric(true));
  delete arr;
}
