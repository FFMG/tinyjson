// Licensed to Florent Guelfucci under one or more agreements.
// Florent Guelfucci licenses this file to you under the MIT license.
// See the LICENSE file in the project root for more information.
#include <gtest/gtest.h>
#include "../src/TinyJSON.h"

TEST(TestNulls, NullIsAfterMissingColon) {
  auto json = TinyJSON::TinyJSON::parse(R"(
    {
      "a" null
    }
    )"
  );
  ASSERT_EQ(nullptr, json);
}

TEST(TestNulls, CheckForNull) {
  auto json = TinyJSON::TinyJSON::parse(R"(
{
  "a" : null
}
)"
);
  ASSERT_NE(nullptr, json);
  auto jobject = dynamic_cast<TinyJSON::TJValueObject*>(json);
  ASSERT_NE(nullptr, jobject);

  EXPECT_STREQ(jobject->try_get_string("a"), "null");
  ASSERT_NE(nullptr, jobject->try_get_value("a"));

  auto tjnull = dynamic_cast<const TinyJSON::TJValueNull*>(jobject->try_get_value("a"));
  ASSERT_NE(nullptr, tjnull);

  delete json;
}

TEST(TestNulls, CheckForNullInsideAnArray) {
  auto json = TinyJSON::TinyJSON::parse(R"(
[
  null, null
]
)"
);
  ASSERT_NE(nullptr, json);
  auto jarray = dynamic_cast<TinyJSON::TJValueArray*>(json);
  ASSERT_NE(nullptr, jarray);

  auto tjnull0 = dynamic_cast<const TinyJSON::TJValueNull*>(jarray->at(0));
  ASSERT_NE(nullptr, tjnull0);

  auto tjnull1 = dynamic_cast<const TinyJSON::TJValueNull*>(jarray->at(1));
  ASSERT_NE(nullptr, tjnull1);

  delete json;
}

TEST(TestNulls, NullNotSpelledProperly1) {
  auto json = TinyJSON::TinyJSON::parse(R"(
    {
      "a" : n
    }
    )"
  );
  ASSERT_EQ(nullptr, json);
}

TEST(TestNulls, NullNotSpelledProperly2) {
  auto json = TinyJSON::TinyJSON::parse(R"(
    {
      "a" : nu
    }
    )"
  );
  ASSERT_EQ(nullptr, json);
}

TEST(TestNulls, NullNotSpelledProperly3) {
  auto json = TinyJSON::TinyJSON::parse(R"(
    {
      "a" : nul
    }
    )"
  );
  ASSERT_EQ(nullptr, json);
}

TEST(TestNulls, NullWordIsNotNull) {
  auto json = TinyJSON::TinyJSON::parse(R"(
{
  "a" : "null"
}
)"
);
  ASSERT_NE(nullptr, json);
  auto jobject = dynamic_cast<TinyJSON::TJValueObject*>(json);
  ASSERT_NE(nullptr, jobject);

  EXPECT_STREQ(jobject->try_get_string("a"), "null");
  ASSERT_NE(nullptr, jobject->try_get_value("a"));

  auto tjnull = dynamic_cast<const TinyJSON::TJValueNull*>(jobject->try_get_value("a"));
  ASSERT_EQ(nullptr, tjnull);

  auto tjstring = dynamic_cast<const TinyJSON::TJValueString*>(jobject->try_get_value("a"));
  ASSERT_NE(nullptr, tjstring);

  delete json;
}
