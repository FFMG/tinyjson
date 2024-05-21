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
  EXPECT_NE(nullptr, json);
  EXPECT_NE(nullptr, dynamic_cast<TinyJSON::TJValue*>(json));
  EXPECT_NE(nullptr, dynamic_cast<TinyJSON::TJValueObject*>(json));

  delete json;
}

TEST(TestBasic, InvalidJsonObjectReturnNull) {
  auto json = TinyJSON::TinyJSON::Parse("{");
  EXPECT_EQ(nullptr, json);
}

TEST(TestBasic, InvalidJsonObject2ReturnNull) {
  auto json = TinyJSON::TinyJSON::Parse( R"({
    "a" : "b"
    )");
  EXPECT_EQ(nullptr, json);
}

TEST(TestBasic, TheObjectInsideTheObjectDoesNotCloseProperly) {
  auto json = TinyJSON::TinyJSON::Parse(R"({
     "a" : { 
      "a" : "b"
    }
    )");
  EXPECT_EQ(nullptr, json);
}
TEST(TestBasic, HaveEnEmptyObjectWithNothing) {
  auto json = TinyJSON::TinyJSON::Parse("{}");
  EXPECT_NE(nullptr, json);
  EXPECT_NE(nullptr, dynamic_cast<TinyJSON::TJValueObject*>(json));

  delete json;
}

TEST(TestBasic, SpacesAreIgnored) {
  auto json = TinyJSON::TinyJSON::Parse(" {  }  ");
  EXPECT_NE(nullptr, json);
  EXPECT_NE(nullptr, dynamic_cast<TinyJSON::TJValueObject*>(json));

  delete json;
}

TEST(TestBasic, InvalidCommaAfterTheClosedObject) {
  auto json = TinyJSON::TinyJSON::Parse("{},");
  EXPECT_EQ(nullptr, json);
}

TEST(TestBasic, ClosedObjectTwice) {
  auto json = TinyJSON::TinyJSON::Parse("{}}");
  EXPECT_EQ(nullptr, json);
}

TEST(TestBasic, TheStringNameValueIsSaved) {
  auto json = TinyJSON::TinyJSON::Parse(R"(
{
  "Hello" : "World"
}
)"
    );
  EXPECT_NE(nullptr, json);

  auto jobject = dynamic_cast<TinyJSON::TJValueObject*>(json);
  EXPECT_NE(nullptr, jobject);

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
  EXPECT_NE(nullptr, json);
  auto jobject = dynamic_cast<TinyJSON::TJValueObject*>(json);
  EXPECT_NE(nullptr, jobject);

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
  EXPECT_NE(nullptr, json);
  auto jobject = dynamic_cast<TinyJSON::TJValueObject*>(json);
  EXPECT_NE(nullptr, jobject);

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
  EXPECT_EQ(nullptr, json);

  delete json;
}

TEST(TestBasic, CommaAfterTheLastString) {
  auto json = TinyJSON::TinyJSON::Parse(R"(
{
  "a" : "a",
}
)"
);
  EXPECT_EQ(nullptr, json);

  delete json;
}

TEST(TestBasic, CheckForTrue) {
  auto json = TinyJSON::TinyJSON::Parse(R"(
{
  "a" : true
}
)"
);
  EXPECT_NE(nullptr, json);
  auto jobject = dynamic_cast<TinyJSON::TJValueObject*>(json);
  EXPECT_NE(nullptr, jobject);

  EXPECT_STREQ(jobject->try_get_string("a"), "true");
  EXPECT_NE(nullptr, jobject->try_get_value("a"));
  EXPECT_NE(nullptr, dynamic_cast<const TinyJSON::TJValueTrue*>(jobject->try_get_value("a")));

  delete json;
}

TEST(TestBasic, CheckForFalse) {
  auto json = TinyJSON::TinyJSON::Parse(R"(
{
  "a" : false
}
)"
);
  EXPECT_NE(nullptr, json);
  auto jobject = dynamic_cast<TinyJSON::TJValueObject*>(json);
  EXPECT_NE(nullptr, jobject);

  EXPECT_STREQ(jobject->try_get_string("a"), "false");
  EXPECT_NE(nullptr, jobject->try_get_value("a"));
  EXPECT_NE(nullptr, dynamic_cast<const TinyJSON::TJValueFalse*>(jobject->try_get_value("a")));

  delete json;
}

TEST(TestBasic, CheckForNull) {
  auto json = TinyJSON::TinyJSON::Parse(R"(
{
  "a" : null
}
)"
);
  EXPECT_NE(nullptr, json);
  auto jobject = dynamic_cast<TinyJSON::TJValueObject*>(json);
  EXPECT_NE(nullptr, jobject);

  EXPECT_STREQ(jobject->try_get_string("a"), "null");
  EXPECT_NE(nullptr, jobject->try_get_value("a"));
  EXPECT_NE(nullptr, dynamic_cast<const TinyJSON::TJValueNull*>(jobject->try_get_value("a")));

  delete json;
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
  EXPECT_NE(nullptr, json);
  auto jobject = dynamic_cast<TinyJSON::TJValueObject*>(json);
  EXPECT_NE(nullptr, jobject);

  EXPECT_STREQ(jobject->try_get_string("a"), "null");
  EXPECT_NE(nullptr, jobject->try_get_value("a"));
  EXPECT_NE(nullptr, dynamic_cast<const TinyJSON::TJValueNull*>(jobject->try_get_value("a")));

  EXPECT_STREQ(jobject->try_get_string("b"), "true");
  EXPECT_NE(nullptr, jobject->try_get_value("b"));
  EXPECT_NE(nullptr, dynamic_cast<const TinyJSON::TJValueTrue*>(jobject->try_get_value("b")));

  EXPECT_STREQ(jobject->try_get_string("c"), "false");
  EXPECT_NE(nullptr, jobject->try_get_value("c"));
  EXPECT_NE(nullptr, dynamic_cast<const TinyJSON::TJValueFalse*>(jobject->try_get_value("c")));

  EXPECT_STREQ(jobject->try_get_string("d"), "world");
  EXPECT_NE(nullptr, jobject->try_get_value("d"));
  EXPECT_NE(nullptr, dynamic_cast<const TinyJSON::TJValueString*>(jobject->try_get_value("d")));

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
  EXPECT_NE(nullptr, json);
  auto jobject = dynamic_cast<TinyJSON::TJValueObject*>(json);
  EXPECT_NE(nullptr, jobject);

  auto value = jobject->try_get_value("a");
  EXPECT_NE(nullptr, value);

  auto jobject2 = dynamic_cast<const TinyJSON::TJValueObject*>(value);
  EXPECT_NE(nullptr, jobject2);

  // finally check that the value are correct.
  EXPECT_STREQ(jobject2->try_get_string("b"), "true");
  EXPECT_NE(nullptr, jobject2->try_get_value("b"));
  EXPECT_NE(nullptr, dynamic_cast<const TinyJSON::TJValueTrue*>(jobject2->try_get_value("b")));

  delete json;
}
