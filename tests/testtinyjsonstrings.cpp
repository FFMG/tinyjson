// Licensed to Florent Guelfucci under one or more agreements.
// Florent Guelfucci licenses this file to you under the MIT license.
// See the LICENSE file in the project root for more information.
#include <gtest/gtest.h>
#define TJ_USE_CHAR 1
#include "../src/TinyJSON.h"

TEST(TestStrings, StringIsAfterMissingColon) {
  auto json = TinyJSON::TJ::parse(R"(
    {
      "a" "b"
    }
    )"
  );
  ASSERT_EQ(nullptr, json);
}

TEST(TestStrings, TheStringNameValueIsSaved) {
  auto json = TinyJSON::TJ::parse(R"(
{
  "Hello" : "World"
}
)"
);
  ASSERT_NE(nullptr, json);

  auto jobject = dynamic_cast<TinyJSON::TJValueObject*>(json);
  ASSERT_NE(nullptr, jobject);

  ASSERT_STREQ(jobject->try_get_string("Hello"), "World");

  delete json;
}

TEST(TestStrings, TheStringNameValueIsSavedMultiLine) {
  auto json = TinyJSON::TJ::parse(R"(
{
  "Hello" 
    : 
  "World"
}
)"
);
  ASSERT_NE(nullptr, json);
  auto jobject = dynamic_cast<TinyJSON::TJValueObject*>(json);
  ASSERT_NE(nullptr, jobject);

  ASSERT_STREQ(jobject->try_get_string("Hello"), "World");

  delete json;
}

TEST(TestStrings, TheStringNameValueIsSavedNultiplItems) {
  auto json = TinyJSON::TJ::parse(R"(
{
  "a name" : "a value",
  "b name" : "b value",
  "c name" : "c value"
}
)"
);
  ASSERT_NE(nullptr, json);
  auto jobject = dynamic_cast<TinyJSON::TJValueObject*>(json);
  ASSERT_NE(nullptr, jobject);

  ASSERT_STREQ(jobject->try_get_string("a name"), "a value");
  ASSERT_STREQ(jobject->try_get_string("b name"), "b value");
  ASSERT_STREQ(jobject->try_get_string("c name"), "c value");

  delete json;
}

TEST(TestStrings, ArrayOfString) {
  auto json = TinyJSON::TJ::parse(R"(
[
  "Hello" , "World"
]
)"
);
  ASSERT_NE(nullptr, json);

  auto jarray = dynamic_cast<TinyJSON::TJValueArray*>(json);
  ASSERT_NE(nullptr, jarray);

  ASSERT_EQ(2, jarray->get_number_of_items());
  ASSERT_STREQ("Hello", jarray->at(0)->dump_string());
  ASSERT_STREQ("World", jarray->at(1)->dump_string());

  delete json;
}

TEST(TestStrings, CheckThatValueWithAVeryLongKeyValuePair) {
  auto json = TinyJSON::TJ::parse(R"(
{
  "ThisIsALongStringThatIsNormallyLongerThanTheDefault" : "The longest word is Pneumonoultramicroscopicsilicovolcanoconiosis"
}
)"
);
  ASSERT_NE(nullptr, json);

  auto jobject = dynamic_cast<TinyJSON::TJValueObject*>(json);
  ASSERT_NE(nullptr, jobject);

  auto string_value = jobject->try_get_value("ThisIsALongStringThatIsNormallyLongerThanTheDefault");
  ASSERT_NE(nullptr, string_value);
  ASSERT_STREQ("The longest word is Pneumonoultramicroscopicsilicovolcanoconiosis", string_value->dump_string());
  ASSERT_TRUE(string_value->is_string());

  delete json;
}

TEST(TestStrings, CheckThatValueIsString) {
  auto json = TinyJSON::TJ::parse(R"(
{
  "Hello" : "World"
}
)"
);
  ASSERT_NE(nullptr, json);

  auto jobject = dynamic_cast<TinyJSON::TJValueObject*>(json);
  ASSERT_NE(nullptr, jobject);

  auto string_value = jobject->try_get_value("Hello");
  ASSERT_NE(nullptr, string_value);

  ASSERT_FALSE(string_value->is_object());
  ASSERT_FALSE(string_value->is_array());
  ASSERT_TRUE(string_value->is_string());
  ASSERT_FALSE(string_value->is_number());
  ASSERT_FALSE(string_value->is_true());
  ASSERT_FALSE(string_value->is_false());
  ASSERT_FALSE(string_value->is_null());

  delete json;
}

