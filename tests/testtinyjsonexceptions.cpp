// Licensed to Florent Guelfucci under one or more agreements.
// Florent Guelfucci licenses this file to you under the MIT license.
// See the LICENSE file in the project root for more information.
#include <gtest/gtest.h>
#define TJ_USE_CHAR 1
#include "../src/TinyJSON.h"

TEST(TestException, IfWeHaveNoExceptionWeDoNotThrow) {
  TinyJSON::parse_options options = {};
  options.throw_exception = true;
  TinyJSON::TJValue* json = nullptr;
  EXPECT_NO_THROW( json = TinyJSON::TJ::parse("[12,13,14]", options));
  delete json;
}

TEST(TestException, ParseExceptionMessageIsSetProperly) {
  TinyJSON::parse_options options = {};
  options.throw_exception = true;
  TinyJSON::TJValue* json = nullptr;
  EXPECT_NO_THROW(json = TinyJSON::TJ::parse("[12,13,14]", options));
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
  TinyJSON::parse_options options = {};
  options.throw_exception = true;
  EXPECT_THROW(TinyJSON::TJ::parse("[\"Tab\tin string\"]", options), TinyJSON::TJParseException);
}

TEST(TestException, EscapedReturnCharacterInString) {
  TinyJSON::parse_options options = {};
  options.throw_exception = true;
  EXPECT_THROW(TinyJSON::TJ::parse("[\"Return\rin string\"]", options), TinyJSON::TJParseException);
}

TEST(TestException, EscapedLineFeedCharacterInString) {
  TinyJSON::parse_options options = {};
  options.throw_exception = true;
  EXPECT_THROW(TinyJSON::TJ::parse("[\"Line Feed\nin string\"]", options), TinyJSON::TJParseException);
}

TEST(TestException, EscapedFormFeedCharacterInString) {
  TinyJSON::parse_options options = {};
  options.throw_exception = true;
  EXPECT_THROW(TinyJSON::TJ::parse("[\"Form Feed\fin string\"]", options), TinyJSON::TJParseException);
}

TEST(TestException, EscapedBackSpaceCharacterInString) {
  TinyJSON::parse_options options = {};
  options.throw_exception = true;
  EXPECT_THROW(TinyJSON::TJ::parse("[\"BackSpace\bin string\"]", options), TinyJSON::TJParseException);
}

TEST(TestException, SingleEscapeCharacterInString) {
  TinyJSON::parse_options options = {};
  options.throw_exception = true;
  EXPECT_THROW(TinyJSON::TJ::parse("[\"Single Escape \\ \"]", options), TinyJSON::TJParseException);
}

TEST(TestException, TheStringIsNotClosed) {
  TinyJSON::parse_options options = {};
  options.throw_exception = true;
  EXPECT_THROW(TinyJSON::TJ::parse("\"Not Closedd", options), TinyJSON::TJParseException);
}

TEST(TestException, CommaAtTheEndOfObject) {
  TinyJSON::parse_options options = {};
  options.throw_exception = true;
  EXPECT_THROW(TinyJSON::TJ::parse("{\"a\" : 12,}", options), TinyJSON::TJParseException);
}

TEST(TestException, CommaAtTheEndOfArray) {
  TinyJSON::parse_options options = {};
  options.throw_exception = true;
  EXPECT_THROW(TinyJSON::TJ::parse("[12,]", options), TinyJSON::TJParseException);
}

TEST(TestException, UnExpectedCommaInObject) {
  TinyJSON::parse_options options = {};
  options.throw_exception = true;
  EXPECT_THROW(TinyJSON::TJ::parse("{,}", options), TinyJSON::TJParseException);
}

TEST(TestException, UnExpectedCommaInArray) {
  TinyJSON::parse_options options = {};
  options.throw_exception = true;
  EXPECT_THROW(TinyJSON::TJ::parse("[,]", options), TinyJSON::TJParseException);
}

TEST(TestException, MissingCommaBetweenItemsInObject) {
  TinyJSON::parse_options options = {};
  options.throw_exception = true;
  EXPECT_THROW(TinyJSON::TJ::parse("{\"a\" : 12 \"b\" : 12}", options), TinyJSON::TJParseException);
}

