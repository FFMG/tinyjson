// Licensed to Florent Guelfucci under one or more agreements.
// Florent Guelfucci licenses this file to you under the MIT license.
// See the LICENSE file in the project root for more information.
#include <gtest/gtest.h>
#include "../src/TinyJSON.h"

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
  ASSERT_NE(nullptr, dynamic_cast<const TinyJSON::TJValueTrue*>(jobject->try_get_value("a")));

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
  ASSERT_NE(nullptr, dynamic_cast<const TinyJSON::TJValueFalse*>(jobject->try_get_value("a")));

  delete json;
}
