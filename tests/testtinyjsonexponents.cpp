// Licensed to Florent Guelfucci under one or more agreements.
// Florent Guelfucci licenses this file to you under the MIT license.
// See the LICENSE file in the project root for more information.
#include <gtest/gtest.h>
#define TJ_USE_CHAR 1
#include "../src/TinyJSON.h"
#include <cmath>

constexpr long double EPSILON = 1e-10;

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
  EXPECT_STREQ("1.23045e+27", valuea->dump(TinyJSON::formating::minify));

  auto valueb = dynamic_cast<const TinyJSON::TJValueNumberExponent*>(jobject->try_get_value("b"));
  ASSERT_NE(nullptr, valueb);
  EXPECT_STREQ("6.7809e+27", valueb->dump(TinyJSON::formating::minify));

  auto valuec = dynamic_cast<const TinyJSON::TJValueNumberExponent*>(jobject->try_get_value("c"));
  ASSERT_NE(nullptr, valuec);
  EXPECT_STREQ("1.0009e+27", valuec->dump(TinyJSON::formating::minify));

  auto valued = dynamic_cast<const TinyJSON::TJValueNumberExponent*>(jobject->try_get_value("d"));
  ASSERT_NE(nullptr, valued);
  EXPECT_STREQ("1.2345e+27", valued->dump(TinyJSON::formating::minify));

  delete json;
}

