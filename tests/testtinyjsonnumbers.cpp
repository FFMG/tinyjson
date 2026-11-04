// Licensed to Florent Guelfucci under one or more agreements.
// Florent Guelfucci licenses this file to you under the MIT license.
// See the LICENSE file in the project root for more information.
#include <gtest/gtest.h>
#include <vector>
#define TJ_USE_CHAR 1
#include "../src/TinyJSON.h"
#include <cmath>
#include <limits>
#include <string>

TEST(TestNumbers, NumberIsAfterMissingColon) {
  auto json = TinyJSON::TJ::parse(R"(
    {
      "a" 42
    }
    )"
  );
  ASSERT_EQ(nullptr, json);
}

TEST(TestNumbers, WholeNumbers) {
  auto json = TinyJSON::TJ::parse(R"(
{
  "a" : 12,
  "b" : -42,
  "c" : 42.00
}
)"
);
  ASSERT_NE(nullptr, json);
  auto jobject = dynamic_cast<TinyJSON::TJValueObject*>(json);
  ASSERT_NE(nullptr, jobject);

  auto valuea = dynamic_cast<const TinyJSON::TJValueNumberInt*>(jobject->try_get_value("a"));
  ASSERT_NE(nullptr, valuea);
  ASSERT_EQ(12, valuea->get_number());

  auto valueb = dynamic_cast<const TinyJSON::TJValueNumberInt*>(jobject->try_get_value("b"));
  ASSERT_NE(nullptr, valueb);
  ASSERT_EQ(-42, valueb->get_number());

  // c is still an integer even if it was given as a fraction
  auto valuec = dynamic_cast<const TinyJSON::TJValueNumberInt*>(jobject->try_get_value("c"));
  ASSERT_NE(nullptr, valuec);
  ASSERT_EQ(42, valuec->get_number());

  delete json;
}

TEST(TestNumbers, FractionsWithLeadingZeros) {
  auto json = TinyJSON::TJ::parse(R"(
{
  "a" : 1.0001,
  "b" : 0.00002
}
)"
);
  ASSERT_NE(nullptr, json);
  auto jobject = dynamic_cast<TinyJSON::TJValueObject*>(json);
  ASSERT_NE(nullptr, jobject);

  auto valuea = dynamic_cast<const TinyJSON::TJValueNumberFloat*>(jobject->try_get_value("a"));
  ASSERT_NE(nullptr, valuea);
  ASSERT_DOUBLE_EQ(1.0001, valuea->get_number());

  auto valueb = dynamic_cast<const TinyJSON::TJValueNumberFloat*>(jobject->try_get_value("b"));
  ASSERT_NE(nullptr, valueb);
  ASSERT_DOUBLE_EQ(0.00002, valueb->get_number());

  delete json;
}

TEST(TestNumbers, WholeNumbersWithZeroDecimals) {
  auto json = TinyJSON::TJ::parse(R"(
{
  "a" : 12.10000,
  "b" : -42.000,
  "c" : 42.00
}
)"
);
  ASSERT_NE(nullptr, json);
  auto jobject = dynamic_cast<TinyJSON::TJValueObject*>(json);
  ASSERT_NE(nullptr, jobject);

  auto valuea = dynamic_cast<const TinyJSON::TJValueNumberFloat*>(jobject->try_get_value("a"));
  ASSERT_NE(nullptr, valuea);
  ASSERT_DOUBLE_EQ(12.1, valuea->get_number());

  auto valueb = dynamic_cast<const TinyJSON::TJValueNumberInt*>(jobject->try_get_value("b"));
  ASSERT_NE(nullptr, valueb);
  ASSERT_EQ(-42, valueb->get_number());

  // c is still an integer even if it was given as a fraction
  auto valuec = dynamic_cast<const TinyJSON::TJValueNumberInt*>(jobject->try_get_value("c"));
  ASSERT_NE(nullptr, valuec);
  ASSERT_EQ(42, valuec->get_number());

  delete json;
}