TEST(TestStrings, CheckThatValueIsStringInArray) {
  auto json = TinyJSON::TJ::parse(R"(
[
  "Hello", "World"
]
)"
);
  ASSERT_NE(nullptr, json);

  auto tjarray = dynamic_cast<TinyJSON::TJValueArray*>(json);
  ASSERT_NE(nullptr, tjarray);

  auto string_value = tjarray->at(0);
  ASSERT_NE(nullptr, string_value);

  ASSERT_FALSE(string_value->is_object());
  ASSERT_FALSE(string_value->is_array());
  ASSERT_TRUE(string_value->is_string());
  ASSERT_FALSE(string_value->is_number());
  ASSERT_FALSE(string_value->is_true());
  ASSERT_FALSE(string_value->is_false());
  ASSERT_FALSE(string_value->is_null());

  delete json;
}

TEST(TestStrings, DifferentEscapeTypes) {
  auto json = TinyJSON::TJ::parse(R"(
[
  "\\\"Hello\\\"",
  "\/\"Hello\/\""
]
)"
);
  ASSERT_NE(nullptr, json);

  auto jarray = dynamic_cast<TinyJSON::TJValueArray*>(json);
  ASSERT_NE(nullptr, jarray);

  ASSERT_STREQ(R"(\"Hello\")", jarray->at(0)->dump_string());
  ASSERT_STREQ(R"(/"Hello/")", jarray->at(1)->dump_string());

  delete json;
}

TEST(TestStrings, EscapeQuoteInString) {
  auto json = TinyJSON::TJ::parse(R"(
[
  "\\\"Escape Then quote\\\"",
  "\"Quote\""
]
)"
);
  ASSERT_NE(nullptr, json);

  auto jarray = dynamic_cast<TinyJSON::TJValueArray*>(json);
  ASSERT_NE(nullptr, jarray);

  ASSERT_STREQ(R"(\"Escape Then quote\")", jarray->at(0)->dump_string());
  ASSERT_STREQ(R"("Quote")", jarray->at(1)->dump_string());

  delete json;
}

TEST(TestStrings, EscapeFormFeedInString) {
  auto json = TinyJSON::TJ::parse(R"(
[
  "This is a \fA Form feed"
]
)"
);
  ASSERT_NE(nullptr, json);

  auto jarray = dynamic_cast<TinyJSON::TJValueArray*>(json);
  ASSERT_NE(nullptr, jarray);

  ASSERT_STREQ("This is a \fA Form feed", jarray->at(0)->dump_string());

  delete json;
}

TEST(TestStrings, EscapeBackSpaceInString) {
  auto json = TinyJSON::TJ::parse(R"(
[
  "This is a \bA backspace"
]
)"
);
  ASSERT_NE(nullptr, json);

  auto jarray = dynamic_cast<TinyJSON::TJValueArray*>(json);
  ASSERT_NE(nullptr, jarray);

  ASSERT_STREQ("This is a \bA backspace", jarray->at(0)->dump_string());

  delete json;
}

TEST(TestStrings, EscapeNewLineInString) {
  auto json = TinyJSON::TJ::parse(R"(
[
  "This is a \nNew Line"
]
)"
);
  ASSERT_NE(nullptr, json);

  auto jarray = dynamic_cast<TinyJSON::TJValueArray*>(json);
  ASSERT_NE(nullptr, jarray);

  ASSERT_STREQ("This is a \nNew Line", jarray->at(0)->dump_string());

  delete json;
}

TEST(TestStrings, EscapeCarriageReturnInString) {
  auto json = TinyJSON::TJ::parse(R"(
[
  "This is a \rCarriage return"
]
)"
);
  ASSERT_NE(nullptr, json);

  auto jarray = dynamic_cast<TinyJSON::TJValueArray*>(json);
  ASSERT_NE(nullptr, jarray);

  ASSERT_STREQ("This is a \rCarriage return", jarray->at(0)->dump_string());

  delete json;
}

