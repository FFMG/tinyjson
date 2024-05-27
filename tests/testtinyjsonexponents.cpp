// Licensed to Florent Guelfucci under one or more agreements.
// Florent Guelfucci licenses this file to you under the MIT license.
// See the LICENSE file in the project root for more information.
#include <gtest/gtest.h>
#include "../src/TinyJSON.h"

TEST(TestExponents, FractionsWithLeadingZeros) {
  auto json = TinyJSON::TinyJSON::parse(R"(
{
  "a" : 12001e-3,
  "b" : 120012e-4,
  "c" : 12340012e-4,
  "d" : 1e-4
}
)"
);
  ASSERT_NE(nullptr, json);
  auto jobject = dynamic_cast<TinyJSON::TJValueObject*>(json);
  ASSERT_NE(nullptr, jobject);

  auto valuea = dynamic_cast<const TinyJSON::TJValueNumberFloat*>(jobject->try_get_value("a"));
  ASSERT_NE(nullptr, valuea);
  ASSERT_EQ(12.001, valuea->get_number());

  auto valueb = dynamic_cast<const TinyJSON::TJValueNumberFloat*>(jobject->try_get_value("b"));
  ASSERT_NE(nullptr, valueb);
  ASSERT_EQ(12.0012, valueb->get_number());

  auto valuec = dynamic_cast<const TinyJSON::TJValueNumberFloat*>(jobject->try_get_value("c"));
  ASSERT_NE(nullptr, valuec);
  ASSERT_EQ(1234.0012, valuec->get_number());

  auto valued = dynamic_cast<const TinyJSON::TJValueNumberFloat*>(jobject->try_get_value("d"));
  ASSERT_NE(nullptr, valued);
  ASSERT_EQ(0.0001, valued->get_number());

  delete json;
}

TEST(TestExponents, InvalidWholeNumberWithExponent) {
  auto json = TinyJSON::TinyJSON::parse(R"(
{
  "a" : 12.e2
}
)"
);
  ASSERT_EQ(nullptr, json);
}

TEST(TestExponents, InvalidMissingPositiveExponent) {
  auto json = TinyJSON::TinyJSON::parse(R"(
{
  "a" : 12.2e
}
)"
);
  ASSERT_EQ(nullptr, json);
}

TEST(TestExponents, InvalidMissingNegativeExponent) {
  auto json = TinyJSON::TinyJSON::parse(R"(
{
  "a" : 12.2-e
}
)"
);
  ASSERT_EQ(nullptr, json);
}

TEST(TestExponents, ExponentCannotBeZero) {
  auto json = TinyJSON::TinyJSON::parse(R"(
{
  "a" : 12.2e0
}
)"
);
  ASSERT_EQ(nullptr, json);
}

TEST(TestExponents, FractionNUmbersWithExponentIsActuallyWholeNumber) {
  auto json = TinyJSON::TinyJSON::parse(R"(
{
  "a" : 12.1e1,
  "b" : 12.1e2,
  "c" : 12.3e6
}
)"
);
  ASSERT_NE(nullptr, json);
  auto jobject = dynamic_cast<TinyJSON::TJValueObject*>(json);
  ASSERT_NE(nullptr, jobject);

  auto valuea = dynamic_cast<const TinyJSON::TJValueNumberInt*>(jobject->try_get_value("a"));
  ASSERT_NE(nullptr, valuea);
  ASSERT_EQ(121, valuea->get_number());

  auto valueb = dynamic_cast<const TinyJSON::TJValueNumberInt*>(jobject->try_get_value("b"));
  ASSERT_NE(nullptr, valueb);
  ASSERT_EQ(1210, valueb->get_number());

  auto valuec = dynamic_cast<const TinyJSON::TJValueNumberInt*>(jobject->try_get_value("c"));
  ASSERT_NE(nullptr, valuec);
  ASSERT_EQ(12300000, valuec->get_number());

  delete json;
}

TEST(TestExponents, FractionNUmbersWithExponentRemoveUnusedExponent) {
  auto json = TinyJSON::TinyJSON::parse(R"(
{
  "a" : 123.456e2,
  "b" : -123.456e2
}
)"
);
  ASSERT_NE(nullptr, json);
  auto jobject = dynamic_cast<TinyJSON::TJValueObject*>(json);
  ASSERT_NE(nullptr, jobject);

  auto valuea = dynamic_cast<const TinyJSON::TJValueNumberFloat*>(jobject->try_get_value("a"));
  ASSERT_NE(nullptr, valuea);
  ASSERT_EQ(12345.6, valuea->get_number());

  auto valueb = dynamic_cast<const TinyJSON::TJValueNumberFloat*>(jobject->try_get_value("b"));
  ASSERT_NE(nullptr, valueb);
  ASSERT_EQ(-12345.6, valueb->get_number());

  delete json;
}

TEST(TestExponents, ExponentWithNoFraction) {
  auto json = TinyJSON::TinyJSON::parse(R"(
{
  "a" : 12e3
}
)"
);
  ASSERT_NE(nullptr, json);
  auto jobject = dynamic_cast<TinyJSON::TJValueObject*>(json);
  ASSERT_NE(nullptr, jobject);

  auto valuea = dynamic_cast<const TinyJSON::TJValueNumberInt*>(jobject->try_get_value("a"));
  ASSERT_NE(nullptr, valuea);
  ASSERT_EQ(12000, valuea->get_number());

  delete json;
}

TEST(TestExponents, NegativeExponentDoesNotAlwaysMakeFraction) {
  auto json = TinyJSON::TinyJSON::parse(R"(
{
  "a" : 12000e-3
}
)"
);
  ASSERT_NE(nullptr, json);
  auto jobject = dynamic_cast<TinyJSON::TJValueObject*>(json);
  ASSERT_NE(nullptr, jobject);

  auto valuea = dynamic_cast<const TinyJSON::TJValueNumberInt*>(jobject->try_get_value("a"));
  ASSERT_NE(nullptr, valuea);
  ASSERT_EQ(12, valuea->get_number());

  delete json;
}
