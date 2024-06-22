// Licensed to Florent Guelfucci under one or more agreements.
// Florent Guelfucci licenses this file to you under the MIT license.
// See the LICENSE file in the project root for more information.
#include <gtest/gtest.h>
#define TJ_USE_CHAR 1
#include "../src/TinyJSON.h"

TEST(TestException, IfWeHaveNoExceptionWeDoNotThrow) {
  TinyJSON::options options = {};
  options.throw_exception = true;
  TinyJSON::TJValue* json = nullptr;
  EXPECT_NO_THROW( json = TinyJSON::TinyJSON::parse("[12,13,14]", options));
  delete json;
}

TEST(TestException, ParseExceptionMessageIsSetProperly) {
  TinyJSON::options options = {};
  options.throw_exception = true;
  TinyJSON::TJValue* json = nullptr;
  EXPECT_NO_THROW(json = TinyJSON::TinyJSON::parse("[12,13,14]", options));
  delete json;
}

TEST(TestException, SourceCannotBeNull) {
  TinyJSON::TJParseException ex("Hello");
  EXPECT_STREQ("Hello", ex.what());
}

TEST(TestException, OperatorCopyParseException) {
  TinyJSON::TJParseException ex1("Hello");
  TinyJSON::TJParseException ex2("World");
  ex2 = ex1;
  EXPECT_STREQ("Hello", ex2.what());
}

TEST(TestException, CopyConstructorParseException) {
  TinyJSON::TJParseException ex1("Hello");
  TinyJSON::TJParseException ex2(ex1);
  EXPECT_STREQ("Hello", ex2.what());
}


TEST(TestException, EscapedTabCharacterInString) {
  TinyJSON::options options = {};
  options.throw_exception = true;
  EXPECT_THROW(TinyJSON::TinyJSON::parse("[\"Tab\tin string\"]", options), TinyJSON::TJParseException);
}

TEST(TestException, EscapedReturnCharacterInString) {
  TinyJSON::options options = {};
  options.throw_exception = true;
  EXPECT_THROW(TinyJSON::TinyJSON::parse("[\"Return\rin string\"]", options), TinyJSON::TJParseException);
}

TEST(TestException, EscapedLineFeedCharacterInString) {
  TinyJSON::options options = {};
  options.throw_exception = true;
  EXPECT_THROW(TinyJSON::TinyJSON::parse("[\"Line Feed\nin string\"]", options), TinyJSON::TJParseException);
}

TEST(TestException, EscapedFormFeedCharacterInString) {
  TinyJSON::options options = {};
  options.throw_exception = true;
  EXPECT_THROW(TinyJSON::TinyJSON::parse("[\"Form Feed\fin string\"]", options), TinyJSON::TJParseException);
}

TEST(TestException, EscapedBackSpaceCharacterInString) {
  TinyJSON::options options = {};
  options.throw_exception = true;
  EXPECT_THROW(TinyJSON::TinyJSON::parse("[\"BackSpace\bin string\"]", options), TinyJSON::TJParseException);
}

TEST(TestException, SingleEscapeCharacterInString) {
  TinyJSON::options options = {};
  options.throw_exception = true;
  EXPECT_THROW(TinyJSON::TinyJSON::parse("[\"Single Escape \\ \"]", options), TinyJSON::TJParseException);
}

TEST(TestException, TheStringIsNotClosed) {
  TinyJSON::options options = {};
  options.throw_exception = true;
  EXPECT_THROW(TinyJSON::TinyJSON::parse("\"Not Closedd", options), TinyJSON::TJParseException);
}

TEST(TestException, CommaAtTheEndOfObject) {
  TinyJSON::options options = {};
  options.throw_exception = true;
  EXPECT_THROW(TinyJSON::TinyJSON::parse("{\"a\" : 12,}", options), TinyJSON::TJParseException);
}

TEST(TestException, CommaAtTheEndOfArray) {
  TinyJSON::options options = {};
  options.throw_exception = true;
  EXPECT_THROW(TinyJSON::TinyJSON::parse("[12,]", options), TinyJSON::TJParseException);
}

TEST(TestException, UnExpectedCommaInObject) {
  TinyJSON::options options = {};
  options.throw_exception = true;
  EXPECT_THROW(TinyJSON::TinyJSON::parse("{,}", options), TinyJSON::TJParseException);
}

TEST(TestException, UnExpectedCommaInArray) {
  TinyJSON::options options = {};
  options.throw_exception = true;
  EXPECT_THROW(TinyJSON::TinyJSON::parse("[,]", options), TinyJSON::TJParseException);
}

TEST(TestException, MissingCommaBetweenItemsInObject) {
  TinyJSON::options options = {};
  options.throw_exception = true;
  EXPECT_THROW(TinyJSON::TinyJSON::parse("{\"a\" : 12 \"b\" : 12}", options), TinyJSON::TJParseException);
}

TEST(TestException, MissingCommaBetweenItemsInArray) {
  TinyJSON::options options = {};
  options.throw_exception = true;
  EXPECT_THROW(TinyJSON::TinyJSON::parse("[12 \"a\"]", options), TinyJSON::TJParseException);
}

TEST(TestException, UnExpectedCharacterInObject) {
  TinyJSON::options options = {};
  options.throw_exception = true;
  EXPECT_THROW(TinyJSON::TinyJSON::parse("{ % }", options), TinyJSON::TJParseException);
}