TEST(TestStrings, EscapeTabInString) {
  auto json = TinyJSON::TJ::parse(R"(
[
  "This is a \tTab"
]
)"
);
  ASSERT_NE(nullptr, json);

  auto jarray = dynamic_cast<TinyJSON::TJValueArray*>(json);
  ASSERT_NE(nullptr, jarray);

  ASSERT_STREQ("This is a \tTab", jarray->at(0)->dump_string());

  delete json;
}

TEST(TestStrings, EscapeQuoteInStringKeyValuePair) {
  auto json = TinyJSON::TJ::parse(R"(
{
  "\\\"Hello\\\"" : "\"World\""
}
)"
);
  ASSERT_NE(nullptr, json);

  auto tjobject = dynamic_cast<TinyJSON::TJValueObject*>(json);
  ASSERT_NE(nullptr, tjobject);

  // the name should be escaped.
  ASSERT_STREQ(R"("World")", tjobject->try_get_string(R"(\"Hello\")"));
  delete json;
}

TEST(TestStrings, TheLastItemInOurBrokenJsonIsAnEscape) {
  auto json = TinyJSON::TJ::parse(R"(
[
"test1"
"tes2\)"
);
  ASSERT_EQ(nullptr, json);
  delete json;
}

TEST(TestStrings, ADumpedStringWithAReverseSolidusKeepsReverseSolidus) {
  auto json = TinyJSON::TJ::parse(R"("This is a string.\\And this is after a reverse solidus")");
  ASSERT_NE(nullptr, json);

  const auto& text = json->dump_string();
  ASSERT_NE(nullptr, text);

  ASSERT_STREQ("This is a string.\\And this is after a reverse solidus", text);
  delete json;
}

TEST(TestStrings, ADumpedStringWithABackSpaceKeepsTheBackSpace) {
  auto json = TinyJSON::TJ::parse(R"("This is a string.\bAnd this is after a backspace")");
  ASSERT_NE(nullptr, json);

  const auto& text = json->dump_string();
  ASSERT_NE(nullptr, text);

  ASSERT_STREQ("This is a string.\bAnd this is after a backspace", text);
  delete json;
}

TEST(TestStrings, ADumpedStringWithAFormFeedKeepsTheFormFeed) {
  auto json = TinyJSON::TJ::parse(R"("This is a string.\fAnd this is after a form feed")");
  ASSERT_NE(nullptr, json);

  const auto& text = json->dump_string();
  ASSERT_NE(nullptr, text);

  ASSERT_STREQ("This is a string.\fAnd this is after a form feed", text);
  delete json;
}

TEST(TestStrings, ADumpedStringWithANewLineKeepsTheNewLine) {
  auto json = TinyJSON::TJ::parse(R"("This is a string.\nAnd this is a new line")");
  ASSERT_NE(nullptr, json);

  const auto& text = json->dump_string();
  ASSERT_NE(nullptr, text);

  ASSERT_STREQ("This is a string.\nAnd this is a new line", text);
  delete json;
}

TEST(TestStrings, ADumpedStringWithACarriageReturnKeepsTheCarriageReturn) {
  auto json = TinyJSON::TJ::parse(R"("This is a string.\rAnd this is after a carriage return")");
  ASSERT_NE(nullptr, json);

  const auto& text = json->dump_string();
  ASSERT_NE(nullptr, text);

  ASSERT_STREQ("This is a string.\rAnd this is after a carriage return", text);
  delete json;
}

TEST(TestStrings, ADumpedStringWithATabKeepsTheTab) {
  auto json = TinyJSON::TJ::parse(R"("This is a string.\tAnd this is after a tab")");
  ASSERT_NE(nullptr, json);

  const auto& text = json->dump_string();
  ASSERT_NE(nullptr, text);

  ASSERT_STREQ("This is a string.\tAnd this is after a tab", text);
  delete json;
}

TEST(TestStrings, ItemInAnObjectHasALineFeed) {
  auto json = TinyJSON::TJ::parse(R"(
{
  "Hello" : "Line 1\nLine2"
}
)"
);
  ASSERT_NE(nullptr, json);

  auto tjobject = dynamic_cast<TinyJSON::TJValueObject*>(json);
  ASSERT_NE(nullptr, tjobject);

  // the name should be escaped.
  ASSERT_STREQ("Line 1\nLine2", tjobject->try_get_string("Hello"));
  delete json;
}

