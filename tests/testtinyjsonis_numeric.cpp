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

static bool is_numeric_string(const char* json, bool strict = false)
{
  auto* value = TJ::parse(json);
  EXPECT_NE(nullptr, value) << json;
  if (nullptr == value)
  {
    return false;
  }
  EXPECT_TRUE(value->is_string()) << json;
  auto result = value->is_numeric(strict);
  delete value;
  return result;
}

TEST(TestIsNumeric, StringIntegers)
{
  EXPECT_TRUE(is_numeric_string("\"0\""));
  EXPECT_TRUE(is_numeric_string("\"-0\""));
  EXPECT_TRUE(is_numeric_string("\"+0\""));
  EXPECT_TRUE(is_numeric_string("\"+1\""));
  EXPECT_TRUE(is_numeric_string("\"-1\""));
  EXPECT_TRUE(is_numeric_string("\"007\""));
  EXPECT_TRUE(is_numeric_string("\"9876543210\""));
  EXPECT_TRUE(is_numeric_string("\"123456789012345678901234567890\""));
}

TEST(TestIsNumeric, StringDecimals)
{
  EXPECT_TRUE(is_numeric_string("\"0.0\""));
  EXPECT_TRUE(is_numeric_string("\"-12.5\""));
  EXPECT_TRUE(is_numeric_string("\"+12.5\""));
  EXPECT_TRUE(is_numeric_string("\"1.\""));
  EXPECT_TRUE(is_numeric_string("\".5\""));
  EXPECT_TRUE(is_numeric_string("\"-.5\""));
  EXPECT_TRUE(is_numeric_string("\"+.5\""));
}

TEST(TestIsNumeric, StringExponents)
{
  EXPECT_TRUE(is_numeric_string("\"1e5\""));
  EXPECT_TRUE(is_numeric_string("\"1E5\""));
  EXPECT_TRUE(is_numeric_string("\"1e+5\""));
  EXPECT_TRUE(is_numeric_string("\"1e-5\""));
  EXPECT_TRUE(is_numeric_string("\"1E+05\""));
  EXPECT_TRUE(is_numeric_string("\"1.5e10\""));
  EXPECT_TRUE(is_numeric_string("\"1.e5\""));
  EXPECT_TRUE(is_numeric_string("\".5e5\""));
  EXPECT_TRUE(is_numeric_string("\"-0.5E-10\""));
}

TEST(TestIsNumeric, StringEmptyOrSignOnly)
{
  EXPECT_FALSE(is_numeric_string("\"\""));
  EXPECT_FALSE(is_numeric_string("\"+\""));
  EXPECT_FALSE(is_numeric_string("\"-\""));
  EXPECT_FALSE(is_numeric_string("\".\""));
  EXPECT_FALSE(is_numeric_string("\"-.\""));
  EXPECT_FALSE(is_numeric_string("\"+.\""));
}

TEST(TestIsNumeric, StringMultipleSigns)
{
  EXPECT_FALSE(is_numeric_string("\"--1\""));
  EXPECT_FALSE(is_numeric_string("\"++1\""));
  EXPECT_FALSE(is_numeric_string("\"+-1\""));
  EXPECT_FALSE(is_numeric_string("\"1-\""));
  EXPECT_FALSE(is_numeric_string("\"1+\""));
  EXPECT_FALSE(is_numeric_string("\"1-2\""));
  EXPECT_FALSE(is_numeric_string("\"1e+-5\""));
  EXPECT_FALSE(is_numeric_string("\"1e--5\""));
}

TEST(TestIsNumeric, StringMultipleDecimalPoints)
{
  EXPECT_FALSE(is_numeric_string("\"1.2.3\""));
  EXPECT_FALSE(is_numeric_string("\"1..2\""));
  EXPECT_FALSE(is_numeric_string("\"..1\""));
}