TEST(TestException, MissingCommaBetweenItemsInArray) {
  TinyJSON::parse_options options = {};
  options.throw_exception = true;
  EXPECT_THROW(TinyJSON::TJ::parse("[12 \"a\"]", options), TinyJSON::TJParseException);
}

TEST(TestException, UnExpectedCharacterInObject) {
  TinyJSON::parse_options options = {};
  options.throw_exception = true;
  EXPECT_THROW(TinyJSON::TJ::parse("{ % }", options), TinyJSON::TJParseException);
}

TEST(TestException, UnExpectedEndOfStringWhileParsingObject) {
  TinyJSON::parse_options options = {};
  options.throw_exception = true;
  EXPECT_THROW(TinyJSON::TJ::parse("{", options), TinyJSON::TJParseException);
}

TEST(TestException, UnExpectedEndOfStringWhileParsingArray) {
  TinyJSON::parse_options options = {};
  options.throw_exception = true;
  EXPECT_THROW(TinyJSON::TJ::parse("[", options), TinyJSON::TJParseException);
}

TEST(TestException, BadTrue) {
  TinyJSON::parse_options options = {};
  options.throw_exception = true;
  EXPECT_THROW(TinyJSON::TJ::parse("[tru]", options), TinyJSON::TJParseException);
}

TEST(TestException, BadFalse) {
  TinyJSON::parse_options options = {};
  options.throw_exception = true;
  EXPECT_THROW(TinyJSON::TJ::parse("[fals]", options), TinyJSON::TJParseException);
}

TEST(TestException, BadNull) {
  TinyJSON::parse_options options = {};
  options.throw_exception = true;
  EXPECT_THROW(TinyJSON::TJ::parse("[nul]", options), TinyJSON::TJParseException);
}

TEST(TestException, MissingColonInObject) {
  TinyJSON::parse_options options = {};
  options.throw_exception = true;
  EXPECT_THROW(TinyJSON::TJ::parse(R"({ "Missing colon" null })", options), TinyJSON::TJParseException);
}

TEST(TestException, InvalidNumberWithLeadingZero) {
  TinyJSON::parse_options options = {};
  options.throw_exception = true;
  EXPECT_THROW(TinyJSON::TJ::parse("[0123]", options), TinyJSON::TJParseException);
}

TEST(TestException, ExponentsWithZeroDoNotThrow) {
  TinyJSON::parse_options options = {};
  options.throw_exception = true;
  TinyJSON::TJValue* json = nullptr;
  EXPECT_NO_THROW(json = TinyJSON::TJ::parse("[1e00]", options));
  delete json;
}

TEST(TestException, WeReachedMaxDepthOfObjects) {
  TinyJSON::parse_options options = {};
  options.throw_exception = true;
  options.max_depth = 4;
  TinyJSON::TJValue* json = nullptr;
  EXPECT_THROW(json = TinyJSON::TJ::parse(R"({"a":{"b":{"c":{}}}})", options), TinyJSON::TJParseException);
  delete json;
}

TEST(TestException, WeReachedMaxDepthOfArrays) {
  TinyJSON::parse_options options = {};
  options.throw_exception = true;
  options.max_depth = 4;
  TinyJSON::TJValue* json = nullptr;
  EXPECT_THROW(json = TinyJSON::TJ::parse("[12,[13,[14,[]]]]", options), TinyJSON::TJParseException);
  delete json;
}

TEST(TestException, WeReachedMaxDepthMixed) {
  TinyJSON::parse_options options = {};
  options.throw_exception = true;
  options.max_depth = 4;
  TinyJSON::TJValue* json = nullptr;
  EXPECT_THROW(json = TinyJSON::TJ::parse(R"({"a":[12,{"c":{}}]})", options), TinyJSON::TJParseException);
  delete json;
}

TEST(TestException, Rfc4627WantsAnObjectOrAnArray) {
  TinyJSON::parse_options options = {};
  options.throw_exception = true;
  options.specification = TinyJSON::parse_options::rfc4627;
  TinyJSON::TJValue* json = nullptr;
  EXPECT_THROW(json = TinyJSON::TJ::parse("true", options), TinyJSON::TJParseException);
  delete json;
}

