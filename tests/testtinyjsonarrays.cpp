// Licensed to Florent Guelfucci under one or more agreements.
// Florent Guelfucci licenses this file to you under the MIT license.
// See the LICENSE file in the project root for more information.
#include <gtest/gtest.h>
#define TJ_USE_CHAR 1
#include "../src/TinyJSON.h"
#include <vector>

TEST(TJValueArray, MakeSureThatEmptyStringIsKinkOfValueArray) {
  auto json = TinyJSON::TJ::parse("[]");
  ASSERT_NE(nullptr, json);
  ASSERT_NE(nullptr, dynamic_cast<TinyJSON::TJValue*>(json));
  auto jarray = dynamic_cast<TinyJSON::TJValueArray*>(json);
  ASSERT_NE(nullptr, jarray);

  delete json;
}

TEST(TJValueArray, ArrayOpensButNeverCloses) {
  auto json = TinyJSON::TJ::parse("[");
  ASSERT_EQ(nullptr, json);
}

TEST(TJValueArray, ArrayOpensAndHasValuesButNeverCloses) {
  auto json = TinyJSON::TJ::parse(R"([
    "a", "b"
    )");
  ASSERT_EQ(nullptr, json);
}

TEST(TJValueArray, ArrayHasTwoColons) {
  auto json = TinyJSON::TJ::parse("[ 1,2,3,,4,5]");
  ASSERT_EQ(nullptr, json);
}

TEST(TJValueArray, ArrayHasTwoColonsAtTheEnd) {
  auto json = TinyJSON::TJ::parse("[ 1,2,3,,]");
  ASSERT_EQ(nullptr, json);
}

TEST(TJValueArray, EmptyArrayInObject) {
  auto json = TinyJSON::TJ::parse(R"(
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
  ASSERT_EQ(0, array_of_values->get_number_of_items());

  delete json;
}

TEST(TJValueArray, ArrayInObjectIsAfterMissingColon) {
  auto json = TinyJSON::TJ::parse(R"(
    {
      "a" [
        42, 12
      ]
    }
    )"
  );
  ASSERT_EQ(nullptr, json);
}

TEST(TJValueArray, EmptyArrayOfNumbersHasNoItems) {
  auto json = TinyJSON::TJ::parse(R"(
[
]
)"
);
  ASSERT_NE(nullptr, json);
  auto array_of_values = dynamic_cast<TinyJSON::TJValueArray*>(json);
  ASSERT_EQ(0, array_of_values->get_number_of_items());
  delete json;
}

TEST(TJValueArray, ArrayOfNumbersHasCorrectNumberOfItems) {
  auto json = TinyJSON::TJ::parse(R"(
[
  12, 13, 14
]
)"
);
  ASSERT_NE(nullptr, json);
  auto array_of_values = dynamic_cast<TinyJSON::TJValueArray*>(json);
  ASSERT_EQ(3, array_of_values->get_number_of_items());
  delete json;
}

TEST(TJValueArray, EmptyArrayInSideArrayHasNoItemsInIt) {
  auto json = TinyJSON::TJ::parse(R"(
    [
      [
      ]
    ]
    )"
  );

  ASSERT_NE(nullptr, json);
  ASSERT_NE(nullptr, dynamic_cast<TinyJSON::TJValue*>(json));

  const auto jarray = dynamic_cast<TinyJSON::TJValueArray*>(json);
  ASSERT_NE(nullptr, jarray);
  ASSERT_EQ(1, jarray->get_number_of_items());

  const auto jarraya = dynamic_cast<const TinyJSON::TJValueArray*>((*jarray)[0]);
  ASSERT_NE(nullptr, jarraya);
  ASSERT_EQ(0, jarraya->get_number_of_items());

  delete json;
}

TEST(TJValueArray, CheckThatValueIsArray) {
  auto json = TinyJSON::TJ::parse("[]");
  ASSERT_NE(nullptr, json);

  ASSERT_FALSE(json->is_object());
  ASSERT_TRUE(json->is_array());
  ASSERT_FALSE(json->is_string());
  ASSERT_FALSE(json->is_number());
  ASSERT_FALSE(json->is_true());
  ASSERT_FALSE(json->is_false());
  ASSERT_FALSE(json->is_null());
  delete json;
}

