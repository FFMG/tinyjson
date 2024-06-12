// Licensed to Florent Guelfucci under one or more agreements.
// Florent Guelfucci licenses this file to you under the MIT license.
// See the LICENSE file in the project root for more information.
#include <gtest/gtest.h>
#include "../src/TinyJSON.h"

TEST(TestStrings, StringIsAfterMissingColon) {
  auto json = TinyJSON::TinyJSON::parse(R"(
    {
      "a" "b"
    }
    )"
  );
  ASSERT_EQ(nullptr, json);
}

TEST(TestStrings, TheStringNameValueIsSaved) {
  auto json = TinyJSON::TinyJSON::parse(R"(
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
  auto json = TinyJSON::TinyJSON::parse(R"(
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
  auto json = TinyJSON::TinyJSON::parse(R"(
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
  auto json = TinyJSON::TinyJSON::parse(R"(
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
  auto json = TinyJSON::TinyJSON::parse(R"(
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
  auto json = TinyJSON::TinyJSON::parse(R"(
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
  auto json = TinyJSON::TinyJSON::parse(R"(
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
  auto json = TinyJSON::TinyJSON::parse(R"(
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
  auto json = TinyJSON::TinyJSON::parse(R"(
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
  auto json = TinyJSON::TinyJSON::parse(R"(
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
  auto json = TinyJSON::TinyJSON::parse(R"(
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
  auto json = TinyJSON::TinyJSON::parse(R"(
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
  auto json = TinyJSON::TinyJSON::parse(R"(
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
  auto json = TinyJSON::TinyJSON::parse(R"(
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
  auto json = TinyJSON::TinyJSON::parse(R"(
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
  auto json = TinyJSON::TinyJSON::parse(R"(
[
"test1"
"tes2\)"
);
  ASSERT_EQ(nullptr, json);
  delete json;
}
