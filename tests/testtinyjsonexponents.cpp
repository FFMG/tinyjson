// Licensed to Florent Guelfucci under one or more agreements.
// Florent Guelfucci licenses this file to you under the MIT license.
// See the LICENSE file in the project root for more information.
#include <gtest/gtest.h>
#define TJ_USE_CHAR 1
#include "../src/TinyJSON.h"

TEST(TestExponents, FractionsWithLeadingZeros) {
  auto json = TinyJSON::TJ::parse(R"(
{
  "a" : 62001e-3,
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
  ASSERT_EQ(62.001, valuea->get_number());

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
  auto json = TinyJSON::TJ::parse(R"(
{
  "a" : 12.e2
}
)"
);
  ASSERT_EQ(nullptr, json);
}

TEST(TestExponents, InvalidMissingPositiveExponent) {
  auto json = TinyJSON::TJ::parse(R"(
{
  "a" : 12.2e
}
)"
);
  ASSERT_EQ(nullptr, json);
}

TEST(TestExponents, InvalidMissingNegativeExponent) {
  auto json = TinyJSON::TJ::parse(R"(
{
  "a" : 12.2-e
}
)"
);
  ASSERT_EQ(nullptr, json);
}

TEST(TestExponents, ExponentCanBeZero) {
  TinyJSON::parse_options options = {};
  options.throw_exception = true;
  auto json = TinyJSON::TJ::parse(R"(
{
  "number" : 12e0
}
)", options
);
  ASSERT_NE(nullptr, json);

  auto jobject = dynamic_cast<TinyJSON::TJValueObject*>(json);
  ASSERT_NE(nullptr, jobject);

  auto valuea = dynamic_cast<const TinyJSON::TJValueNumberInt*>(jobject->try_get_value("number"));
  ASSERT_NE(nullptr, valuea);
  ASSERT_EQ(12, valuea->get_number());

  delete json;
}

TEST(TestExponents, NegativeExponentCanBeZero) {
  TinyJSON::parse_options options = {};
  options.throw_exception = true;
  auto json = TinyJSON::TJ::parse(R"(
{
  "number" : 12e-000
}
)", options
);
  ASSERT_NE(nullptr, json);

  auto jobject = dynamic_cast<TinyJSON::TJValueObject*>(json);
  ASSERT_NE(nullptr, jobject);

  auto valuea = dynamic_cast<const TinyJSON::TJValueNumberInt*>(jobject->try_get_value("number"));
  ASSERT_NE(nullptr, valuea);
  ASSERT_EQ(12, valuea->get_number());

  delete json;
}

