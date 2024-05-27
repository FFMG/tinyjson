// Licensed to Florent Guelfucci under one or more agreements.
// Florent Guelfucci licenses this file to you under the MIT license.
// See the LICENSE file in the project root for more information.
#include <gtest/gtest.h>
#include "../src/TinyJSON.h"

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

TEST(TestNumbers, InvalidWholeNumber) {
  auto json = TinyJSON::TinyJSON::parse(R"(
{
  "a" : 12.
}
)"
);
  ASSERT_EQ(nullptr, json);
}