TEST(TestNumbers, WholeNumbersIsZero) {
  auto json = TinyJSON::TJ::parse(R"(
{
  "a" : 0,
  "b" : -0,
  "c" : 0.00
}
)"
);
  ASSERT_NE(nullptr, json);
  auto jobject = dynamic_cast<TinyJSON::TJValueObject*>(json);
  ASSERT_NE(nullptr, jobject);

  auto valuea = dynamic_cast<const TinyJSON::TJValueNumberInt*>(jobject->try_get_value("a"));
  ASSERT_NE(nullptr, valuea);
  ASSERT_EQ(0, valuea->get_number());

  auto valueb = dynamic_cast<const TinyJSON::TJValueNumberInt*>(jobject->try_get_value("b"));
  ASSERT_NE(nullptr, valueb);
  ASSERT_EQ(0, valueb->get_number());

  // c is still an integer even if it was given as a fraction
  auto valuec = dynamic_cast<const TinyJSON::TJValueNumberInt*>(jobject->try_get_value("c"));
  ASSERT_NE(nullptr, valuec);
  ASSERT_EQ(0, valuec->get_number());

  delete json;
}

TEST(TestNumbers, FractionNUmbers) {
  auto json = TinyJSON::TJ::parse(R"(
{
  "a" : 12.1,
  "b" : -42.6,
  "c" : 42.17,
  "d" : 0.12
}
)"
);
  ASSERT_NE(nullptr, json);
  auto jobject = dynamic_cast<TinyJSON::TJValueObject*>(json);
  ASSERT_NE(nullptr, jobject);

  auto valuea = dynamic_cast<const TinyJSON::TJValueNumberFloat*>(jobject->try_get_value("a"));
  ASSERT_NE(nullptr, valuea);
  ASSERT_DOUBLE_EQ(12.1, valuea->get_number());

  auto valueb = dynamic_cast<const TinyJSON::TJValueNumberFloat*>(jobject->try_get_value("b"));
  ASSERT_NE(nullptr, valueb);
  ASSERT_DOUBLE_EQ(-42.6, valueb->get_number());

  auto valuec = dynamic_cast<const TinyJSON::TJValueNumberFloat*>(jobject->try_get_value("c"));
  ASSERT_NE(nullptr, valuec);
  ASSERT_DOUBLE_EQ(42.17, valuec->get_number());

  auto valued = dynamic_cast<const TinyJSON::TJValueNumberFloat*>(jobject->try_get_value("d"));
  ASSERT_NE(nullptr, valued);
  ASSERT_DOUBLE_EQ(0.12, valued->get_number());

  delete json;
}

TEST(TestNumbers, MaxPositiveNumber) {
  auto json = TinyJSON::TJ::parse(R"(
{
  "a" : 9223372036854775807,
  "b" : -9223372036854775806
}
)"
);
  ASSERT_NE(nullptr, json);
  auto jobject = dynamic_cast<TinyJSON::TJValueObject*>(json);
  ASSERT_NE(nullptr, jobject);

  auto valuea = dynamic_cast<const TinyJSON::TJValueNumberInt*>(jobject->try_get_value("a"));
  ASSERT_NE(nullptr, valuea);
  ASSERT_EQ(9223372036854775807, valuea->get_number());

  auto valueb = dynamic_cast<const TinyJSON::TJValueNumberInt*>(jobject->try_get_value("b"));
  ASSERT_NE(nullptr, valueb);
  ASSERT_EQ(-9223372036854775806, valueb->get_number());

  delete json;
}

TEST(TestNumbers, InvalidWholeNumber) {
  auto json = TinyJSON::TJ::parse(R"(
{
  "a" : 12.
}
)"
);
  ASSERT_EQ(nullptr, json);
}

TEST(TestNumbers, TestManyWholeNumbers) {
  for (int i = 0; i < 19/*TJ_MAX_NUMBER_OF_DIGGITS*/;++i)
  {
    unsigned long long s = static_cast<unsigned long long>(std::pow(10, i));
    std::string value = std::to_string(s);

    std::string s_json = "{ \"a\" :" + value + "}";
    auto json = TinyJSON::TJ::parse(s_json.c_str());

    ASSERT_NE(nullptr, json);
    auto jobject = dynamic_cast<TinyJSON::TJValueObject*>(json);
    ASSERT_NE(nullptr, jobject);

    auto valuea = dynamic_cast<const TinyJSON::TJValueNumberInt*>(jobject->try_get_value("a"));
    ASSERT_NE(nullptr, valuea);
    ASSERT_EQ(s, valuea->get_number());

    delete json;

  }
}

