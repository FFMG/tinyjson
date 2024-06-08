// Licensed to Florent Guelfucci under one or more agreements.
// Florent Guelfucci licenses this file to you under the MIT license.
// See the LICENSE file in the project root for more information.
#include <gtest/gtest.h>
#include <vector>
#include "../src/TinyJSON.h"

TEST(TestNumbers, NumberIsAfterMissingColon) {
  auto json = TinyJSON::TinyJSON::parse(R"(
    {
      "a" 42
    }
    )"
  );
  ASSERT_EQ(nullptr, json);
}

TEST(TestNumbers, WholeNumbers) {
  auto json = TinyJSON::TinyJSON::parse(R"(
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
  auto json = TinyJSON::TinyJSON::parse(R"(
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
  ASSERT_EQ(1.0001, valuea->get_number());

  auto valueb = dynamic_cast<const TinyJSON::TJValueNumberFloat*>(jobject->try_get_value("b"));
  ASSERT_NE(nullptr, valueb);
  ASSERT_EQ(0.00002, valueb->get_number());

  delete json;
}

TEST(TestNumbers, WholeNumbersWithZeroDecimals) {
  auto json = TinyJSON::TinyJSON::parse(R"(
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
  ASSERT_EQ(12.1, valuea->get_number());

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
  auto json = TinyJSON::TinyJSON::parse(R"(
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
  auto json = TinyJSON::TinyJSON::parse(R"(
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
  ASSERT_EQ(12.1, valuea->get_number());

  auto valueb = dynamic_cast<const TinyJSON::TJValueNumberFloat*>(jobject->try_get_value("b"));
  ASSERT_NE(nullptr, valueb);
  ASSERT_EQ(-42.6, valueb->get_number());

  auto valuec = dynamic_cast<const TinyJSON::TJValueNumberFloat*>(jobject->try_get_value("c"));
  ASSERT_NE(nullptr, valuec);
  ASSERT_EQ(42.17, valuec->get_number());

  auto valued = dynamic_cast<const TinyJSON::TJValueNumberFloat*>(jobject->try_get_value("d"));
  ASSERT_NE(nullptr, valued);
  ASSERT_EQ(0.12, valued->get_number());

  delete json;
}

TEST(TestNumbers, MaxPositiveNumber) {
  auto json = TinyJSON::TinyJSON::parse(R"(
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
  auto json = TinyJSON::TinyJSON::parse(R"(
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
    auto json = TinyJSON::TinyJSON::parse(s_json.c_str());

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
    auto json = TinyJSON::TinyJSON::parse(s_json.c_str());

    ASSERT_NE(nullptr, json);
    auto jobject = dynamic_cast<TinyJSON::TJValueObject*>(json);
    ASSERT_NE(nullptr, jobject);

    auto valuea = dynamic_cast<const TinyJSON::TJValueNumberFloat*>(jobject->try_get_value("a"));
    ASSERT_NE(nullptr, valuea);
    ASSERT_EQ(value.expected, valuea->get_number());

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
    auto json = TinyJSON::TinyJSON::parse(s_json.c_str());

    ASSERT_NE(nullptr, json);
    auto jobject = dynamic_cast<TinyJSON::TJValueObject*>(json);
    ASSERT_NE(nullptr, jobject);

    auto valuea = dynamic_cast<const TinyJSON::TJValueNumberFloat*>(jobject->try_get_value("a"));
    ASSERT_NE(nullptr, valuea);
    ASSERT_EQ(value.expected, valuea->get_number());

    delete json;

  }
}

TEST(TestNumbers, CheckThatValueIsNumber) {
  auto json = TinyJSON::TinyJSON::parse(R"(
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
  auto json = TinyJSON::TinyJSON::parse(R"(
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
  auto json = TinyJSON::TinyJSON::parse(R"(
{
  "a" : 12as
}
)"
);
  ASSERT_EQ(nullptr, json);
}

TEST(TestNumbers, UnexpectedSpaceInTheNumbers) {
  auto json = TinyJSON::TinyJSON::parse(R"(
[
  12,14,1 5
]
)"
);
  ASSERT_EQ(nullptr, json);
}
