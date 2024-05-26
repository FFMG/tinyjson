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

TEST(TestBasic, MakeSureThatEmptyStringIsKinkOfValueObject) {
  auto json = TinyJSON::TinyJSON::Parse("{}");
  ASSERT_NE(nullptr, json);
  ASSERT_NE(nullptr, dynamic_cast<TinyJSON::TJValue*>(json));
  ASSERT_NE(nullptr, dynamic_cast<TinyJSON::TJValueObject*>(json));

  delete json;
}

TEST(TestBasic, InvalidJsonObjectReturnNull) {
  auto json = TinyJSON::TinyJSON::Parse("{");
  ASSERT_EQ(nullptr, json);
}

TEST(TestBasic, InvalidJsonObject2ReturnNull) {
  auto json = TinyJSON::TinyJSON::Parse( R"({
    "a" : "b"
    )");
  ASSERT_EQ(nullptr, json);
}

TEST(TestBasic, TheObjectInsideTheObjectDoesNotCloseProperly) {
  auto json = TinyJSON::TinyJSON::Parse(R"({
     "a" : { 
      "a" : "b"
    }
    )");
  ASSERT_EQ(nullptr, json);
}
TEST(TestBasic, HaveEnEmptyObjectWithNothing) {
  auto json = TinyJSON::TinyJSON::Parse("{}");
  ASSERT_NE(nullptr, json);
  ASSERT_NE(nullptr, dynamic_cast<TinyJSON::TJValueObject*>(json));

  delete json;
}

TEST(TestBasic, SpacesAreIgnored) {
  auto json = TinyJSON::TinyJSON::Parse(" {  }  ");
  ASSERT_NE(nullptr, json);
  ASSERT_NE(nullptr, dynamic_cast<TinyJSON::TJValueObject*>(json));

  delete json;
}

TEST(TestBasic, InvalidCommaAfterTheClosedObject) {
  auto json = TinyJSON::TinyJSON::Parse("{},");
  ASSERT_EQ(nullptr, json);
}

TEST(TestBasic, ClosedObjectTwice) {
  auto json = TinyJSON::TinyJSON::Parse("{}}");
  ASSERT_EQ(nullptr, json);
}

TEST(TestBasic, TheStringNameValueIsSaved) {
  auto json = TinyJSON::TinyJSON::Parse(R"(
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

TEST(TestBasic, TheStringNameValueIsSavedMultiLine) {
  auto json = TinyJSON::TinyJSON::Parse(R"(
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

TEST(TestBasic, TheStringNameValueIsSavedNultiplItems) {
  auto json = TinyJSON::TinyJSON::Parse(R"(
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

TEST(TestBasic, CommaBeforeTheString) {
  auto json = TinyJSON::TinyJSON::Parse(R"(
{
  ,"a" : "a"
}
)"
);
  ASSERT_EQ(nullptr, json);

  delete json;
}

TEST(TestBasic, CommaAfterTheLastString) {
  auto json = TinyJSON::TinyJSON::Parse(R"(
{
  "a" : "a",
}
)"
);
  ASSERT_EQ(nullptr, json);

  delete json;
}

TEST(TestBasic, CheckForTrue) {
  auto json = TinyJSON::TinyJSON::Parse(R"(
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

TEST(TestBasic, CheckForFalse) {
  auto json = TinyJSON::TinyJSON::Parse(R"(
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

TEST(TestBasic, CheckForNull) {
  auto json = TinyJSON::TinyJSON::Parse(R"(
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

TEST(TestBasic, WeRequireACommaBetweenValues) {
  auto json = TinyJSON::TinyJSON::Parse(R"(
{
  "a" : "a",
  "b" : "b"
  "c" : "c"
}
)"
);
  ASSERT_EQ(nullptr, json);
}

TEST(TestBasic, CheckForDifferentValueTypes) {
  auto json = TinyJSON::TinyJSON::Parse(R"(
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
  ASSERT_NE(nullptr, dynamic_cast<const TinyJSON::TJValueTrue*>(jobject->try_get_value("b")));

  EXPECT_STREQ(jobject->try_get_string("c"), "false");
  ASSERT_NE(nullptr, jobject->try_get_value("c"));
  ASSERT_NE(nullptr, dynamic_cast<const TinyJSON::TJValueFalse*>(jobject->try_get_value("c")));

  EXPECT_STREQ(jobject->try_get_string("d"), "world");
  ASSERT_NE(nullptr, jobject->try_get_value("d"));
  ASSERT_NE(nullptr, dynamic_cast<const TinyJSON::TJValueString*>(jobject->try_get_value("d")));

  delete json;
}

TEST(TestBasic, ObjectInsideAnObject) {
  auto json = TinyJSON::TinyJSON::Parse(R"(
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
  ASSERT_NE(nullptr, dynamic_cast<const TinyJSON::TJValueTrue*>(jobject2->try_get_value("b")));

  delete json;
}

TEST(TestBasic, ObjectMultipleDepth) {
  auto json = TinyJSON::TinyJSON::Parse(R"(
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
  EXPECT_STREQ(valuebbb->try_get_string("aaaa"), "true");
  ASSERT_NE(nullptr, valuebbb->try_get_value("aaaa"));
  ASSERT_NE(nullptr, dynamic_cast<const TinyJSON::TJValueTrue*>(valuebbb->try_get_value("aaaa")));

  delete json;
}