TEST(TestExponents, FractionNUmbersWithExponentIsActuallyWholeNumber) {
  auto json = TinyJSON::TJ::parse(R"(
{
  "a" : 12.3e1,
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
  ASSERT_EQ(123, valuea->get_number());

  auto valueb = dynamic_cast<const TinyJSON::TJValueNumberInt*>(jobject->try_get_value("b"));
  ASSERT_NE(nullptr, valueb);
  ASSERT_EQ(1210, valueb->get_number());

  auto valuec = dynamic_cast<const TinyJSON::TJValueNumberInt*>(jobject->try_get_value("c"));
  ASSERT_NE(nullptr, valuec);
  ASSERT_EQ(12300000, valuec->get_number());

  delete json;
}

TEST(TestExponents, FractionNUmbersWithExponentIsActuallyWholeNumberWithPlusSign) {
  auto json = TinyJSON::TJ::parse(R"(
{
  "a" : 12.1e+1,
  "b" : 12.1e+2,
  "c" : 12.3e+6
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
  auto json = TinyJSON::TJ::parse(R"(
{
  "a" : 123.456e2,
  "b" : -123.456e2,
  "c" : 123.456e+2,
  "d" : -123.456e+2
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

  auto valuec = dynamic_cast<const TinyJSON::TJValueNumberFloat*>(jobject->try_get_value("c"));
  ASSERT_NE(nullptr, valuec);
  ASSERT_EQ(12345.6, valuec->get_number());

  auto valued = dynamic_cast<const TinyJSON::TJValueNumberFloat*>(jobject->try_get_value("d"));
  ASSERT_NE(nullptr, valued);
  ASSERT_EQ(-12345.6, valued->get_number());

  delete json;
}

TEST(TestExponents, ExponentWithNoFraction) {
  auto json = TinyJSON::TJ::parse(R"(
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
  auto json = TinyJSON::TJ::parse(R"(
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

TEST(TestExponents, LargeExponentIsConvertedToSingleWholeNumber) {
  auto json = TinyJSON::TJ::parse(R"(
{
  "a" : 123.045e+25,
  "b" : 678.09e+25,
  "c" : 100.09e+25,
  "d" : 123.45e+25
}
)"
);
  ASSERT_NE(nullptr, json);
  auto jobject = dynamic_cast<TinyJSON::TJValueObject*>(json);
  ASSERT_NE(nullptr, jobject);

  auto valuea = dynamic_cast<const TinyJSON::TJValueNumberExponent*>(jobject->try_get_value("a"));
  ASSERT_NE(nullptr, valuea);
  EXPECT_STREQ("1.23045e+27", valuea->dump(TinyJSON::formating::none));

  auto valueb = dynamic_cast<const TinyJSON::TJValueNumberExponent*>(jobject->try_get_value("b"));
  ASSERT_NE(nullptr, valueb);
  EXPECT_STREQ("6.7809e+27", valueb->dump(TinyJSON::formating::none));

  auto valuec = dynamic_cast<const TinyJSON::TJValueNumberExponent*>(jobject->try_get_value("c"));
  ASSERT_NE(nullptr, valuec);
  EXPECT_STREQ("1.0009e+27", valuec->dump(TinyJSON::formating::none));

  auto valued = dynamic_cast<const TinyJSON::TJValueNumberExponent*>(jobject->try_get_value("d"));
  ASSERT_NE(nullptr, valued);
  EXPECT_STREQ("1.2345e+27", valued->dump(TinyJSON::formating::none));

  delete json;
}

TEST(TestExponents, TinyNumberWithLargeExponentShiftsEnoughToBecomeANumberAgain) {
  auto json = TinyJSON::TJ::parse(R"(
{
  "a" : 0.00000000000000000001e+24
}
)"
);
  ASSERT_NE(nullptr, json);
  auto jobject = dynamic_cast<TinyJSON::TJValueObject*>(json);
  ASSERT_NE(nullptr, jobject);

  auto valuea = dynamic_cast<const TinyJSON::TJValueNumberInt*>(jobject->try_get_value("a"));
  ASSERT_NE(nullptr, valuea);
  ASSERT_EQ(10000, valuea->get_number());

  delete json;
}

TEST(TestExponents, TinyNumberWithLargeExponentShiftsEnoughToBecomeFloatNumber) {
  auto json = TinyJSON::TJ::parse(R"(
{
  "a" : 0.00000000000000000001e+18
}
)"
);
  ASSERT_NE(nullptr, json);
  auto jobject = dynamic_cast<TinyJSON::TJValueObject*>(json);
  ASSERT_NE(nullptr, jobject);

  auto valuea = dynamic_cast<const TinyJSON::TJValueNumberFloat*>(jobject->try_get_value("a"));
  ASSERT_NE(nullptr, valuea);
  ASSERT_EQ(0.01, valuea->get_number());

  delete json;
}

TEST(TestExponents, NumberJustShiftsEnoughToBecomeANumberAgain) {
  auto json = TinyJSON::TJ::parse(R"(
{
  "a" : 0.00001000000000000001e+18
}
)"
);
  ASSERT_NE(nullptr, json);
  auto jobject = dynamic_cast<TinyJSON::TJValueObject*>(json);
  ASSERT_NE(nullptr, jobject);

  auto valuea = dynamic_cast<const TinyJSON::TJValueNumberFloat*>(jobject->try_get_value("a"));
  ASSERT_NE(nullptr, valuea);
  ASSERT_EQ(10000000000000.01, valuea->get_number());

  delete json;
}

TEST(TestExponents, ShortNumberWithLongPositiveExponentShiftsEnoughToBecomeANumberAgain) {
  auto json = TinyJSON::TJ::parse(R"(
{
  "a" : 0.00001e+24
}
)"
);
  ASSERT_NE(nullptr, json);
  auto jobject = dynamic_cast<TinyJSON::TJValueObject*>(json);
  ASSERT_NE(nullptr, jobject);

  auto valuea = dynamic_cast<const TinyJSON::TJValueNumberInt*>(jobject->try_get_value("a"));
  ASSERT_NE(nullptr, valuea);
  ASSERT_EQ(10000000000000000000, valuea->get_number());

  delete json;
}

TEST(TestExponents, PositiveExponentNumberCannotBeConvertedToWholeNumber) {
  auto json = TinyJSON::TJ::parse(R"(
{
  "a" : 2.00001e+24
}
)"
);
  ASSERT_NE(nullptr, json);
  auto jobject = dynamic_cast<TinyJSON::TJValueObject*>(json);
  ASSERT_NE(nullptr, jobject);

  auto valuea = dynamic_cast<const TinyJSON::TJValueNumberExponent*>(jobject->try_get_value("a"));
  ASSERT_NE(nullptr, valuea);
  ASSERT_STREQ("2.00001e+24", valuea->dump(TinyJSON::formating::none));

  delete json;
}

TEST(TestExponents, PositiveExponentOfNegativeNumberNumberCannotBeConvertedToWholeNumber) {
  auto json = TinyJSON::TJ::parse(R"(
{
  "a" : -2.00001e+24
}
)"
);
  ASSERT_NE(nullptr, json);
  auto jobject = dynamic_cast<TinyJSON::TJValueObject*>(json);
  ASSERT_NE(nullptr, jobject);

  auto valuea = dynamic_cast<const TinyJSON::TJValueNumberExponent*>(jobject->try_get_value("a"));
  ASSERT_NE(nullptr, valuea);
  ASSERT_STREQ("-2.00001e+24", valuea->dump(TinyJSON::formating::none));

  delete json;
}

TEST(TestExponents, NegativeExponentNumberCannotBeConvertedToWholeNumber) {
  auto json = TinyJSON::TJ::parse(R"(
{
  "a" : 12.00001e-24,
  "b" : 10.00001000000000000001e-18
}
)"
);
  ASSERT_NE(nullptr, json);
  auto jobject = dynamic_cast<TinyJSON::TJValueObject*>(json);
  ASSERT_NE(nullptr, jobject);

  auto valuea = dynamic_cast<const TinyJSON::TJValueNumberExponent*>(jobject->try_get_value("a"));
  ASSERT_NE(nullptr, valuea);
  ASSERT_STREQ("1.200001e-23", valuea->dump(TinyJSON::formating::none));

  auto valueb = dynamic_cast<const TinyJSON::TJValueNumberExponent*>(jobject->try_get_value("b"));
  ASSERT_NE(nullptr, valueb);
  ASSERT_STREQ("1.000001000000000000001e-17", valueb->dump(TinyJSON::formating::none));

  delete json;
}

TEST(TestExponents, NegativeExponentNumberShiftsEnoughToBecomeANumberAgain) {
  auto json = TinyJSON::TJ::parse(R"(
{
  "a" : 1.00001e-24
}
)"
);
  ASSERT_NE(nullptr, json);
  auto jobject = dynamic_cast<TinyJSON::TJValueObject*>(json);
  ASSERT_NE(nullptr, jobject);

  auto valuea = dynamic_cast<const TinyJSON::TJValueNumberExponent*>(jobject->try_get_value("a"));
  ASSERT_NE(nullptr, valuea);
  ASSERT_STREQ("1.00001e-24", valuea->dump(TinyJSON::formating::none));

  delete json;
}

