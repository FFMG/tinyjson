// Licensed to Florent Guelfucci under one or more agreements.
// Florent Guelfucci licenses this file to you under the MIT license.
// See the LICENSE file in the project root for more information.
#include <gtest/gtest.h>
#define TJ_USE_CHAR 1
#include "../src/TinyJSON.h"

TEST(TestObjects, MakeSureThatEmptyStringIsKinkOfValueObject) {
  auto json = TinyJSON::TJ::parse("{}");
  ASSERT_NE(nullptr, json);
  ASSERT_NE(nullptr, dynamic_cast<TinyJSON::TJValue*>(json));
  ASSERT_NE(nullptr, dynamic_cast<TinyJSON::TJValueObject*>(json));

  delete json;
}

TEST(TestObjects, EmptyObjectHasNoItemsInIt) {
  auto json = TinyJSON::TJ::parse("{}");
  ASSERT_NE(nullptr, json);
  ASSERT_NE(nullptr, dynamic_cast<TinyJSON::TJValue*>(json));

  const auto jobject = dynamic_cast<TinyJSON::TJValueObject*>(json);
  ASSERT_NE(nullptr, jobject);
  ASSERT_EQ(0, jobject->get_number_of_items());

  delete json;
}

TEST(TestObjects, EmptyObjectInSideObectHasNoItemsInIt) {
  auto json = TinyJSON::TJ::parse(R"(
    {
      "a" : {
      }
    }
    )"
  );

  ASSERT_NE(nullptr, json);
  ASSERT_NE(nullptr, dynamic_cast<TinyJSON::TJValue*>(json));

  const auto jobject = dynamic_cast<TinyJSON::TJValueObject*>(json);
  ASSERT_NE(nullptr, jobject);
  ASSERT_EQ(1, jobject->get_number_of_items());

  const auto jobjecta = dynamic_cast<const TinyJSON::TJValueObject*>(jobject->try_get_value("a"));
  ASSERT_NE(nullptr, jobjecta);
  ASSERT_EQ(0, jobjecta->get_number_of_items());

  delete json;
}

TEST(TestObjects, GetItemByIndex) {
  auto json = TinyJSON::TJ::parse(R"(
    {
      "a" : {
        "aa" : {}
      }
    }
    )"
  );

  ASSERT_NE(nullptr, json);
  ASSERT_NE(nullptr, dynamic_cast<TinyJSON::TJValue*>(json));

  const auto jobject = dynamic_cast<TinyJSON::TJValueObject*>(json);
  ASSERT_NE(nullptr, jobject);
  ASSERT_EQ(1, jobject->get_number_of_items());

  const auto jobjecta = dynamic_cast<const TinyJSON::TJValueObject*>((*jobject)[0]->value());
  ASSERT_NE(nullptr, jobjecta);
  ASSERT_EQ(1, jobjecta->get_number_of_items());

  const auto jobjectb = dynamic_cast<const TinyJSON::TJValueObject*>(jobjecta->at(0)->value());
  ASSERT_NE(nullptr, jobjectb);
  ASSERT_EQ(0, jobjectb->get_number_of_items());

  delete json;
}

TEST(TestObjects, ClosedObjectTwice) {
  auto json = TinyJSON::TJ::parse("{}}");
  ASSERT_EQ(nullptr, json);
}

TEST(TestObjects, ObjectIsAfterMissingColon) {
  auto json = TinyJSON::TJ::parse(R"(
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
  auto json = TinyJSON::TJ::parse("{");
  ASSERT_EQ(nullptr, json);
}

TEST(TestObjects, ObjectOpensAndHasValuesButNeverCloses) {
  auto json = TinyJSON::TJ::parse(R"({
    "a" : "b"
    )");
  ASSERT_EQ(nullptr, json);
}

TEST(TestObjects, CheckThatValueIsObject) {
  auto json = TinyJSON::TJ::parse("{}");
  ASSERT_NE(nullptr, json);

  ASSERT_TRUE(json->is_object());
  ASSERT_FALSE(json->is_array());
  ASSERT_FALSE(json->is_string());
  ASSERT_FALSE(json->is_number());
  ASSERT_FALSE(json->is_true());
  ASSERT_FALSE(json->is_false());
  ASSERT_FALSE(json->is_null());
  delete json;
}

TEST(TestObjects, TryingToGetAnItemThatDoesNotExitReturnsNull) {
  auto json = TinyJSON::TJ::parse(R"(
{
  "a" : 12, "b" : 13, "c" : 14
}
)"
);
  ASSERT_NE(nullptr, json);
  auto object_of_values = dynamic_cast<TinyJSON::TJValueObject*>(json);
  ASSERT_EQ(3, object_of_values->get_number_of_items());

  ASSERT_NE(nullptr, object_of_values->at(0));
  ASSERT_EQ(nullptr, object_of_values->at(3));
  delete json;
}