TEST(TestNumbers, TestManyFloatNumbers) {
  struct Values
  {
    std::string given;
    double expected;
  };

  std::vector<Values> values;
  values.push_back({ "0.1", 0.1 });
  values.push_back({ "0.01", 0.01 });
  values.push_back({ "0.001", 0.001 });
  values.push_back({ "0.0001", 0.0001 });
  values.push_back({ "0.00001", 0.00001 });
  values.push_back({ "0.000001", 0.000001 });
  values.push_back({ "0.0000001", 0.0000001 });
  values.push_back({ "0.00000001", 0.00000001 });
  values.push_back({ "0.000000001", 0.000000001 });
  values.push_back({ "0.0000000001", 0.0000000001 });
  values.push_back({ "0.00000000001", 0.00000000001 });

  for (auto value : values)
  {
    std::string s_json = "{ \"a\" : " + value.given + "}";
    auto json = TinyJSON::TJ::parse(s_json.c_str());

    ASSERT_NE(nullptr, json);
    auto jobject = dynamic_cast<TinyJSON::TJValueObject*>(json);
    ASSERT_NE(nullptr, jobject);

    auto valuea = dynamic_cast<const TinyJSON::TJValueNumberFloat*>(jobject->try_get_value("a"));
    ASSERT_NE(nullptr, valuea);
    ASSERT_DOUBLE_EQ(value.expected, valuea->get_number());

    delete json;
  }
}

TEST(TestNumbers, TestManyComplexFloatNumbers) {
  struct Values
  {
    std::string given;
    double expected;
  };

  std::vector<Values> values;
  values.push_back({ "0.123456", 0.123456 });
  values.push_back({ "0.0123456", 0.0123456 });
  values.push_back({ "0.00123456", 0.00123456 });
  values.push_back({ "0.000123456", 0.000123456 });
  values.push_back({ "0.0000123456", 0.0000123456 });
  values.push_back({ "0.00000123456", 0.00000123456 });
  values.push_back({ "0.000000123456", 0.000000123456 });
  values.push_back({ "0.0000000123456", 0.0000000123456 });
  values.push_back({ "0.00000000123456", 0.00000000123456 });
  values.push_back({ "0.000000000123456", 0.000000000123456 });
  values.push_back({ "0.0000000000123456", 0.0000000000123456 });

  for (auto value : values)
  {
    std::string s_json = "{ \"a\" : " + value.given + "}";
    auto json = TinyJSON::TJ::parse(s_json.c_str());

    ASSERT_NE(nullptr, json);
    auto jobject = dynamic_cast<TinyJSON::TJValueObject*>(json);
    ASSERT_NE(nullptr, jobject);

    auto valuea = dynamic_cast<const TinyJSON::TJValueNumberFloat*>(jobject->try_get_value("a"));
    ASSERT_NE(nullptr, valuea);
    ASSERT_DOUBLE_EQ(value.expected, valuea->get_number());

    delete json;
  }
}

TEST(TestNumbers, CheckThatValueIsNumber) {
  auto json = TinyJSON::TJ::parse(R"(
{
  "a" : 123
}
)"
);
  ASSERT_NE(nullptr, json);

  auto jobject = dynamic_cast<TinyJSON::TJValueObject*>(json);
  ASSERT_NE(nullptr, jobject);

  auto number_value = jobject->try_get_value("a");
  ASSERT_NE(nullptr, number_value);
  ASSERT_FALSE(number_value->is_object());
  ASSERT_FALSE(number_value->is_array());
  ASSERT_FALSE(number_value->is_string());
  ASSERT_TRUE(number_value->is_number());
  ASSERT_FALSE(number_value->is_true());
  ASSERT_FALSE(number_value->is_false());
  ASSERT_FALSE(number_value->is_null());

  delete json;
}

TEST(TestNumbers, CheckThatValueIsNumberInArray) {
  auto json = TinyJSON::TJ::parse(R"(
[
  42
]
)"
);
  ASSERT_NE(nullptr, json);

  auto tjarray = dynamic_cast<TinyJSON::TJValueArray*>(json);
  ASSERT_NE(nullptr, tjarray);

  auto number_value = tjarray->at(0);
  ASSERT_NE(nullptr, number_value);

  ASSERT_FALSE(number_value->is_object());
  ASSERT_FALSE(number_value->is_array());
  ASSERT_FALSE(number_value->is_string());
  ASSERT_TRUE(number_value->is_number());
  ASSERT_FALSE(number_value->is_true());
  ASSERT_FALSE(number_value->is_false());
  ASSERT_FALSE(number_value->is_null());

  delete json;
}

TEST(TestNumbers, InvalidWholeNumber2) {
  auto json = TinyJSON::TJ::parse(R"(
{
  "a" : 12as
}
)"
);
  ASSERT_EQ(nullptr, json);
}

