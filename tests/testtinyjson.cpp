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

  ASSERT_STREQ(jobject->try_get_string("a"), "null");
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

  ASSERT_STREQ(jobject->try_get_string("a"), "null");
  ASSERT_NE(nullptr, jobject->try_get_value("a"));
  ASSERT_NE(nullptr, dynamic_cast<const TinyJSON::TJValueNull*>(jobject->try_get_value("a")));

  ASSERT_STREQ(jobject->try_get_string("b"), "true");
  ASSERT_NE(nullptr, jobject->try_get_value("b"));
  
  auto boolean = dynamic_cast<const TinyJSON::TJValueBoolean*>(jobject->try_get_value("b"));
  ASSERT_NE(nullptr, boolean);
  ASSERT_TRUE(boolean->is_true());


  ASSERT_STREQ(jobject->try_get_string("c"), "false");
  ASSERT_NE(nullptr, jobject->try_get_value("c"));
  auto boolean2 = dynamic_cast<const TinyJSON::TJValueBoolean*>(jobject->try_get_value("c"));
  ASSERT_NE(nullptr, boolean2);
  ASSERT_FALSE(boolean2->is_true());

  ASSERT_STREQ(jobject->try_get_string("d"), "world");
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
  ASSERT_STREQ(jobject2->try_get_string("b"), "true");
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

// this is the test data take from https://github.com/stephenberry/json_performance/blob/main/README.md
// we have to be able to read this ...
TEST(TestBasic, ReadPerformanceBlob) {
  auto json = TinyJSON::TinyJSON::parse(R"(
{
   "fixed_object": {
      "int_array": [0, 1, 2, 3, 4, 5, 6],
      "float_array": [0.1, 0.2, 0.3, 0.4, 0.5, 0.6],
      "double_array": [3288398.238, 233e22, 289e-1, 0.928759872, 0.22222848, 0.1, 0.2, 0.3, 0.4]
   },
   "fixed_name_object": {
      "name0": "James",
      "name1": "Abraham",
      "name2": "Susan",
      "name3": "Frank",
      "name4": "Alicia"
   },
   "another_object": {
      "string": "here is some text",
      "another_string": "Hello World",
      "escaped_text": "{\"some key\":\"some string value\"}",
      "boolean": false,
      "nested_object": {
         "v3s": [[0.12345, 0.23456, 0.001345],
                  [0.3894675, 97.39827, 297.92387],
                  [18.18, 87.289, 2988.298]],
         "id": "298728949872"
      }
   },
   "string_array": ["Cat", "Dog", "Elephant", "Tiger"],
   "string": "Hello world",
   "number": 3.14,
   "boolean": true,
   "another_bool": false
}
)"
);
  ASSERT_NE(nullptr, json);

  // then check some values
  auto tjobject = dynamic_cast<TinyJSON::TJValueObject*>(json);
  ASSERT_NE(nullptr, tjobject);
  auto fixed_name_object = tjobject->try_get_value("fixed_name_object");
  ASSERT_NE(nullptr, fixed_name_object);
  ASSERT_TRUE(fixed_name_object->is_object());
  auto actual_fixed_name_object = dynamic_cast<const TinyJSON::TJValueObject*>(fixed_name_object);
  ASSERT_EQ(5, actual_fixed_name_object->get_number_of_items());

  delete json;
}

TEST(TestBasic, TrueBooleanInStringIsValid) {
  auto json = TinyJSON::TinyJSON::parse("true");
  ASSERT_NE(nullptr, json);
  ASSERT_TRUE(json->is_true());

  delete json;
}

TEST(TestBasic, IntNumberInStringIsValid) {

  std::vector<std::string> values;
  values.push_back("42");
  values.push_back("-42");
  values.push_back("0");
  values.push_back("-1");
  for (auto& number : values)
  {
    std::string s_json = " " + number + " ";
    auto json = TinyJSON::TinyJSON::parse(s_json.c_str());
    ASSERT_NE(nullptr, json);
    ASSERT_TRUE(json->is_number());

    auto value = dynamic_cast<const TinyJSON::TJValueNumberInt*>(json);
    ASSERT_NE(nullptr, value);
    ASSERT_EQ(value->get_number(), std::atoi(number.c_str()));
    delete json;
  }
}