TEST(TestStrings, ItemInAnObjectHasAFormFeed) {
  auto json = TinyJSON::TJ::parse(R"(
{
  "Hello" : "Item 1\fItem2"
}
)"
);
  ASSERT_NE(nullptr, json);

  auto tjobject = dynamic_cast<TinyJSON::TJValueObject*>(json);
  ASSERT_NE(nullptr, tjobject);

  // the name should be escaped.
  ASSERT_STREQ("Item 1\fItem2", tjobject->try_get_string("Hello"));
  delete json;
}

TEST(TestStrings, ItemInAnObjectHasACarriageReturnAndLineFeed) {
  auto json = TinyJSON::TJ::parse(R"(
{
  "Hello" : "Item 1\r\nItem2"
}
)"
);
  ASSERT_NE(nullptr, json);

  auto tjobject = dynamic_cast<TinyJSON::TJValueObject*>(json);
  ASSERT_NE(nullptr, tjobject);

  // the name should be escaped.
  ASSERT_STREQ("Item 1\r\nItem2", tjobject->try_get_string("Hello"));
  delete json;
}

TEST(TestStrings, ADumpedWithAReverseSolidusKeepsReverseSolidus) {
  auto json = TinyJSON::TJ::parse(R"("This is a string.\\And this is after a reverse solidus")");
  ASSERT_NE(nullptr, json);

  const auto& text = json->dump(TinyJSON::formating::minify);
  ASSERT_NE(nullptr, text);

  ASSERT_STREQ(R"("This is a string.\\And this is after a reverse solidus")", text);
  delete json;
}

TEST(TestStrings, ADumpedWithABackSpaceKeepsTheBackSpace) {
  auto json = TinyJSON::TJ::parse(R"("This is a string.\bAnd this is after a backspace")");
  ASSERT_NE(nullptr, json);

  const auto& text = json->dump(TinyJSON::formating::minify);
  ASSERT_NE(nullptr, text);

  ASSERT_STREQ(R"("This is a string.\bAnd this is after a backspace")", text);
  delete json;
}

TEST(TestStrings, ADumpedWithAFormFeedKeepsTheFormFeed) {
  auto json = TinyJSON::TJ::parse(R"("This is a string.\fAnd this is after a form feed")");
  ASSERT_NE(nullptr, json);

  const auto& text = json->dump(TinyJSON::formating::minify);
  ASSERT_NE(nullptr, text);

  ASSERT_STREQ(R"("This is a string.\fAnd this is after a form feed")", text);
  delete json;
}

TEST(TestStrings, ADumpedWithANewLineKeepsTheNewLine) {
  auto json = TinyJSON::TJ::parse(R"("This is a string.\nAnd this is a new line")");
  ASSERT_NE(nullptr, json);

  const auto& text = json->dump(TinyJSON::formating::minify);
  ASSERT_NE(nullptr, text);

  ASSERT_STREQ(R"("This is a string.\nAnd this is a new line")", text);
  delete json;
}

TEST(TestStrings, ADumpedWithACarriageReturnKeepsTheCarriageReturn) {
  auto json = TinyJSON::TJ::parse(R"("This is a string.\rAnd this is after a carriage return")");
  ASSERT_NE(nullptr, json);

  const auto& text = json->dump(TinyJSON::formating::minify);
  ASSERT_NE(nullptr, text);

  ASSERT_STREQ(R"("This is a string.\rAnd this is after a carriage return")", text);
  delete json;
}

TEST(TestStrings, ADumpedWithATabKeepsTheTab) {
  auto json = TinyJSON::TJ::parse(R"("This is a string.\tAnd this is after a tab")");
  ASSERT_NE(nullptr, json);

  const auto& text = json->dump(TinyJSON::formating::minify);
  ASSERT_NE(nullptr, text);

  ASSERT_STREQ(R"("This is a string.\tAnd this is after a tab")", text);
  delete json;
}