TEST(TestExponents, NegativeExponentNumberWithZeroWholeNumberShiftsEnoughToBecomeANumberAgain) {
  auto json = TinyJSON::TJ::parse(R"(
{
  "a" : 0.00003000000000000002e-18
}
)"
);
  ASSERT_NE(nullptr, json);
  auto jobject = dynamic_cast<TinyJSON::TJValueObject*>(json);
  ASSERT_NE(nullptr, jobject);

  auto valuea = dynamic_cast<const TinyJSON::TJValueNumberExponent*>(jobject->try_get_value("a"));
  ASSERT_NE(nullptr, valuea);
  ASSERT_STREQ("3.000000000000002e-23", valuea->dump(TinyJSON::formating::none));

  delete json;
}

TEST(TestExponents, FractionShiftExactlyToTheLeft) {
  auto json = TinyJSON::TJ::parse(R"(
{
  "a" : 0.123e3
}
)"
);
  ASSERT_NE(nullptr, json);
  auto jobject = dynamic_cast<TinyJSON::TJValueObject*>(json);
  ASSERT_NE(nullptr, jobject);

  auto value = dynamic_cast<const TinyJSON::TJValueNumberInt*>(jobject->try_get_value("a"));
  ASSERT_NE(nullptr, value);
  ASSERT_EQ(123, value->get_number());

  delete json;
}

