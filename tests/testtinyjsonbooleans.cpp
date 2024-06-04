// Licensed to Florent Guelfucci under one or more agreements.
// Florent Guelfucci licenses this file to you under the MIT license.
// See the LICENSE file in the project root for more information.
#include <gtest/gtest.h>
#include "../src/TinyJSON.h"

TEST(TestBooleans, BooleanIsAfterMissingColon) {
  auto json = TinyJSON::TinyJSON::parse(R"(
    {
      "a" true
    }
    )"
  );
  ASSERT_EQ(nullptr, json);
}

TEST(TestBooleans, CheckForTrue) {
  auto json = TinyJSON::TinyJSON::parse(R"(
{
  "a" : true
}
)"
);
  ASSERT_NE(nullptr, json);
  auto jobject = dynamic_cast<TinyJSON::TJValueObject*>(json);
  ASSERT_NE(nullptr, jobject);

  EXPECT_STREQ(jobject->try_get_string("a"), "true");
  ASSERT_NE(nullptr, jobject->try_get_value("a"));

  auto boolean = dynamic_cast<const TinyJSON::TJValueBoolean*>(jobject->try_get_value("a"));
  ASSERT_NE(nullptr, boolean);
  ASSERT_TRUE(boolean->is_true());

  delete json;
}

TEST(TestBooleans, CheckForFalse) {
  auto json = TinyJSON::TinyJSON::parse(R"(
{
  "a" : false
}
)"
);
  ASSERT_NE(nullptr, json);
  auto jobject = dynamic_cast<TinyJSON::TJValueObject*>(json);
  ASSERT_NE(nullptr, jobject);

  EXPECT_STREQ(jobject->try_get_string("a"), "false");
  ASSERT_NE(nullptr, jobject->try_get_value("a"));

  auto boolean = dynamic_cast<const TinyJSON::TJValueBoolean*>(jobject->try_get_value("a"));
  ASSERT_NE(nullptr, boolean);
  ASSERT_FALSE(boolean->is_true());

  delete json;
}

TEST(TestBooleans, TrueBooleanNotSpelledProperly1) {
  auto json = TinyJSON::TinyJSON::parse(R"(
    {
      "a" : t
    }
    )"
  );
  ASSERT_EQ(nullptr, json);
}

TEST(TestBooleans, TrueBooleanNotSpelledProperly2) {
  auto json = TinyJSON::TinyJSON::parse(R"(
    {
      "a" : tr
    }
    )"
  );
  ASSERT_EQ(nullptr, json);
}

TEST(TestBooleans, TrueBooleanNotSpelledProperly3) {
  auto json = TinyJSON::TinyJSON::parse(R"(
    {
      "a" : tru
    }
    )"
  );
  ASSERT_EQ(nullptr, json);
}

TEST(TestBooleans, FalseBooleanNotSpelledProperly1) {
  auto json = TinyJSON::TinyJSON::parse(R"(
    {
      "a" : f
    }
    )"
  );
  ASSERT_EQ(nullptr, json);
}

TEST(TestBooleans, FalseBooleanNotSpelledProperly2) {
  auto json = TinyJSON::TinyJSON::parse(R"(
    {
      "a" : fa
    }
    )"
  );
  ASSERT_EQ(nullptr, json);
}

TEST(TestBooleans, FalseBooleanNotSpelledProperly3) {
  auto json = TinyJSON::TinyJSON::parse(R"(
    {
      "a" : fal
    }
    )"
  );
  ASSERT_EQ(nullptr, json);
}

TEST(TestBooleans, FalseBooleanNotSpelledProperly4) {
  auto json = TinyJSON::TinyJSON::parse(R"(
    {
      "a" : fals
    }
    )"
  );
  ASSERT_EQ(nullptr, json);
}
