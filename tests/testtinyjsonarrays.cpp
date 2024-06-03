// Licensed to Florent Guelfucci under one or more agreements.
// Florent Guelfucci licenses this file to you under the MIT license.
// See the LICENSE file in the project root for more information.
#include <gtest/gtest.h>
#include "../src/TinyJSON.h"

TEST(TestArrays, MakeSureThatEmptyStringIsKinkOfValueArray) {
  auto json = TinyJSON::TinyJSON::parse("[]");
  ASSERT_NE(nullptr, json);
  ASSERT_NE(nullptr, dynamic_cast<TinyJSON::TJValue*>(json));
  auto jarray = dynamic_cast<TinyJSON::TJValueArray*>(json);
  ASSERT_NE(nullptr, jarray);

  delete json;
}

TEST(TestArrays, ArrayOpensButNeverCloses) {
  auto json = TinyJSON::TinyJSON::parse("[");
  ASSERT_EQ(nullptr, json);
}

TEST(TestArrays, ArrayOpensAndHasValuesButNeverCloses) {
  auto json = TinyJSON::TinyJSON::parse(R"([
    "a", "b"
    )");
  ASSERT_EQ(nullptr, json);
}

TEST(TestArrays, ArrayHasTwoColons) {
  auto json = TinyJSON::TinyJSON::parse("[ 1,2,3,,4,5]");
  ASSERT_EQ(nullptr, json);
}

TEST(TestArrays, ArrayHasTwoColonsAtTheEnd) {
  auto json = TinyJSON::TinyJSON::parse("[ 1,2,3,,]");
  ASSERT_EQ(nullptr, json);
}

TEST(TestArrays, EmptyArrayInObject) {
  auto json = TinyJSON::TinyJSON::parse(R"(
{
  "Hello" : []
}
)"
);
  ASSERT_NE(nullptr, json);
  auto jobject = dynamic_cast<TinyJSON::TJValueObject*>(json);
  ASSERT_NE(nullptr, jobject);

  const auto array_of_values = dynamic_cast<const TinyJSON::TJValueArray*>(jobject->try_get_value("Hello"));
  ASSERT_NE(nullptr, array_of_values);
  ASSERT_EQ(0, array_of_values->number_of_items());

  delete json;
}

TEST(TestArrays, ArrayInObjectIsAfterMissingColon) {
  auto json = TinyJSON::TinyJSON::parse(R"(
    {
      "a" [
        42, 12
      ]
    }
    )"
  );
  ASSERT_EQ(nullptr, json);
}

TEST(TestArrays, EmptyArrayOfNumbersHasNoItems) {
  auto json = TinyJSON::TinyJSON::parse(R"(
[
]
)"
);
  ASSERT_NE(nullptr, json);
  auto array_of_values = dynamic_cast<TinyJSON::TJValueArray*>(json);
  ASSERT_EQ(0, array_of_values->number_of_items());
  delete json;
}

TEST(TestArrays, ArrayOfNumbersHasCorrectNumberOfItems) {
  auto json = TinyJSON::TinyJSON::parse(R"(
[
  12, 13, 14
]
)"
);
  ASSERT_NE(nullptr, json);
  auto array_of_values = dynamic_cast<TinyJSON::TJValueArray*>(json);
  ASSERT_EQ(3, array_of_values->number_of_items());
  delete json;
}