TEST(TJValueArray, TryingToGetAnItemThatDoesNotExitReturnsNull) {
  auto json = TinyJSON::TJ::parse(R"(
[
  12, 13, 14
]
)"
);
  ASSERT_NE(nullptr, json);
  auto array_of_values = dynamic_cast<TinyJSON::TJValueArray*>(json);
  ASSERT_EQ(3, array_of_values->get_number_of_items());

  ASSERT_NE(nullptr, array_of_values->at(0));
  ASSERT_EQ(nullptr, array_of_values->at(3));
  delete json;
}

TEST(TJValueArray, TryingToGetANegativeItemReturnsNull) {
  auto json = TinyJSON::TJ::parse(R"(
[
  12, 13, 14
]
)"
);
  ASSERT_NE(nullptr, json);
  auto array_of_values = dynamic_cast<TinyJSON::TJValueArray*>(json);
  ASSERT_EQ(3, array_of_values->get_number_of_items());

  ASSERT_NE(nullptr, array_of_values->at(0));
  ASSERT_EQ(nullptr, array_of_values->at(-1));
  ASSERT_EQ(nullptr, array_of_values->at(-42));
  delete json;
}

TEST(TJValueArray, ItemsInArrayMustBeSeparatedByCommaNumbers) {
  auto json = TinyJSON::TJ::parse(R"(
[
  12
  13
]
)"
);
  ASSERT_EQ(nullptr, json);
}

TEST(TJValueArray, ItemsInArrayMustBeSeparatedByCommaWithStrings) {
  auto json = TinyJSON::TJ::parse(R"(
[
  "A"
  "B"
]
)"
);
  ASSERT_EQ(nullptr, json);
}

TEST(TJValueArray, ItemsInArrayMustBeSeparatedByCommaWithNumbersAndStrings) {
  auto json = TinyJSON::TJ::parse(R"(
[
  12
  "B"
]
)"
);
  ASSERT_EQ(nullptr, json);
}

TEST(TJValueArray, ArrayHasACommaButThenTheArrayEnds) {
  auto json = TinyJSON::TJ::parse(R"(
[
  12,
  "B",
]
)"
);
  ASSERT_EQ(nullptr, json);
}

TEST(TJValueArray, CreateWithNumbersAndStrings)
{
  auto json = new TinyJSON::TJValueArray();
  json->add_number(42);
  json->add_string("Hello");
  json->add_string("World");

  ASSERT_EQ(3, json->get_number_of_items());

  const auto& text = json->dump(TinyJSON::formating::minify);
  ASSERT_NE(nullptr, text);

  ASSERT_STREQ(R"([42,"Hello","World"])", text);

  delete json;
}

TEST(TJValueArray, CreateWithNegativeNumbers) 
{
  auto json = new TinyJSON::TJValueArray();
  json->add_number(42);
  json->add_number(0);
  json->add_number(-42);

  ASSERT_EQ(3, json->get_number_of_items());

  const auto& text = json->dump(TinyJSON::formating::minify);
  ASSERT_NE(nullptr, text);

  ASSERT_STREQ(R"([42,0,-42])", text);

  delete json;
}

TEST(TJValueArray, CreateWithBoolean)
{
  auto json = new TinyJSON::TJValueArray();
  json->add_boolean(true);
  json->add_boolean(0);
  json->add_boolean(false);

  ASSERT_EQ(3, json->get_number_of_items());

  const auto& text = json->dump(TinyJSON::formating::minify);
  ASSERT_NE(nullptr, text);

  ASSERT_STREQ(R"([true,false,false])", text);

  delete json;
}

TEST(TJValueArray, CreateWithFloats)
{
  auto json = new TinyJSON::TJValueArray();
  json->add_float(42.5);
  json->add_float(0.05);
  json->add_float(1.00);

  ASSERT_EQ(3, json->get_number_of_items());

  const auto& text = json->dump(TinyJSON::formating::minify);
  ASSERT_NE(nullptr, text);

  ASSERT_STREQ(R"([42.5,0.05,1])", text);

  delete json;
}

TEST(TJValueArray, CreateWithFloatsAndNegativeNumbers)
{
  auto json = new TinyJSON::TJValueArray();
  json->add_float(42.5);
  json->add_float(0.05);
  json->add_float(1.00);
  json->add_float(-1.00); //  this is actually a whole number
  json->add_float(-42.5);
  json->add_float(-0.05);

  ASSERT_EQ(6, json->get_number_of_items());

  const auto& text = json->dump(TinyJSON::formating::minify);
  ASSERT_NE(nullptr, text);

  ASSERT_STREQ(R"([42.5,0.05,1,-1,-42.5,-0.05])", text);

  delete json;
}