TEST(TestExponents, FractionShiftToTheLeftLessThanTheNumberOfFractions) {
  auto json = TinyJSON::TJ::parse(R"(
{
  "a" : 0.123e2
}
)"
);
  ASSERT_NE(nullptr, json);
  auto jobject = dynamic_cast<TinyJSON::TJValueObject*>(json);
  ASSERT_NE(nullptr, jobject);

  auto value = dynamic_cast<const TinyJSON::TJValueNumberFloat*>(jobject->try_get_value("a"));
  ASSERT_NE(nullptr, value);
  ASSERT_EQ(12.3, value->get_number());

  delete json;
}

TEST(TestExponents, FractionShiftToTheLeftLessThanTheNumberOfFractionsButHasNoLeadingZeros) {
  auto json = TinyJSON::TJ::parse(R"(
{
  "a" : 0.123e2
}
)"
);
  ASSERT_NE(nullptr, json);
  auto jobject = dynamic_cast<TinyJSON::TJValueObject*>(json);
  ASSERT_NE(nullptr, jobject);

  auto value = dynamic_cast<const TinyJSON::TJValueNumberFloat*>(jobject->try_get_value("a"));
  ASSERT_NE(nullptr, value);
  ASSERT_EQ(12.3, value->get_number());

  delete json;
}

TEST(TestExponents, FractionShiftToTheLeftLessThanTheTotalNumberOfFractionsButHasLeadingZeros) {
  auto json = TinyJSON::TJ::parse(R"(
{
  "a" : 0.0012e2,
  "b" : 0.00012e3,
  "c" : 0.00012e2
}
)"
);
  ASSERT_NE(nullptr, json);
  auto jobject = dynamic_cast<TinyJSON::TJValueObject*>(json);
  ASSERT_NE(nullptr, jobject);

  auto valuea = dynamic_cast<const TinyJSON::TJValueNumberFloat*>(jobject->try_get_value("a"));
  ASSERT_NE(nullptr, valuea);
  ASSERT_EQ(0.12, valuea->get_number());

  auto valueb = dynamic_cast<const TinyJSON::TJValueNumberFloat*>(jobject->try_get_value("b"));
  ASSERT_NE(nullptr, valueb);
  ASSERT_EQ(0.12, valueb->get_number());

  auto valuec = dynamic_cast<const TinyJSON::TJValueNumberFloat*>(jobject->try_get_value("c"));
  ASSERT_NE(nullptr, valuec);
  ASSERT_EQ(0.012, valuec->get_number());

  delete json;
}

TEST(TestExponents, FractionShiftToTheLeftLessThanTheNumberOfFractionsButHasLeadingZeros) {
  auto json = TinyJSON::TJ::parse(R"(
{
  "a" : 0.0123e3
}
)"
);
  ASSERT_NE(nullptr, json);
  auto jobject = dynamic_cast<TinyJSON::TJValueObject*>(json);
  ASSERT_NE(nullptr, jobject);

  auto value = dynamic_cast<const TinyJSON::TJValueNumberFloat*>(jobject->try_get_value("a"));
  ASSERT_NE(nullptr, value);
  ASSERT_EQ(12.3, value->get_number());

  delete json;
}