TEST(TestBasic, FalseBooleanInStringIsValid) {
  auto json = TinyJSON::TinyJSON::parse("false");
  ASSERT_NE(nullptr, json);
  ASSERT_TRUE(json->is_false());

  delete json;
}

TEST(TestBasic, NullInStringIsValid) {
  auto json = TinyJSON::TinyJSON::parse("null");
  ASSERT_NE(nullptr, json);
  ASSERT_TRUE(json->is_null());

  delete json;
}

TEST(TestBasic, NothingIsJustAnEmptyString) {
  auto json = TinyJSON::TinyJSON::parse("");
  ASSERT_NE(nullptr, json);
  ASSERT_TRUE(json->is_string());
  ASSERT_STREQ("", json->dump_string());

  delete json;
}

TEST(TestBasic, NothingIsJustAnEmptyStringWithSpaces) {
  auto json = TinyJSON::TinyJSON::parse(R"(   






)");
  ASSERT_NE(nullptr, json);
  ASSERT_TRUE(json->is_string());
  ASSERT_STREQ("", json->dump_string());

  delete json;
}

TEST(TestBasic, StringValueIsValid) {
  auto json = TinyJSON::TinyJSON::parse(R"("Hello")");
  ASSERT_NE(nullptr, json);
  ASSERT_TRUE(json->is_string());
  ASSERT_STREQ("Hello", json->dump_string());

  delete json;
}

TEST(TestBasic, StringValueIsValidWithSpaces) {
  auto json = TinyJSON::TinyJSON::parse(R"(   



"Hello"


)");
  ASSERT_NE(nullptr, json);
  ASSERT_TRUE(json->is_string());
  ASSERT_STREQ("Hello", json->dump_string());

  delete json;
}

TEST(TestBasic, ValueInObjectOverwriteEachother) {
  auto json = TinyJSON::TinyJSON::parse(R"(
   {
"a" : 12,
"a" : 24
   }
   )");
  ASSERT_NE(nullptr, json);
  const auto jobject = dynamic_cast<TinyJSON::TJValueObject*>(json);
  ASSERT_NE(nullptr, jobject);

  // should only be one item
  ASSERT_EQ(1, jobject->get_number_of_items());

  auto text = json->dump();
  ASSERT_STREQ(R"({
  "a": 24
})", text );

  delete json;
}

TEST(TestBasic, ValueInObjectOverwriteEachotherInsideArray) {
  auto json = TinyJSON::TinyJSON::parse(R"(
   {
"a" : [12,24,48],
"a" : 24
   }
   )");
  ASSERT_NE(nullptr, json);
  const auto jobject = dynamic_cast<TinyJSON::TJValueObject*>(json);
  ASSERT_NE(nullptr, jobject);

  // should only be one item
  ASSERT_EQ(1, jobject->get_number_of_items());

  auto text = json->dump();
  ASSERT_STREQ(R"({
  "a": 24
})", text);

  delete json;
}

TEST(TestBasic, ValueInObjectOverwriteEachotherInsideArray2) {
  auto json = TinyJSON::TinyJSON::parse(R"(
[
  { 
    "a" : 12,
    "a" : 24
  },
  12,24
])");
  ASSERT_NE(nullptr, json);
  const auto tjarray = dynamic_cast<TinyJSON::TJValueArray*>(json);
  ASSERT_NE(nullptr, tjarray);

  // should only be one item
  ASSERT_EQ(3, tjarray->get_number_of_items());

  auto text = json->dump();
  ASSERT_STREQ(R"([
  {
    "a": 24
  },
  12,
  24
])", text);

  delete json;
}