TEST(TestNumbers, UnexpectedSpaceInTheNumbers) {
  auto json = TinyJSON::TJ::parse(R"(
[
  12,14,1 5
]
)"
);
  ASSERT_EQ(nullptr, json);
}

TEST(TestBooleans, CloneInt) {
  auto number1 = new TinyJSON::TJValueNumberInt(42, false);
  auto number2 = number1->clone();
  ASSERT_NE(number1, number2);
  ASSERT_NE(nullptr, number2);

  ASSERT_TRUE(number2->is_number());
 
  auto value1 = dynamic_cast<const TinyJSON::TJValueNumberInt*>(number1);
  auto value2 = dynamic_cast<const TinyJSON::TJValueNumberInt*>(number2);
  ASSERT_EQ(value1->get_number(), value2->get_number());

  delete number1;
  delete number2;
}

TEST(TestNumbers, NumberCannotHaveALeadingZero) {
  auto json = TinyJSON::TJ::parse(R"(
{
  "a" : 0123
}
)"
);
  ASSERT_EQ(nullptr, json);
}

TEST(TestNumbers, NegativeNumberCannotHaveALeadingZero) {
  auto json = TinyJSON::TJ::parse(R"(
{
  "a" : -0123
}
)"
);
  ASSERT_EQ(nullptr, json);
}

TEST(TestNumbers, NumberCannotHaveALeadingZeroEvenIfDecimal) {
  auto json = TinyJSON::TJ::parse(R"(
{
  "a" : 0123.45
}
)"
);
  ASSERT_EQ(nullptr, json);
}

TEST(TestNumbers, JustZero) {
  auto json = TinyJSON::TJ::parse( "0");

  ASSERT_NE(nullptr, json);
  auto value = dynamic_cast<const TinyJSON::TJValueNumberInt*>(json);
  ASSERT_NE(nullptr, value);

  ASSERT_EQ(0, value->get_number());
  delete json;
}

TEST(TestNumbers, JustZeroDecimal) {
  auto json = TinyJSON::TJ::parse("0.0");

  ASSERT_NE(nullptr, json);
  auto value = dynamic_cast<const TinyJSON::TJValueNumberInt*>(json);
  ASSERT_NE(nullptr, value);

  ASSERT_EQ(0, value->get_number());
  delete json;
}

TEST(TestNumbers, TheNumberZero) {
  auto json = TinyJSON::TJ::parse("0.0");

  ASSERT_NE(nullptr, json);
  auto value = dynamic_cast<const TinyJSON::TJValueNumber*>(json);
  ASSERT_NE(nullptr, value);

  ASSERT_EQ(0, value->get_number());
  ASSERT_EQ(0, value->get_float());
  delete json;
}

TEST(TestNumbers, TheNumberNegative) {
  auto json = TinyJSON::TJ::parse("-42.0");

  ASSERT_NE(nullptr, json);
  auto value = dynamic_cast<const TinyJSON::TJValueNumber*>(json);
  ASSERT_NE(nullptr, value);

  ASSERT_EQ(-42, value->get_number());
  ASSERT_EQ(-42.0, value->get_float());
  delete json;
}

TEST(TestNumbers, TheNumberPositive) {
  auto json = TinyJSON::TJ::parse("42.0");

  ASSERT_NE(nullptr, json);
  auto value = dynamic_cast<const TinyJSON::TJValueNumber*>(json);
  ASSERT_NE(nullptr, value);

  ASSERT_EQ(42, value->get_number());
  ASSERT_EQ(42.0, value->get_float());
  delete json;
}

TEST(TestNumbers, TheNumberNegativeAndFloat) {
  auto json = TinyJSON::TJ::parse("-42.1234");

  ASSERT_NE(nullptr, json);
  auto value = dynamic_cast<const TinyJSON::TJValueNumber*>(json);
  ASSERT_NE(nullptr, value);

  ASSERT_EQ(-42, value->get_number());
  ASSERT_DOUBLE_EQ(-42.1234, value->get_float());
  delete json;
}

TEST(TestNumbers, TheNumberPositiveAndFloat) {
  auto json = TinyJSON::TJ::parse("42.1234");

  ASSERT_NE(nullptr, json);
  auto value = dynamic_cast<const TinyJSON::TJValueNumber*>(json);
  ASSERT_NE(nullptr, value);

  ASSERT_EQ(42, value->get_number());
  ASSERT_DOUBLE_EQ(42.1234, value->get_float());
  delete json;
}