TEST(TestException, UnExpectedEndOfStringWhileParsingObject) {
  TinyJSON::options options = {};
  options.throw_exception = true;
  EXPECT_THROW(TinyJSON::TinyJSON::parse("{", options), TinyJSON::TJParseException);
}

TEST(TestException, UnExpectedEndOfStringWhileParsingArray) {
  TinyJSON::options options = {};
  options.throw_exception = true;
  EXPECT_THROW(TinyJSON::TinyJSON::parse("[", options), TinyJSON::TJParseException);
}

TEST(TestException, BadTrue) {
  TinyJSON::options options = {};
  options.throw_exception = true;
  EXPECT_THROW(TinyJSON::TinyJSON::parse("[tru]", options), TinyJSON::TJParseException);
}

TEST(TestException, BadFalse) {
  TinyJSON::options options = {};
  options.throw_exception = true;
  EXPECT_THROW(TinyJSON::TinyJSON::parse("[fals]", options), TinyJSON::TJParseException);
}

TEST(TestException, BadNull) {
  TinyJSON::options options = {};
  options.throw_exception = true;
  EXPECT_THROW(TinyJSON::TinyJSON::parse("[nul]", options), TinyJSON::TJParseException);
}

TEST(TestException, MissingColonInObject) {
  TinyJSON::options options = {};
  options.throw_exception = true;
  EXPECT_THROW(TinyJSON::TinyJSON::parse(R"({ "Missing colon" null })", options), TinyJSON::TJParseException);
}

TEST(TestException, InvalidNumberWithLeadingZero) {
  TinyJSON::options options = {};
  options.throw_exception = true;
  EXPECT_THROW(TinyJSON::TinyJSON::parse("[0123]", options), TinyJSON::TJParseException);
}

TEST(TestException, ExponentsWithZeroDoNotThrow) {
  TinyJSON::options options = {};
  options.throw_exception = true;
  TinyJSON::TJValue* json = nullptr;
  EXPECT_NO_THROW(json = TinyJSON::TinyJSON::parse("[1e00]", options));
  delete json;
}

TEST(TestException, WeReachedMaxDepthOfObjects) {
  TinyJSON::options options = {};
  options.throw_exception = true;
  options.max_depth = 4;
  TinyJSON::TJValue* json = nullptr;
  EXPECT_THROW(json = TinyJSON::TinyJSON::parse(R"({"a":{"b":{"c":{}}}})", options), TinyJSON::TJParseException);
  delete json;
}

TEST(TestException, WeReachedMaxDepthOfArrays) {
  TinyJSON::options options = {};
  options.throw_exception = true;
  options.max_depth = 4;
  TinyJSON::TJValue* json = nullptr;
  EXPECT_THROW(json = TinyJSON::TinyJSON::parse("[12,[13,[14,[]]]]", options), TinyJSON::TJParseException);
  delete json;
}

TEST(TestException, WeReachedMaxDepthMixed) {
  TinyJSON::options options = {};
  options.throw_exception = true;
  options.max_depth = 4;
  TinyJSON::TJValue* json = nullptr;
  EXPECT_THROW(json = TinyJSON::TinyJSON::parse(R"({"a":[12,{"c":{}}]})", options), TinyJSON::TJParseException);
  delete json;
}

TEST(TestException, Rfc4627WantsAnObjectOrAnArray) {
  TinyJSON::options options = {};
  options.throw_exception = true;
  options.specification = TinyJSON::options::rfc4627;
  TinyJSON::TJValue* json = nullptr;
  EXPECT_THROW(json = TinyJSON::TinyJSON::parse("true", options), TinyJSON::TJParseException);
  delete json;
}

TEST(TestException, WeCannotHaveMoreThanOneItemInRoot) {
  TinyJSON::options options = {};
  options.throw_exception = true;
  EXPECT_THROW(TinyJSON::TinyJSON::parse("{},[]", options), TinyJSON::TJParseException);
}

TEST(TestException, UnexpectedTokenWhileLookingForValue) {
  TinyJSON::options options = {};
  options.throw_exception = true;
  EXPECT_THROW(TinyJSON::TinyJSON::parse(R"({"a" : Value)", options), TinyJSON::TJParseException);
}

TEST(TestException, ExponentDoesNotHaveANumber) {
  TinyJSON::options options = {};
  options.throw_exception = true;
  EXPECT_THROW(TinyJSON::TinyJSON::parse("[0e]", options), TinyJSON::TJParseException);
}

TEST(TestException, ExponentDoesNotHaveANumberButHasNegativeSign) {
  TinyJSON::options options = {};
  options.throw_exception = true;
  EXPECT_THROW(TinyJSON::TinyJSON::parse("[0e-]", options), TinyJSON::TJParseException);
}

TEST(TestException, ExponentDoesNotHaveANumberButHasPositiveSign) {
  TinyJSON::options options = {};
  options.throw_exception = true;
  EXPECT_THROW(TinyJSON::TinyJSON::parse("[0e+]", options), TinyJSON::TJParseException);
}

TEST(TestException, FractionIsMissingNumberBeforExponent) {
  TinyJSON::options options = {};
  options.throw_exception = true;
  EXPECT_THROW(TinyJSON::TinyJSON::parse("12.e2", options), TinyJSON::TJParseException);
}

TEST(TestException, FractionIsMissingNumber) {
  TinyJSON::options options = {};
  options.throw_exception = true;
  EXPECT_THROW(TinyJSON::TinyJSON::parse("12.", options), TinyJSON::TJParseException);
}