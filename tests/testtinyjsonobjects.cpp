// Licensed to Florent Guelfucci under one or more agreements.
// Florent Guelfucci licenses this file to you under the MIT license.
// See the LICENSE file in the project root for more information.
#include <gtest/gtest.h>
#include "../src/TinyJSON.h"

TEST(TestObjects, MakeSureThatEmptyStringIsKinkOfValueObject) {
  auto json = TinyJSON::TinyJSON::parse("{}");
  ASSERT_NE(nullptr, json);
  ASSERT_NE(nullptr, dynamic_cast<TinyJSON::TJValue*>(json));
  ASSERT_NE(nullptr, dynamic_cast<TinyJSON::TJValueObject*>(json));

  delete json;
}

TEST(TestObjects, EmptyObjectHasNoItemsInIt) {
  auto json = TinyJSON::TinyJSON::parse("{}");
  ASSERT_NE(nullptr, json);
  ASSERT_NE(nullptr, dynamic_cast<TinyJSON::TJValue*>(json));

  const auto jobject = dynamic_cast<TinyJSON::TJValueObject*>(json));
  ASSERT_NE(nullptr, jobject);
  ASSERT_EQ(0, jobject->number_of_items());

  delete json;
}

TEST(TestObjects, EmptyObjectInSideObectHasNoItemsInIt) {
  auto json = TinyJSON::TinyJSON::parse(R"(
    {
      "a" {
      }
    }
    )"
  );

  ASSERT_NE(nullptr, json);
  ASSERT_NE(nullptr, dynamic_cast<TinyJSON::TJValue*>(json));

  const auto jobject = dynamic_cast<TinyJSON::TJValueObject*>(json));
  ASSERT_NE(nullptr, jobject);
  ASSERT_EQ(1, jobject->number_of_items());

  const auto jobjecta = dynamic_cast<TinyJSON::TJValueObject*>(jobject->try_get_value("a"));
  ASSERT_NE(nullptr, jobjecta);
  ASSERT_EQ(0, jobjecta->number_of_items());

  delete json;
}

TEST(TestObjects, ClosedObjectTwice) {
  auto json = TinyJSON::TinyJSON::parse("{}}");
  ASSERT_EQ(nullptr, json);
}

TEST(TestObjects, ObjectIsAfterMissingColon) {
  auto json = TinyJSON::TinyJSON::parse(R"(
    {
      "a" {
        "b" : 12
      }
    }
    )"
  );
  ASSERT_EQ(nullptr, json);
}

TEST(TestObjects, ObjectOpensButNeverCloses) {
  auto json = TinyJSON::TinyJSON::parse("{");
  ASSERT_EQ(nullptr, json);
}

TEST(TestObjects, ObjectOpensAndHasValuesButNeverCloses) {
  auto json = TinyJSON::TinyJSON::parse(R"({
    "a" : "b"
    )");
  ASSERT_EQ(nullptr, json);
}