TEST(TestNumbers, CreateFloatByPassingNumber) {
  auto json = new TinyJSON::TJValueNumberFloat(42.5);

  ASSERT_NE(nullptr, json);
  ASSERT_EQ(42.5, json->get_number());

  auto value = dynamic_cast<const TinyJSON::TJValueNumber*>(json);
  ASSERT_NE(nullptr, value);

  ASSERT_EQ(42, value->get_number());
  ASSERT_EQ(42.5, value->get_float());
  delete json;
}

TEST(TestNumbers, CreateFloatByPassingNegativeNumber) {
  auto json = new TinyJSON::TJValueNumberFloat(-42.5);

  ASSERT_NE(nullptr, json);
  ASSERT_EQ(-42.5, json->get_number());

  auto value = dynamic_cast<const TinyJSON::TJValueNumber*>(json);
  ASSERT_NE(nullptr, value);

  ASSERT_EQ(-42, value->get_number());
  ASSERT_EQ(-42.5, value->get_float());
  delete json;
  }

  TEST(TestNumbers, OperatorBracketAccess)
  {
  auto json = TinyJSON::TJ::parse("123");
  ASSERT_NE(nullptr, json);

  // operator[] on number should return null
  ASSERT_TRUE((*json)["any_key"].is_null());

  delete json;
  }


// https://github.com/FFMG/tinyjson/issues/54
// A number that does not fit must either be rejected, (parse returns nullptr),
// or keep (roughly) its real value, what it must never do is silently wrap around.
static void assert_rejected_or_close_to(const std::string& value, const long double expected)
{
  auto json = TinyJSON::TJ::parse(value.c_str());
  if (nullptr == json)
  {
    return;
  }
  const auto actual = json->get_float();
  delete json;
  ASSERT_NEAR(1.0, static_cast<double>(actual / expected), 1e-9) << "Value: " << value << " was parsed as " << actual;
}

// https://github.com/FFMG/tinyjson/issues/54
// A huge positive exponent must either be rejected or become infinite.
static void assert_rejected_or_infinite(const std::string& value)
{
  auto json = TinyJSON::TJ::parse(value.c_str());
  if (nullptr == json)
  {
    return;
  }
  const auto actual = json->get_float();
  delete json;
  ASSERT_TRUE(std::isinf(actual)) << "Value: " << value << " was parsed as " << actual;
}

// https://github.com/FFMG/tinyjson/issues/54
// A huge negative exponent must either be rejected or become zero.
static void assert_rejected_or_zero(const std::string& value)
{
  auto json = TinyJSON::TJ::parse(value.c_str());
  if (nullptr == json)
  {
    return;
  }
  const auto actual = json->get_float();
  delete json;
  ASSERT_EQ(0.0L, actual) << "Value: " << value << " was parsed as " << actual;
}

// https://github.com/FFMG/tinyjson/issues/54
TEST(TestNumbers, Issue54_WholeNumberLargerThan64BitsDoesNotWrap)
{
  std::vector<std::string> values;
  values.push_back("18446744073709551616");  // 2^64, wraps to 0
  values.push_back("18446744073709551617");  // 2^64+1, wraps to 1
  values.push_back("-18446744073709551617"); // wraps to -1
  values.push_back("99999999999999999999");
  values.push_back("123456789012345678901234567890");
  values.push_back("18446744073709551617.5");
  values.push_back("18446744073709551617e2");
  for (auto& value : values)
  {
    assert_rejected_or_close_to(value, std::stold(value));
  }
}

// https://github.com/FFMG/tinyjson/issues/54
TEST(TestNumbers, Issue54_FractionLongerThan64BitsDoesNotWrap)
{
  std::vector<std::string> values;
  values.push_back("0.18446744073709551617"); // fraction wraps to 1, (0.00000000000000000001)
  values.push_back("1.18446744073709551617");
  values.push_back("-0.18446744073709551617");
  values.push_back("0.99999999999999999999");
  values.push_back("3.14159265358979323846264338327950288");
  values.push_back("1.18446744073709551617e2");
  values.push_back("1.18446744073709551617e-2");
  for (auto& value : values)
  {
    assert_rejected_or_close_to(value, std::stold(value));
  }
}