TEST(TestException, WeCannotHaveMoreThanOneItemInRoot) {
  TinyJSON::parse_options options = {};
  options.throw_exception = true;
  EXPECT_THROW(TinyJSON::TJ::parse("{},[]", options), TinyJSON::TJParseException);
}

TEST(TestException, UnexpectedTokenWhileLookingForValue) {
  TinyJSON::parse_options options = {};
  options.throw_exception = true;
  EXPECT_THROW(TinyJSON::TJ::parse(R"({"a" : Value)", options), TinyJSON::TJParseException);
}

TEST(TestException, ExponentDoesNotHaveANumber) {
  TinyJSON::parse_options options = {};
  options.throw_exception = true;
  EXPECT_THROW(TinyJSON::TJ::parse("[0e]", options), TinyJSON::TJParseException);
}

TEST(TestException, ExponentDoesNotHaveANumberButHasNegativeSign) {
  TinyJSON::parse_options options = {};
  options.throw_exception = true;
  EXPECT_THROW(TinyJSON::TJ::parse("[0e-]", options), TinyJSON::TJParseException);
}

TEST(TestException, ExponentDoesNotHaveANumberButHasPositiveSign) {
  TinyJSON::parse_options options = {};
  options.throw_exception = true;
  EXPECT_THROW(TinyJSON::TJ::parse("[0e+]", options), TinyJSON::TJParseException);
}

TEST(TestException, FractionIsMissingNumberBeforExponent) {
  TinyJSON::parse_options options = {};
  options.throw_exception = true;
  EXPECT_THROW(TinyJSON::TJ::parse("12.e2", options), TinyJSON::TJParseException);
}

TEST(TestException, FractionIsMissingNumber) {
  TinyJSON::parse_options options = {};
  options.throw_exception = true;
  EXPECT_THROW(TinyJSON::TJ::parse("12.", options), TinyJSON::TJParseException);
}

TEST(TestException, EscapedTabCharacterInStringWillLogAndThrow) {
  bool called = false;
  TinyJSON::parse_options options = {};
  options.throw_exception = true;
  options.callback_function = [&](TinyJSON::parse_options::message_type message_type, const TJCHAR* exception_message) {
    EXPECT_EQ(message_type, TinyJSON::parse_options::fatal);
    EXPECT_TRUE(nullptr != exception_message);
    called = true;
    };
  EXPECT_THROW(TinyJSON::TJ::parse("[\"Tab\tin string\"]", options), TinyJSON::TJParseException);
  EXPECT_TRUE(called);
}

TEST(TestException, EscapedTabCharacterInStringWillLogAndNotThrow) {
  bool called = false;
  TinyJSON::parse_options options = {};
  options.throw_exception = false;
  options.callback_function = [&](TinyJSON::parse_options::message_type message_type, const TJCHAR* exception_message) {
    EXPECT_EQ(message_type, TinyJSON::parse_options::fatal);
    EXPECT_TRUE(nullptr != exception_message);
    called = true;
    };
  EXPECT_NO_THROW(TinyJSON::TJ::parse("[\"Tab\tin string\"]", options));
  EXPECT_TRUE(called);
}

TEST(TestException, GettingNonExistentKeyWillLogAndNotThrow) {
  bool called = false;
  TinyJSON::parse_options options = {};
  options.throw_exception = false;
  options.strict = true;
  options.callback_function = [&](TinyJSON::parse_options::message_type message_type, const TJCHAR* exception_message) {
    EXPECT_EQ(message_type, TinyJSON::parse_options::fatal);
    EXPECT_STREQ(exception_message, "The key was not found!");
    called = true;
    };
  TinyJSON::TJValue* json = TinyJSON::TJ::parse("{\"a\" : 12}", options);
  ASSERT_TRUE(json != nullptr);
  auto* object = static_cast<TinyJSON::TJValueObject*>(json);

  // try and get a non existent key
  const TJCHAR* value = nullptr;
  EXPECT_NO_THROW(value = object->get_string("b", true));
  EXPECT_TRUE(called);
  EXPECT_STREQ(value, "");
  delete json;
}