TEST(TJValueArray, AddVectorOfFloats)
{
  std::vector<float> values;
  values.push_back(42.5);
  values.push_back(1.00);
  values.push_back(-1.00); //  this is actually a whole number
  values.push_back(-42.5);

  auto json = new TinyJSON::TJValueArray();
  json->add_floats(values);

  ASSERT_EQ(4, json->get_number_of_items());

  const auto& text = json->dump(TinyJSON::formating::minify);
  ASSERT_NE(nullptr, text);

  ASSERT_STREQ(R"([42.5,1,-1,-42.5])", text);

  delete json;
}

TEST(TJValueArray, AddVectorOfDoubles)
{
  std::vector<double> values;
  values.push_back(42.5);
  values.push_back(0.05);
  values.push_back(1.00);
  values.push_back(-1.00); //  this is actually a whole number
  values.push_back(-42.5);
  values.push_back(-0.05);

  auto json = new TinyJSON::TJValueArray();
  json->add_floats(values);

  ASSERT_EQ(6, json->get_number_of_items());

  const auto& text = json->dump(TinyJSON::formating::minify);
  ASSERT_NE(nullptr, text);

  ASSERT_STREQ(R"([42.5,0.05,1,-1,-42.5,-0.05])", text);

  delete json;
}

TEST(TJValueArray, AddVectorOfLongDoubles)
{
  std::vector<long double> values;
  values.push_back(42.5);
  values.push_back(0.05);
  values.push_back(1.00);
  values.push_back(-1.00); //  this is actually a whole number
  values.push_back(-42.5);
  values.push_back(-0.05);

  auto json = new TinyJSON::TJValueArray();
  json->add_floats(values);

  ASSERT_EQ(6, json->get_number_of_items());

  const auto& text = json->dump(TinyJSON::formating::minify);
  ASSERT_NE(nullptr, text);

  ASSERT_STREQ(R"([42.5,0.05,1,-1,-42.5,-0.05])", text);

  delete json;
}

TEST(TJValueArray, AddVectorOfInt)
{
  std::vector<int> values;
  values.push_back(42);
  values.push_back(0);
  values.push_back(-42);

  auto json = new TinyJSON::TJValueArray();
  json->add_numbers(values);

  ASSERT_EQ(3, json->get_number_of_items());

  const auto& text = json->dump(TinyJSON::formating::minify);
  ASSERT_NE(nullptr, text);

  ASSERT_STREQ(R"([42,0,-42])", text);

  delete json;
}

TEST(TJValueArray, AddVectorOfLong)
{
  std::vector<long> values;
  values.push_back(42);
  values.push_back(0);
  values.push_back(-42);

  auto json = new TinyJSON::TJValueArray();
  json->add_numbers(values);

  ASSERT_EQ(3, json->get_number_of_items());

  const auto& text = json->dump(TinyJSON::formating::minify);
  ASSERT_NE(nullptr, text);

  ASSERT_STREQ(R"([42,0,-42])", text);

  delete json;
}

TEST(TJValueArray, AddVectorOfLongLong)
{
  std::vector<long long> values;
  values.push_back(42);
  values.push_back(0);
  values.push_back(-42);

  auto json = new TinyJSON::TJValueArray();
  json->add_numbers(values);

  ASSERT_EQ(3, json->get_number_of_items());

  const auto& text = json->dump(TinyJSON::formating::minify);
  ASSERT_NE(nullptr, text);

  ASSERT_STREQ(R"([42,0,-42])", text);

  delete json;
}

TEST(TJValueArray, AddVectorOfNumberAndGetItAsAVector)
{
  auto json = new TinyJSON::TJValueArray();
  json->add_number(42);
  json->add_number((long long)43);
  json->add_number((unsigned)44);
  json->add_number((long)45);
  json->add_number((signed)46);
  json->add_number((long)47);

  const auto& text = json->dump(TinyJSON::formating::minify);
  ASSERT_NE(nullptr, text);

  // the order should remain the same as the order of insertion
  // but it could change, if this fails we need to change the way we verify the output
  ASSERT_STREQ(R"([42,43,44,45,46,47])", text);
  delete json;
}