// https://github.com/FFMG/tinyjson/issues/54
// The exponent is cast to an unsigned int so 2^32 wraps to 0 and 2^32+1 wraps to 1
TEST(TestNumbers, Issue54_PositiveExponentLargerThan32BitsDoesNotWrap)
{
  std::vector<std::string> values;
  values.push_back("1e4294967296");  // becomes 1
  values.push_back("1e4294967297");  // becomes 10
  values.push_back("1e+4294967296");
  values.push_back("1E4294967296");
  values.push_back("1.5e4294967296");
  values.push_back("1e18446744073709551617");
  for (auto& value : values)
  {
    assert_rejected_or_infinite(value);
  }
}

// https://github.com/FFMG/tinyjson/issues/54
// The unsigned exponent is multiplied by -1 so 2^32-1 becomes +1
TEST(TestNumbers, Issue54_NegativeExponentLargerThan32BitsDoesNotWrap)
{
  std::vector<std::string> values;
  values.push_back("1e-4294967295"); // becomes 10
  values.push_back("1e-4294967296"); // becomes 1
  values.push_back("1.5e-4294967295");
  values.push_back("1e-18446744073709551617");
  for (auto& value : values)
  {
    assert_rejected_or_zero(value);
  }
}

// https://github.com/FFMG/tinyjson/issues/54
// 2^31 becomes INT_MIN, and -1 * INT_MIN is undefined behaviour.
TEST(TestNumbers, Issue54_ExponentOfIntMinDoesNotWrap)
{
  assert_rejected_or_zero("1e-2147483648");
  assert_rejected_or_infinite("1e2147483648");
}

// https://github.com/FFMG/tinyjson/issues/54
// The reproduction given in the issue.
TEST(TestNumbers, Issue54_ExampleFromTheIssue)
{
  auto json = TinyJSON::TJ::parse("[18446744073709551617, 1e4294967296]");
  if (nullptr == json)
  {
    return;
  }
  auto jarray = dynamic_cast<TinyJSON::TJValueArray*>(json);
  ASSERT_NE(nullptr, jarray);
  ASSERT_EQ(2u, jarray->get_number_of_items());

  const auto first = jarray->at(0)->get_float();
  const auto second = jarray->at(1)->get_float();
  delete json;

  ASSERT_NEAR(1.0, static_cast<double>(first / 18446744073709551617.0L), 1e-9);
  ASSERT_TRUE(std::isinf(second));
}


// https://github.com/FFMG/tinyjson/issues/55
// Whole numbers between 2^63 and 2^64-1 fit in an unsigned long long but not in a long long.
// Either the number is rejected, (parse returns nullptr), or get_number() must not flip the sign,
// (clamping or throwing are both acceptable), get_float() must keep (roughly) the real value
// and dump() must give the original value back.
static void assert_rejected_or_does_not_wrap_to_long_long(const std::string& value)
{
  auto json = TinyJSON::TJ::parse(value.c_str());
  if (nullptr == json)
  {
    return;
  }

  const auto& text = json->dump(TinyJSON::formatting::minify);
  ASSERT_NE(nullptr, text) << "Value: " << value;
  const std::string dumped(text);

  const auto is_negative = value[0] == '-';
  bool sign_is_preserved = true;
  long long actual_number = 0;
  try
  {
    actual_number = json->get_number();
    sign_is_preserved = is_negative ? actual_number <= 0 : actual_number >= 0;
  }
  catch (const TinyJSON::TJParseException&)
  {
    // throwing because the number does not fit is acceptable.
  }
  delete json;

  ASSERT_EQ(value, dumped);
  ASSERT_TRUE(sign_is_preserved) << "Value: " << value << " was returned as " << actual_number;
  assert_rejected_or_close_to(value, std::stold(value));
}

// https://github.com/FFMG/tinyjson/issues/55
TEST(TestNumbers, Issue55_WholeNumberLargerThanLongLongDoesNotWrap)
{
  std::vector<std::string> values;
  values.push_back("9223372036854775808");   // LLONG_MAX+1, becomes LLONG_MIN
  values.push_back("9223372036854775809");
  values.push_back("18446744073709551615");  // ULLONG_MAX, becomes -1
  values.push_back("-9223372036854775809");  // LLONG_MIN-1, becomes LLONG_MAX
  values.push_back("-18446744073709551615"); // becomes 1
  for (auto& value : values)
  {
    assert_rejected_or_does_not_wrap_to_long_long(value);
  }
}

