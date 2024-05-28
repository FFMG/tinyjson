// Licensed to Florent Guelfucci under one or more agreements.
// Florent Guelfucci licenses this file to you under the MIT license.
// See the LICENSE file in the project root for more information.
#include <gtest/gtest.h>
#include "../src/TinyJSON.h"

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

  EXPECT_STREQ(jobject->try_get_string("Hello"), "World");

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

  EXPECT_STREQ(jobject->try_get_string("Hello"), "World");

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

  EXPECT_STREQ(jobject->try_get_string("a name"), "a value");
  EXPECT_STREQ(jobject->try_get_string("b name"), "b value");
  EXPECT_STREQ(jobject->try_get_string("c name"), "c value");

  delete json;
}