TEST(TestIsNumeric, StringInvalidExponents)
{
  // no mantissa
  EXPECT_FALSE(is_numeric_string("\"e5\""));
  EXPECT_FALSE(is_numeric_string("\"E5\""));
  EXPECT_FALSE(is_numeric_string("\".e5\""));
  EXPECT_FALSE(is_numeric_string("\"-e5\""));

  // no exponent digits
  EXPECT_FALSE(is_numeric_string("\"1e\""));
  EXPECT_FALSE(is_numeric_string("\"1E\""));
  EXPECT_FALSE(is_numeric_string("\"1e+\""));
  EXPECT_FALSE(is_numeric_string("\"1e-\""));
  EXPECT_FALSE(is_numeric_string("\"1.5e\""));

  // more than one exponent or a decimal exponent
  EXPECT_FALSE(is_numeric_string("\"1e5e5\""));
  EXPECT_FALSE(is_numeric_string("\"1e5E5\""));
  EXPECT_FALSE(is_numeric_string("\"1ee5\""));
  EXPECT_FALSE(is_numeric_string("\"1e5.5\""));
  EXPECT_FALSE(is_numeric_string("\"1e.5\""));
}

TEST(TestIsNumeric, StringWithWhitespace)
{
  EXPECT_FALSE(is_numeric_string("\" 1\""));
  EXPECT_FALSE(is_numeric_string("\"1 \""));
  EXPECT_FALSE(is_numeric_string("\"1 2\""));
  EXPECT_FALSE(is_numeric_string("\"- 1\""));
  EXPECT_FALSE(is_numeric_string("\"\\t1\""));
  EXPECT_FALSE(is_numeric_string("\"1\\n\""));
}

TEST(TestIsNumeric, StringOtherNumberFormats)
{
  EXPECT_FALSE(is_numeric_string("\"0x1A\""));
  EXPECT_FALSE(is_numeric_string("\"0b101\""));
  EXPECT_FALSE(is_numeric_string("\"NaN\""));
  EXPECT_FALSE(is_numeric_string("\"Infinity\""));
  EXPECT_FALSE(is_numeric_string("\"-Infinity\""));
  EXPECT_FALSE(is_numeric_string("\"1,000\""));
  EXPECT_FALSE(is_numeric_string("\"1,5\""));
  EXPECT_FALSE(is_numeric_string("\"1_000\""));
  EXPECT_FALSE(is_numeric_string("\"1f\""));
  EXPECT_FALSE(is_numeric_string("\"12a\""));
  EXPECT_FALSE(is_numeric_string("\"a12\""));
}

TEST(TestIsNumeric, StringNonAsciiCharacters)
{
  // U+0661 ARABIC-INDIC DIGIT ONE and U+FF11 FULLWIDTH DIGIT ONE are not ASCII digits.
  EXPECT_FALSE(is_numeric_string("\"\\u0661\""));
  EXPECT_FALSE(is_numeric_string("\"\\uFF11\""));

  // U+0131, (low byte 0x31 = '1'), must not be mistaken for a digit.
  EXPECT_FALSE(is_numeric_string("\"\\u0131\""));
  EXPECT_FALSE(is_numeric_string("\"1\\u0131\""));
}

TEST(TestIsNumeric, StringWithEscapedDigits)
{
  // "\u0031" is the escaped form of "1"
  EXPECT_TRUE(is_numeric_string("\"\\u0031\""));
  EXPECT_TRUE(is_numeric_string("\"\\u0031\\u0032.5\""));
}

TEST(TestIsNumeric, StringIsNeverNumericWhenStrict)
{
  EXPECT_FALSE(is_numeric_string("\"0\"", true));
  EXPECT_FALSE(is_numeric_string("\"-1.5e10\"", true));
  EXPECT_FALSE(is_numeric_string("\"\"", true));
  EXPECT_FALSE(is_numeric_string("\"abc\"", true));
}

TEST(TestIsNumeric, NumberValuesInsideContainers)
{
  auto* json = TJ::parse("{\"a\":\"1e\",\"b\":\"-2.5\",\"c\":3,\"d\":[\"4\",\"x\"]}");
  ASSERT_NE(nullptr, json);
  auto* object = dynamic_cast<TJValueObject*>(json);
  ASSERT_NE(nullptr, object);

  EXPECT_FALSE(object->try_get_value("a")->is_numeric());
  EXPECT_TRUE(object->try_get_value("b")->is_numeric());
  EXPECT_FALSE(object->try_get_value("b")->is_numeric(true));
  EXPECT_TRUE(object->try_get_value("c")->is_numeric());
  EXPECT_TRUE(object->try_get_value("c")->is_numeric(true));

  auto* array = dynamic_cast<const TJValueArray*>(object->try_get_value("d"));
  ASSERT_NE(nullptr, array);
  EXPECT_TRUE(array->at(0)->is_numeric());
  EXPECT_FALSE(array->at(1)->is_numeric());
  delete json;
}