TEST(TestObjects, TryingToGetANegativeItemReturnsNull) {
  auto json = TinyJSON::TJ::parse(R"(
{
  "a" : 12, "b" : 13, "c" : 14
}
)"
);
  ASSERT_NE(nullptr, json);
  auto object_of_values = dynamic_cast<TinyJSON::TJValueObject*>(json);
  ASSERT_EQ(3, object_of_values->get_number_of_items());

  ASSERT_NE(nullptr, object_of_values->at(0));
  ASSERT_EQ(nullptr, object_of_values->at(-1));
  ASSERT_EQ(nullptr, object_of_values->at(-42));
  delete json;
}

TEST(TestObjects, ObjectHasAValidStringJustNoColon) {
  auto json = TinyJSON::TJ::parse(R"(
    {
      "a" 
    }
    )"
  );
  ASSERT_EQ(nullptr, json);
}

TEST(TestObjects, ItemsInArrayMustBeSeparatedByComma) {
  auto json = TinyJSON::TJ::parse(R"(
{
  "a" : 12
  "b" : 13
}
)"
);
  ASSERT_EQ(nullptr, json);
}

TEST(TestObjects, ItemsInArrayMustBeSeparatedByCommaWithStrings) {
  auto json = TinyJSON::TJ::parse(R"(
{
  "a" : "A"
  "b" : "B"
}
)"
);
  ASSERT_EQ(nullptr, json);
}

TEST(TestObjects, ItemsInArrayMustBeSeparatedByCommaWithNumberAndStrings) {
  auto json = TinyJSON::TJ::parse(R"(
{
  "a" : 12
  "b" : "B"
}
)"
);
  ASSERT_EQ(nullptr, json);
}

TEST(TestObjects, ObjectHasACommaButThenTheObjectEnds) {
  auto json = TinyJSON::TJ::parse(R"(
[
  "a" : 12,
  "b" : "B",
]
)"
);
  ASSERT_EQ(nullptr, json);
}

TEST(TestObjects, TheLastItemInOurBrokenJsonIsAnEscape) {
  auto json = TinyJSON::TJ::parse(R"(
{
 "a" : "test1"
 "b" : "tes2\)"
);
  ASSERT_EQ(nullptr, json);
  delete json;
}


TEST(TestObjects, ShallowQueries)
{
  auto json = R"({
    "string" : "Hello world 0",
    "number" : 12
  })";
  auto tjjson = TinyJSON::TJ::parse(json);
  ASSERT_NE(nullptr, tjjson);
  auto tjobject = dynamic_cast<TinyJSON::TJValueObject*>(tjjson);
  auto o1 = dynamic_cast<const TinyJSON::TJValueNumberInt*>(tjobject->try_get_value("number"));
  ASSERT_NE(nullptr, o1);
  auto o2 = dynamic_cast<const TinyJSON::TJValueString*>(tjobject->try_get_value("string"));
  ASSERT_NE(nullptr, o2);
  ASSERT_STREQ("Hello world 0", o2->dump_string());

  delete tjjson;
}

TEST(TestObjects, ShallowQueriesWithManyItems)
{
  auto json = R"({
"t" : 20,
"a" : 1,
"b" : 2,
"w" : 23,
"c" : 3,
"d" : 4,
"e" : 5,
"f" : 6,
"g" : 7,
"h" : 8,
"j" : 10,
"v" : 22,
"k" : 11,
"l" : 12,
"m" : 13,
"n" : 14,
"o" : 15,
"p" : 16,
"q" : 17,
"r" : 18,
"i" : 9,
"s" : 19,
"y" : 25,
"u" : 21,
"x" : 24,
"z" : 26
  })";
  auto tjjson = TinyJSON::TJ::parse(json);
  ASSERT_NE(nullptr, tjjson);
  auto tjobject = dynamic_cast<TinyJSON::TJValueObject*>(tjjson);
  auto o1 = dynamic_cast<const TinyJSON::TJValueNumberInt*>(tjobject->try_get_value("a"));
  ASSERT_EQ(1, o1->get_number());
  o1 = dynamic_cast<const TinyJSON::TJValueNumberInt*>(tjobject->try_get_value("z"));
  ASSERT_EQ(26, o1->get_number());

  delete tjjson;
}

TEST(TestObjects, DeepQueries)
{
  auto json = R"({
    "number" : 12,
    "string" : "Hello world 0",
    "object" : {
      "number" : 12,
      "string" : "Hello world 1",
      "object" : {
        "number" : 12,
        "string" : "Hello world 2"
      }
    }
  })";
  auto tjjson = TinyJSON::TJ::parse(json);
  ASSERT_NE(nullptr, tjjson);
  auto tjobject = dynamic_cast<TinyJSON::TJValueObject*>(tjjson);
  auto o1 = dynamic_cast<const TinyJSON::TJValueObject*>(tjobject->try_get_value("object"));
  ASSERT_NE(nullptr, o1);
  auto o2 = dynamic_cast<const TinyJSON::TJValueObject*>(o1->try_get_value("object"));
  ASSERT_NE(nullptr, o2);
  auto o3 = o2->try_get_value("string");
  ASSERT_NE(nullptr, o3);
  ASSERT_STREQ("Hello world 2", o3->dump_string());

  delete tjjson;
}