TEST(TestStrings, YouCannotHaveALineFeedInAString) {
  auto json = TinyJSON::TJ::parse(R"(
    [
      "This is
Invalid"])"
  );
  ASSERT_EQ(nullptr, json);
}

TEST(TestStrings, YouCannotHaveAFormFeedInAString) {
  auto json = TinyJSON::TJ::parse("[   \"This is \fInvalid\"]   ");
  ASSERT_EQ(nullptr, json);
}

TEST(TestStrings, YouCannotHaveASingleReverseSolidusInAString) {
  auto json = TinyJSON::TJ::parse("[\"This\\ is invalid\"]");
  ASSERT_EQ(nullptr, json);
}

TEST(TestStrings, YouCannotHaveATabInAString) {
  auto json = TinyJSON::TJ::parse("[   \"This is \\\tInvalid\"]   ");
  ASSERT_EQ(nullptr, json);
}

TEST(TestStrings, YouCanHaveATabBeforeAndAfterAString) {
  auto json = TinyJSON::TJ::parse("\t\"This is valid\"\t");
  ASSERT_NE(nullptr, json);

  const auto& text = json->dump_string();
  ASSERT_NE(nullptr, text);

  ASSERT_STREQ("This is valid", text);

  delete json;
}

TEST(TestStrings, JustControls) {
  auto json = TinyJSON::TJ::parse(R"("\b\f\n\r\t")");
  ASSERT_NE(nullptr, json);

  const auto& text = json->dump_string();
  ASSERT_NE(nullptr, text);

  ASSERT_STREQ("\b\f\n\r\t", text);

  delete json;
}

TEST(TestStrings, JustControlsInObject) {
  auto json = TinyJSON::TJ::parse(R"({"controls": "\b\f\n\r\t"})");
  ASSERT_NE(nullptr, json);

  auto jobject = dynamic_cast<TinyJSON::TJValueObject*>(json);
  ASSERT_NE(nullptr, jobject);

  auto controls = jobject->try_get_value("controls");
  ASSERT_NE(nullptr, controls);

  const auto& text = controls->dump_string();
  ASSERT_NE(nullptr, text);

  ASSERT_STREQ("\b\f\n\r\t", text);

  delete json;
}

TEST(TestStrings, JustSlash) {
  auto json = TinyJSON::TJ::parse(R"("/ & \/")");
  ASSERT_NE(nullptr, json);

  const auto& text = json->dump_string();
  ASSERT_NE(nullptr, text);

  ASSERT_STREQ("/ & /", text);

  delete json;
}

TEST(TestStrings, JustSlashInObject) {
  auto json = TinyJSON::TJ::parse(R"({"slash": "/ & \/"})");
  ASSERT_NE(nullptr, json);

  auto jobject = dynamic_cast<TinyJSON::TJValueObject*>(json);
  ASSERT_NE(nullptr, jobject);

  auto controls = jobject->try_get_value("slash");
  ASSERT_NE(nullptr, controls);

  const auto& text = controls->dump_string();
  ASSERT_NE(nullptr, text);

  ASSERT_STREQ("/ & /", text);

  delete json;
}

TEST(TestStrings, YouCanHaveATabBeforeAndAfterAStringInObject) {
  auto json = TinyJSON::TJ::parse("{\"a\" : \t\"This is valid\"\t}");
  ASSERT_NE(nullptr, json);

  auto jobject = dynamic_cast<TinyJSON::TJValueObject*>(json);
  ASSERT_NE(nullptr, jobject);

  ASSERT_NE(nullptr, jobject->try_get_value("a"));
  ASSERT_STREQ(jobject->try_get_string("a"), "This is valid");

  delete json;
}

TEST(TestStrings, ValidHexValues) {
  GTEST_SKIP();

  auto json = TinyJSON::TJ::parse(R"(
[
  "\u0123\u4567\u89AB\uCDEF\uabcd\uef4A"
]
)"
);

  ASSERT_NE(nullptr, json);

  auto jarray = dynamic_cast<TinyJSON::TJValueArray*>(json);
  ASSERT_NE(nullptr, jarray);

  ASSERT_EQ(1, jarray->get_number_of_items());
  ASSERT_STREQ("\u0123\u4567\u89AB\uCDEF\uabcd\uef4A", jarray->at(0)->dump_string());

  delete json;
}