TEST(TestExponents, LargeExponentIsConvertedToSingleWholeNumberInAStandAloneString) {
  auto json = TinyJSON::TJ::parse("100.09e+25");

  ASSERT_NE(nullptr, json);

  auto a = dynamic_cast<const TinyJSON::TJValueNumberExponent*>(json);
  ASSERT_NE(nullptr, a);
  ASSERT_STREQ("1.0009e+27", a->dump(TinyJSON::formating::minify));
  
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
  ASSERT_STREQ("2.00001e+24", valuea->dump(TinyJSON::formating::minify));

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
  ASSERT_STREQ("-2.00001e+24", valuea->dump(TinyJSON::formating::minify));

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
  ASSERT_STREQ("1.200001e-23", valuea->dump(TinyJSON::formating::minify));

  auto valueb = dynamic_cast<const TinyJSON::TJValueNumberExponent*>(jobject->try_get_value("b"));
  ASSERT_NE(nullptr, valueb);
  ASSERT_STREQ("1.000001000000000000001e-17", valueb->dump(TinyJSON::formating::minify));

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
  ASSERT_STREQ("1.00001e-24", valuea->dump(TinyJSON::formating::minify));

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
  ASSERT_STREQ("3.000000000000002e-23", valuea->dump(TinyJSON::formating::minify));

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

TEST(TestExponents, BasicNumber)
{
  auto json = TinyJSON::TJ::parse("1.07e4");
  auto exponent = dynamic_cast<const TinyJSON::TJValueNumber*>(json);
  ASSERT_NE(nullptr, exponent);

  long double result = exponent->get_float(); // 1.07e4 = 10700.0
  EXPECT_NEAR(result, 10700.0L, EPSILON);

  delete json;
}

TEST(TestExponents, ZeroFraction)
{
  auto json = TinyJSON::TJ::parse("5.0e2");
  auto exponent = dynamic_cast<const TinyJSON::TJValueNumber*>(json);
  ASSERT_NE(nullptr, exponent);

  long double result = exponent->get_float(); // 5.0e2 = 500.0
  EXPECT_NEAR(result, 500.0L, EPSILON);

  delete json;
}

TEST(TestExponents, NegativeExponent)
{
  auto json = TinyJSON::TJ::parse("2.05e-2");
  auto exponent = dynamic_cast<const TinyJSON::TJValueNumber*>(json);
  ASSERT_NE(nullptr, exponent);

  long double result = exponent->get_float(); // 2.05e-2 = 0.0205
  EXPECT_NEAR(result, 0.0205L, EPSILON);

  delete json;
}

TEST(TestExponents, NegativeExponentWithNoFraction)
{
  auto json = TinyJSON::TJ::parse("2e-2");
  auto exponent = dynamic_cast<const TinyJSON::TJValueNumber*>(json);
  ASSERT_NE(nullptr, exponent);

  long double result = exponent->get_float(); // 2e-2 = 0.02
  EXPECT_NEAR(result, 0.02L, EPSILON);

  delete json;
}

TEST(TestExponents, NegativeExponentWithGivenZeroFraction)
{
  auto json = TinyJSON::TJ::parse("2.00e-2");
  auto exponent = dynamic_cast<const TinyJSON::TJValueNumber*>(json);
  ASSERT_NE(nullptr, exponent);

  long double result = exponent->get_float(); // 2.00e-2 = 0.02
  EXPECT_NEAR(result, 0.02L, EPSILON);

  delete json;
}

TEST(TestExponents, PositiveExponent)
{
  auto json = TinyJSON::TJ::parse("2.5e+2");
  auto exponent = dynamic_cast<const TinyJSON::TJValueNumber*>(json);
  ASSERT_NE(nullptr, exponent);

  long double result = exponent->get_float(); // 2.5e+2 = 250.00
  EXPECT_NEAR(result, 250.0L, EPSILON);

  delete json;
}

TEST(TestExponents, PositiveExponentWithNoFraction)
{
  auto json = TinyJSON::TJ::parse("2e+2");
  auto exponent = dynamic_cast<const TinyJSON::TJValueNumber*>(json);
  ASSERT_NE(nullptr, exponent);

  long double result = exponent->get_float(); // 2e+2 = 200.00
  EXPECT_NEAR(result, 200.0L, EPSILON);

  delete json;
}

TEST(TestExponents, PositiveExponentWithGivenZeroFraction)
{
  auto json = TinyJSON::TJ::parse("2.0e+2");
  auto exponent = dynamic_cast<const TinyJSON::TJValueNumber*>(json);
  ASSERT_NE(nullptr, exponent);

  long double result = exponent->get_float(); // 2.0e+2 = 200.00
  EXPECT_NEAR(result, 200.0L, EPSILON);

  delete json;
}

TEST(TestExponents, NegativeNumberPositiveExponent)
{
  auto json = TinyJSON::TJ::parse("-2.5e+2");
  auto exponent = dynamic_cast<const TinyJSON::TJValueNumber*>(json);
  ASSERT_NE(nullptr, exponent);

  long double result = exponent->get_float(); // -2.5e+2 = -250.00
  EXPECT_NEAR(result, -250.0L, EPSILON);

  delete json;
}

TEST(TestExponents, NegativeNumber)
{
  auto json = TinyJSON::TJ::parse("-3.33e1");
  auto exponent = dynamic_cast<const TinyJSON::TJValueNumber*>(json);
  ASSERT_NE(nullptr, exponent);

  long double result = exponent->get_float(); // -3.33e1 = -33.3
  EXPECT_NEAR(result, -33.3L, EPSILON);

  delete json;
}

TEST(TestExponents, Overflow)
{
  auto json = TinyJSON::TJ::parse("1e10000");
  auto exponent = dynamic_cast<const TinyJSON::TJValueNumber*>(json);
  ASSERT_NE(nullptr, exponent);
  
  long double result = exponent->get_float(); // Force overflow
  EXPECT_TRUE(std::isinf(result));

  delete json;
}

TEST(TestExponents, Underflow)
{
  auto json = TinyJSON::TJ::parse("1e-50000");
  auto exponent = dynamic_cast<const TinyJSON::TJValueNumber*>(json);
  ASSERT_NE(nullptr, exponent);

  long double result = exponent->get_float(); // Force underflow
  EXPECT_EQ(result, 0.0L);

  delete json;
}

TEST(TestExponents, NegativeExponentToString)
{
  auto json = TinyJSON::TJ::parse("2.5e-2");
  EXPECT_STREQ("0.025", json->dump(TinyJSON::formating::minify));
  delete json;
}

TEST(TestExponents, PositiveExponentToString)
{
  auto json = TinyJSON::TJ::parse("2.5e+2");
  EXPECT_STREQ("250", json->dump(TinyJSON::formating::minify));
  delete json;
}