// https://github.com/FFMG/tinyjson/issues/55
// LLONG_MIN is a valid long long, but get_number() does -1 * LLONG_MIN which is undefined behaviour.
TEST(TestNumbers, Issue55_WholeNumberLongLongMinIsValid)
{
  auto json = TinyJSON::TJ::parse("-9223372036854775808");
  ASSERT_NE(nullptr, json);

  const auto& text = json->dump(TinyJSON::formatting::minify);
  ASSERT_NE(nullptr, text);
  ASSERT_STREQ("-9223372036854775808", text);

  ASSERT_EQ(std::numeric_limits<long long>::min(), json->get_number());
  delete json;
}

// https://github.com/FFMG/tinyjson/issues/55
// LLONG_MAX is a valid long long and must not be affected by any fix.
TEST(TestNumbers, Issue55_WholeNumberLongLongMaxIsValid)
{
  auto json = TinyJSON::TJ::parse("9223372036854775807");
  ASSERT_NE(nullptr, json);

  const auto& text = json->dump(TinyJSON::formatting::minify);
  ASSERT_NE(nullptr, text);
  ASSERT_STREQ("9223372036854775807", text);

  ASSERT_EQ(std::numeric_limits<long long>::max(), json->get_number());
  delete json;
}

// https://github.com/FFMG/tinyjson/issues/1
// When numbers exceed supported precision or memory bounds, they should be truncated or converted.

// Functor to track warnings without using lambdas
struct Issue1WarningTracker
{
  int count = 0;
  std::string last_message;

  void operator()(TinyJSON::parse_options::message_type type, const TJCHAR* message, int, int)
  {
    if (type == TinyJSON::parse_options::message_type::warning)
    {
      count++;
      if (nullptr != message)
      {
        last_message = message;
      }
    }
  }
};

// The example from the issue: fraction with 27 digits is truncated to at most 19 significant digits.
TEST(TestNumbers, Issue1_ExampleFractionTruncatedToSignificantDigits)
{
  auto json = TinyJSON::TJ::parse(R"({ "a" : 0.123456789123456789123456789 })");
  ASSERT_NE(nullptr, json);

  auto jobject = dynamic_cast<const TinyJSON::TJValueObject*>(json);
  ASSERT_NE(nullptr, jobject);

  auto val = jobject->try_get_value("a");
  ASSERT_NE(nullptr, val);

  ASSERT_STREQ("0.1234567891234567891", val->dump(TinyJSON::formatting::minify));
  ASSERT_NEAR(0.1234567891234567891, static_cast<double>(val->get_float()), 1e-15);

  delete json;
}

// Negative fraction with 27 digits is truncated to at most 19 significant digits.
TEST(TestNumbers, Issue1_NegativeFractionTruncatedToSignificantDigits)
{
  auto json = TinyJSON::TJ::parse(R"({ "a" : -0.123456789123456789123456789 })");
  ASSERT_NE(nullptr, json);

  auto jobject = dynamic_cast<const TinyJSON::TJValueObject*>(json);
  ASSERT_NE(nullptr, jobject);

  auto val = jobject->try_get_value("a");
  ASSERT_NE(nullptr, val);

  ASSERT_STREQ("-0.1234567891234567891", val->dump(TinyJSON::formatting::minify));
  ASSERT_NEAR(-0.1234567891234567891, static_cast<double>(val->get_float()), 1e-15);

  delete json;
}

// The example from the issue: whole number with 27 digits is converted to an exponent number.
TEST(TestNumbers, Issue1_ExampleWholeNumberConvertedToExponent)
{
  auto json = TinyJSON::TJ::parse(R"({ "a" : 123456789123456789123456789 })");
  ASSERT_NE(nullptr, json);

  auto jobject = dynamic_cast<const TinyJSON::TJValueObject*>(json);
  ASSERT_NE(nullptr, jobject);

  auto val = jobject->try_get_value("a");
  ASSERT_NE(nullptr, val);

  // 20 digits kept (1.2345678912345678912) and remaining 7 digits shifted to exponent (+26)
  ASSERT_STREQ("1.2345678912345678912e+26", val->dump(TinyJSON::formatting::minify));
  ASSERT_NEAR(1.2345678912345678912e26, static_cast<double>(val->get_float()), 1e11);

  delete json;
}

