// Licensed to Florent Guelfucci under one or more agreements.
// Florent Guelfucci licenses this file to you under the MIT license.
// See the LICENSE file in the project root for more information.
#if defined(_MSC_VER)
#  define _CRTDBG_MAP_ALLOC
#  include <stdlib.h>
#  include <crtdbg.h>
#endif

#include <gtest/gtest.h>
#include <type_traits>

#include "../src/TinyJSON.h"

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
#if defined(_MSC_VER)
  _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
  return RUN_ALL_TESTS();
}

template <typename Base, typename Derived>
bool IsDerivedFrom() {
  return std::is_base_of<Base, Derived>::value;
}

TEST(TestBasic, TheObjectInsideTheObjectDoesNotCloseProperly) {
  auto json = TinyJSON::TinyJSON::parse(R"(
   {
     "a" : { 
        "a" : "b"
   }
   )");
  ASSERT_EQ(nullptr, json);
}
TEST(TestBasic, HaveEnEmptyObjectWithNothing) {
  auto json = TinyJSON::TinyJSON::parse("{}");
  ASSERT_NE(nullptr, json);
  ASSERT_NE(nullptr, dynamic_cast<TinyJSON::TJValueObject*>(json));

  delete json;
}

TEST(TestBasic, SpacesAreIgnored) {
  auto json = TinyJSON::TinyJSON::parse(" {  }  ");
  ASSERT_NE(nullptr, json);
  ASSERT_NE(nullptr, dynamic_cast<TinyJSON::TJValueObject*>(json));

  delete json;
}

TEST(TestBasic, InvalidCommaAfterTheClosedObject) {
  auto json = TinyJSON::TinyJSON::parse("{},");
  ASSERT_EQ(nullptr, json);
}

TEST(TestBasic, CommaBeforeTheStringIsNotAllowed) {
  auto json = TinyJSON::TinyJSON::parse(R"(
{
  ,"a" : "a"
}
)"
);
  ASSERT_EQ(nullptr, json);

  delete json;
}

TEST(TestBasic, CommaAfterTheLastStringIsNotAllowed) {
  auto json = TinyJSON::TinyJSON::parse(R"(
{
  "a" : "a",
}
)"
);
  ASSERT_EQ(nullptr, json);

  delete json;
}

TEST(TestBasic, CheckForNull) {
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
  ASSERT_NE(nullptr, dynamic_cast<const TinyJSON::TJValueNull*>(jobject->try_get_value("a")));

  delete json;
}

TEST(TestBasic, WeRequireACommaBetweenStringValues) {
  auto json = TinyJSON::TinyJSON::parse(R"(
{
  "a" : "a",
  "b" : "b"
  "c" : "c"
}
)"
);
  ASSERT_EQ(nullptr, json);
}

TEST(TestBasic, WeRequireACommaBetweenValuesOfNumbersAndObjects) {

  // missing a comma after the number
  auto json = TinyJSON::TinyJSON::parse(R"(
{
  "a" : 12,
  "b" : {}
  "c" : "c"
}
)"
);
  ASSERT_EQ(nullptr, json);
}

TEST(TestBasic, TwoCommaBetweenElementsIsNotValid) {

  // missing a comma after the number
  auto json = TinyJSON::TinyJSON::parse(R"(
{
  "a" : 12,,
  "b" : 13
}
)"
);
  ASSERT_EQ(nullptr, json);
}

TEST(TestBasic, CheckForDifferentValueTypes) {
  auto json = TinyJSON::TinyJSON::parse(R"(
{
  "a" : null,
  "b" : true,
  "c" : false,
  "d" : "world"
}
)"
);
  ASSERT_NE(nullptr, json);
  auto jobject = dynamic_cast<TinyJSON::TJValueObject*>(json);
  ASSERT_NE(nullptr, jobject);

  EXPECT_STREQ(jobject->try_get_string("a"), "null");
  ASSERT_NE(nullptr, jobject->try_get_value("a"));
  ASSERT_NE(nullptr, dynamic_cast<const TinyJSON::TJValueNull*>(jobject->try_get_value("a")));

  EXPECT_STREQ(jobject->try_get_string("b"), "true");
  ASSERT_NE(nullptr, jobject->try_get_value("b"));
  
  auto boolean = dynamic_cast<const TinyJSON::TJValueBoolean*>(jobject->try_get_value("b"));
  ASSERT_NE(nullptr, boolean);
  ASSERT_TRUE(boolean->is_true());


  EXPECT_STREQ(jobject->try_get_string("c"), "false");
  ASSERT_NE(nullptr, jobject->try_get_value("c"));
  auto boolean2 = dynamic_cast<const TinyJSON::TJValueBoolean*>(jobject->try_get_value("c"));
  ASSERT_NE(nullptr, boolean2);
  ASSERT_FALSE(boolean2->is_true());


  EXPECT_STREQ(jobject->try_get_string("d"), "world");
  ASSERT_NE(nullptr, jobject->try_get_value("d"));
  ASSERT_NE(nullptr, dynamic_cast<const TinyJSON::TJValueString*>(jobject->try_get_value("d")));

  delete json;
}

TEST(TestBasic, ObjectInsideAnObject) {
  auto json = TinyJSON::TinyJSON::parse(R"(
{
  "a" : {
    "b" : true
  }
}
)"
);
  ASSERT_NE(nullptr, json);
  auto jobject = dynamic_cast<TinyJSON::TJValueObject*>(json);
  ASSERT_NE(nullptr, jobject);

  auto value = jobject->try_get_value("a");
  ASSERT_NE(nullptr, value);

  auto jobject2 = dynamic_cast<const TinyJSON::TJValueObject*>(value);
  ASSERT_NE(nullptr, jobject2);

  // finally check that the value are correct.
  EXPECT_STREQ(jobject2->try_get_string("b"), "true");
  ASSERT_NE(nullptr, jobject2->try_get_value("b"));

  auto boolean = dynamic_cast<const TinyJSON::TJValueBoolean*>(jobject2->try_get_value("b"));
  ASSERT_NE(nullptr, boolean);
  ASSERT_TRUE(boolean->is_true());

  delete json;
}

TEST(TestBasic, ObjectMultipleDepth) {
  auto json = TinyJSON::TinyJSON::parse(R"(
{
  "a" : true,
  "b" : {
    "aa" : true,
    "bb" : {
      "aaa" : true,
      "bbb" : {
        "aaaa" : true
      }
    }
  }
}
)"
);
  ASSERT_NE(nullptr, json);
  auto jobject = dynamic_cast<TinyJSON::TJValueObject*>(json);
  ASSERT_NE(nullptr, jobject);

  auto valueb = dynamic_cast<const TinyJSON::TJValueObject*>(jobject->try_get_value("b"));
  ASSERT_NE(nullptr, valueb);

  auto valuebb = dynamic_cast<const TinyJSON::TJValueObject*>(valueb->try_get_value("bb"));
  ASSERT_NE(nullptr, valuebb);

  auto valuebbb = dynamic_cast<const TinyJSON::TJValueObject*>(valuebb->try_get_value("bbb"));
  ASSERT_NE(nullptr, valuebbb);

  // finally check that the value are correct.
  auto boolean = dynamic_cast<const TinyJSON::TJValueBoolean*>(valuebbb->try_get_value("aaaa"));
  ASSERT_NE(nullptr, boolean);
  ASSERT_TRUE(boolean->is_true());

  delete json;
}