// Negative whole number with 27 digits is converted to an exponent number.
TEST(TestNumbers, Issue1_NegativeWholeNumberConvertedToExponent)
{
  auto json = TinyJSON::TJ::parse(R"({ "a" : -123456789123456789123456789 })");
  ASSERT_NE(nullptr, json);

  auto jobject = dynamic_cast<const TinyJSON::TJValueObject*>(json);
  ASSERT_NE(nullptr, jobject);

  auto val = jobject->try_get_value("a");
  ASSERT_NE(nullptr, val);

  ASSERT_STREQ("-1.2345678912345678912e+26", val->dump(TinyJSON::formatting::minify));
  ASSERT_NEAR(-1.2345678912345678912e26, static_cast<double>(val->get_float()), 1e11);

  delete json;
}

// When the whole number exceeds ULLONG_MAX, the fraction digits are negligible.
TEST(TestNumbers, Issue1_WholeNumberAndFractionBothExceedingPrecision)
{
  auto json = TinyJSON::TJ::parse(R"({ "a" : 123456789123456789123456789.9876543210123456789 })");
  ASSERT_NE(nullptr, json);

  auto jobject = dynamic_cast<const TinyJSON::TJValueObject*>(json);
  ASSERT_NE(nullptr, jobject);

  auto val = jobject->try_get_value("a");
  ASSERT_NE(nullptr, val);

  ASSERT_STREQ("1.2345678912345678912e+26", val->dump(TinyJSON::formatting::minify));

  delete json;
}

// An extremely long run of digits (5,000 zeros) parses and dumps without overflow or crash.
TEST(TestNumbers, Issue1_ExtremelyLongWholeNumberRunsDoNotOverflow)
{
  const std::string long_number = "1" + std::string(5000, '0');
  auto json = TinyJSON::TJ::parse(long_number.c_str());
  ASSERT_NE(nullptr, json);

  const auto& text = json->dump(TinyJSON::formatting::minify);
  ASSERT_NE(nullptr, text);
  ASSERT_STREQ("1e+5000", text);

  delete json;
}

// An extremely long fraction (5,000 digits) is truncated to 19 significant digits.
TEST(TestNumbers, Issue1_ExtremelyLongFractionRunsDoNotOverflow)
{
  const std::string long_fraction = "0." + std::string(5000, '9');
  auto json = TinyJSON::TJ::parse(long_fraction.c_str());
  ASSERT_NE(nullptr, json);

  const auto& text = json->dump(TinyJSON::formatting::minify);
  ASSERT_NE(nullptr, text);
  ASSERT_STREQ("0.9999999999999999999", text);

  delete json;
}

// A whole number with more than TJ_MAX_EXPONENT digits (> 100,000) exceeds maximum exponent and is rejected.
TEST(TestNumbers, Issue1_WholeNumberExceedingMaxExponentIsRejected)
{
  const std::string huge_number = "1" + std::string(100021, '0');
  auto json = TinyJSON::TJ::parse(huge_number.c_str());
  EXPECT_EQ(nullptr, json);
}

// A warning should be emitted when precision is lost by truncating whole number digits.
TEST(TestNumbers, Issue1_WarningTriggeredWhenWholeNumberPrecisionIsLost)
{
  Issue1WarningTracker tracker;
  TinyJSON::parse_options options = {};
  options.callback_function = std::ref(tracker);

  auto json = TinyJSON::TJ::parse(R"({ "a" : 123456789123456789123456789 })", options);
  ASSERT_NE(nullptr, json);

  EXPECT_GT(tracker.count, 0);

  delete json;
}

// A warning should be emitted when precision is lost by truncating fraction digits.
TEST(TestNumbers, Issue1_WarningTriggeredWhenFractionPrecisionIsLost)
{
  Issue1WarningTracker tracker;
  TinyJSON::parse_options options = {};
  options.callback_function = std::ref(tracker);

  auto json = TinyJSON::TJ::parse(R"({ "a" : 0.123456789123456789123456789 })", options);
  ASSERT_NE(nullptr, json);

  EXPECT_GT(tracker.count, 0);

  delete json;
}

// No warning should be emitted when numbers fit within supported precision.
TEST(TestNumbers, Issue1_NoWarningWhenNumberFitsWithinPrecision)
{
  Issue1WarningTracker tracker;
  TinyJSON::parse_options options = {};
  options.callback_function = std::ref(tracker);

  auto json = TinyJSON::TJ::parse(R"([ 123456789012345678, 0.123456789012345678 ])", options);
  ASSERT_NE(nullptr, json);

  EXPECT_EQ(0, tracker.count);

  delete json;
}
