// Licensed to Florent Guelfucci under one or more agreements.
// Florent Guelfucci licenses this file to you under the MIT license.
// See the LICENSE file in the project root for more information.
#include <gtest/gtest.h>
#define TJ_USE_CHAR 1
#include "../src/TinyJSON.h"
#include "testshelper.h"

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

TEST(TestObjects, SetInteger)
{
  auto object = new TinyJSON::TJValueObject();
  object->set_number("a", 42);
  auto dump = object->dump();
  ASSERT_STREQ(dump, R"({
  "a": 42
})");
  delete object;
}

TEST(TestObjects, SetString)
{
  auto object = new TinyJSON::TJValueObject();
  object->set_string("a", "World");
  auto dump = object->dump();
  ASSERT_STREQ(dump, R"({
  "a": "World"
})");
  delete object;
}

TEST(TestObjects, SetBoolean)
{
  auto object = new TinyJSON::TJValueObject();
  object->set_boolean("a", true);
  object->set_boolean("b", false);
  auto dump = object->dump();
  ASSERT_STREQ(dump, R"({
  "a": true,
  "b": false
})");
  delete object;
}

TEST(TestObjects, SetNumber)
{
  auto object = new TinyJSON::TJValueObject();
  object->set_number("a", 42);
  object->set_number("b", -42);
  
  const auto& text = object->dump(TinyJSON::formating::minify);
  ASSERT_NE(nullptr, text);
  ASSERT_STREQ(R"({"a":42,"b":-42})", text);

  delete object;
}

TEST(TestObjects, SetFloats)
{
  auto object = new TinyJSON::TJValueObject();
  object->set_float("a", 42.0);
  object->set_float("b", -42.0);
  object->set_float("c", -00.012);

  const auto& text = object->dump(TinyJSON::formating::minify);
  ASSERT_NE(nullptr, text);
  ASSERT_STREQ(R"({"a":42,"b":-42,"c":-0.012})", text);

  delete object;
}

TEST(TestObjects, SetFloatsWithVectors)
{
  auto object = new TinyJSON::TJValueObject();

  std::vector<float> f_values = { 1.5f, 2.5f };
  object->set_floats("f", f_values);

  std::vector<double> d_values = { 3.5, 4.5 };
  object->set_floats("d", d_values);

  std::vector<long double> ld_values = { 5.5, 6.5 };
  object->set_floats("ld", ld_values);

  const auto& text = object->dump(TinyJSON::formating::minify);
  ASSERT_NE(nullptr, text);
  ASSERT_STREQ(R"({"f":[1.5,2.5],"d":[3.5,4.5],"ld":[5.5,6.5]})", text);

  delete object;
}

TEST(TestObjects, SetNumbersWithVectors)
{
  auto object = new TinyJSON::TJValueObject();

  std::vector<int> i_values = { 1, 2 };
  object->set_numbers("i", i_values);

  std::vector<long> l_values = { 3L, 4L };
  object->set_numbers("l", l_values);

  std::vector<long long> ll_values = { 5LL, 6LL };
  object->set_numbers("ll", ll_values);

  std::vector<unsigned int> ui_values = { 7u, 8u };
  object->set_numbers("ui", ui_values);

  std::vector<unsigned long long> ull_values = { 9ULL, 10ULL };
  object->set_numbers("ull", ull_values);

  std::vector<short> s_values = { 11, 12 };
  object->set_numbers("s", s_values);

  const auto& text = object->dump(TinyJSON::formating::minify);
  ASSERT_NE(nullptr, text);
  ASSERT_STREQ(R"({"i":[1,2],"l":[3,4],"ll":[5,6],"ui":[7,8],"ull":[9,10],"s":[11,12]})", text);

  delete object;
}

TEST(TestObjects, SetAValueDirectlyAsReference)
{
  auto object = new TinyJSON::TJValueObject();
  object->set("a", TinyJSON::TJValueBoolean(true));
  object->set("b", TinyJSON::TJValueBoolean(false));
  auto dump = object->dump();
  ASSERT_STREQ(dump, R"({
  "a": true,
  "b": false
})");
  delete object;
}

TEST(TestObjects, SetAValueDirectlyAsPointers)
{
  auto object = new TinyJSON::TJValueObject();
  auto t = new TinyJSON::TJValueBoolean(true);
  auto f = new TinyJSON::TJValueBoolean(false);
  object->set("a", t);
  object->set("b", f);

  // delete it before the dump.
  delete f;
  delete t;

  auto dump = object->dump();
  ASSERT_STREQ(dump, R"({
  "a": true,
  "b": false
})");
  delete object;
}

TEST(TestObjects, PopWithNoItems)
{
  auto object = new TinyJSON::TJValueObject();
  ASSERT_NO_THROW(object->pop("a"));

  auto dump = object->dump();
  ASSERT_STREQ(dump, R"({})");
  delete object;
}

TEST(TestObjects, PopAKeyThatDoesNotExist)
{
  auto object = new TinyJSON::TJValueObject();
  object->set_string("a", "World");

  ASSERT_NO_THROW(object->pop("b"));

  auto dump = object->dump();
  ASSERT_STREQ(dump, R"({
  "a": "World"
})");
  delete object;
}

TEST(TestObjects, PopAKeyThatDoesExist)
{
  auto object = new TinyJSON::TJValueObject();
  object->set_string("a", "Hello");
  object->set_string("b", "World");
  object->set_string("c", "Bye");

  auto dump = object->dump();
  ASSERT_STREQ(dump, R"({
  "a": "Hello",
  "b": "World",
  "c": "Bye"
})");

  ASSERT_NO_THROW(object->pop("b"));

  dump = object->dump();
  ASSERT_STREQ(dump, R"({
  "a": "Hello",
  "c": "Bye"
})");
  delete object;
}

TEST(TestObjects, PopAllTheItems)
{
  auto object = new TinyJSON::TJValueObject();
  object->set_string("a", "Hello");
  object->set_string("b", "World");
  object->set_string("c", "Bye");

  auto dump = object->dump();
  ASSERT_STREQ(dump, R"({
  "a": "Hello",
  "b": "World",
  "c": "Bye"
})");

  ASSERT_NO_THROW(object->pop("b"));
  ASSERT_NO_THROW(object->pop("a"));
  ASSERT_NO_THROW(object->pop("c"));

  dump = object->dump();
  ASSERT_STREQ(dump, R"({})");
  delete object;
}

TEST(TestObjects, PopTheLastItem)
{
  auto object = new TinyJSON::TJValueObject();
  object->set_string("a", "Hello");
  object->set_string("b", "World");
  object->set_string("c", "Bye");

  auto dump = object->dump();
  ASSERT_STREQ(dump, R"({
  "a": "Hello",
  "b": "World",
  "c": "Bye"
})");

  ASSERT_NO_THROW(object->pop("c"));

  dump = object->dump();
  ASSERT_STREQ(dump, R"({
  "a": "Hello",
  "b": "World"
})");
  delete object;
}

TEST(TestObjects, CaseInSensitiveSearch) {
  auto json = TinyJSON::TJ::parse(R"(
    {
      "Hello" : 12,
      "WORld" : 14
    }
    )"
  );

  ASSERT_NE(nullptr, json);
  ASSERT_NE(nullptr, dynamic_cast<TinyJSON::TJValue*>(json));

  const auto jobject = dynamic_cast<TinyJSON::TJValueObject*>(json);
  ASSERT_NE(nullptr, jobject);
  ASSERT_EQ(2, jobject->get_number_of_items());

  const auto a = dynamic_cast<const TinyJSON::TJValueNumberInt*>(jobject->try_get_value("hello", false));
  ASSERT_NE(nullptr, a);
  ASSERT_EQ(12, a->get_number());

  const auto a1 = dynamic_cast<const TinyJSON::TJValueNumberInt*>(jobject->try_get_value("hello", true));
  ASSERT_EQ(nullptr, a1);

  const auto b = dynamic_cast<const TinyJSON::TJValueNumberInt*>(jobject->try_get_value("world", false));
  ASSERT_NE(nullptr, b);
  ASSERT_EQ(14, b->get_number());

  const auto b1 = dynamic_cast<const TinyJSON::TJValueNumberInt*>(jobject->try_get_value("world", true));
  ASSERT_EQ(nullptr, b1);
  delete json;
}

TEST(TestObjects, AddAnArrayToObject)
{
  auto object = new TinyJSON::TJValueObject();
  auto array = new TinyJSON::TJValueArray();
  array->add_number(1);
  array->add_number(2);
  array->add_number(3);
  object->set("a", array);
  delete array;

  const auto& text = object->dump(TinyJSON::formating::minify);
  ASSERT_NE(nullptr, text);
  ASSERT_STREQ(R"({"a":[1,2,3]})", text);

  delete object;
}

TEST(TestObjects, AddMultipleArraysToObject)
{
  auto object = new TinyJSON::TJValueObject();
  auto array_out = new TinyJSON::TJValueArray();

  auto array1 = new TinyJSON::TJValueArray();
  array1->add_number(1);
  array1->add_number(2);
  array1->add_number(3);
  auto array2 = new TinyJSON::TJValueArray();
  array2->add_number(4);
  array2->add_number(5);
  array2->add_number(6);
  array_out->add(array1);
  array_out->add(array2);
  object->set("a", array_out);
  delete array1;
  delete array2;
  delete array_out;

  const auto& text = object->dump(TinyJSON::formating::minify);
  ASSERT_NE(nullptr, text);
  ASSERT_STREQ(R"({"a":[[1,2,3],[4,5,6]]})", text);

  delete object;
}

// Float
TEST(TestObjects, GetFloatFromObjectCaseIsCorrect)
{
  auto object = new TinyJSON::TJValueObject();
  object->set_float("a", 123.4);

  auto f = object->get_float("a");
  ASSERT_DOUBLE_EQ(123.4, f);

  delete object;
}

TEST(TestObjects, GetFloatFromObjectThatIsInteger)
{
  auto object = new TinyJSON::TJValueObject();
  object->set_number("a", 123);

  auto f = object->get_float("a");
  ASSERT_DOUBLE_EQ(123, f);

  delete object;
}

TEST(TestObjects, GetFloatFromObjectThatIsBoolean)
{
  auto object = new TinyJSON::TJValueObject();
  object->set_boolean("a", true);
  object->set_boolean("b", false);

  auto a = object->get_float("a");
  auto b = object->get_float("b");
  ASSERT_DOUBLE_EQ(1.0, a);
  ASSERT_DOUBLE_EQ(0.0, b);

  delete object;
}

TEST(TestObjects, GetFloatFromObjectCaseIsIncorrect)
{
  auto object = new TinyJSON::TJValueObject();
  object->set_float("a", 123.4);

  ASSERT_ANY_THROW({auto f = object->get_float("A", true, true); });
  // We expect an exception here because the case is incorrect and we asked to throw.
  delete object;
}

TEST(TestObjects, GetFloatFromObjectCaseIsIncorrectButDontThrow)
{
  auto object = new TinyJSON::TJValueObject();
  object->set_float("a", 123.4);

  ASSERT_NO_THROW({ 
    auto f = object->get_float("A", true, false); 
    ASSERT_DOUBLE_EQ(0.0, f);
    });
  delete object;
}

TEST(TestObjects, GetFloatFromObjectCaseIsIncorrectButWeDontCare)
{
  auto object = new TinyJSON::TJValueObject();
  object->set_float("a", 123.4);

  ASSERT_NO_THROW({
    auto f = object->get_float("A", true, false);
    ASSERT_DOUBLE_EQ(0.0, f);
    });

  delete object;
}

TEST(TestObjects, GetFloatFromObjectThatIsAStringNoThrow)
{
  auto object = new TinyJSON::TJValueObject();
  object->set_string("a", "not a number");

  ASSERT_ANY_THROW({
    auto f = object->get_float("a", false, false);
    });

  delete object;
}

TEST(TestObjects, GetFloatFromObjectThatIsAStringThrow)
{
  auto object = new TinyJSON::TJValueObject();
  object->set_string("a", "not a number");

  ASSERT_ANY_THROW({
    auto f = object->get_float("a", false, true);
    });

  delete object;
}

// number
TEST(TestObjects, GetNumberFromObjectCaseIsCorrect)
{
  auto object = new TinyJSON::TJValueObject();
  object->set_number("a", 123);

  auto n = object->get_number("a");
  ASSERT_EQ(123, n);

  delete object;
}

TEST(TestObjects, GetNumberFromObjectThatIsFloat)
{
  auto object = new TinyJSON::TJValueObject();
  object->set_float("a", 123.4);

  auto n = object->get_number("a");
  ASSERT_EQ(123, n);

  delete object;
}

TEST(TestObjects, GetNumberFromObjectThatIsBoolean)
{
  auto object = new TinyJSON::TJValueObject();
  object->set_boolean("a", true);
  object->set_boolean("b", false);

  auto a = object->get_number("a");
  auto b = object->get_number("b");
  ASSERT_EQ(1, a);
  ASSERT_EQ(0, b);

  delete object;
}

TEST(TestObjects, GetNumberFromObjectCaseIsIncorrect)
{
  auto object = new TinyJSON::TJValueObject();
  object->set_number("a", 123);

  ASSERT_ANY_THROW({ auto f = object->get_number("A", true, true); });
  // We expect an exception here because the case is incorrect and we asked to throw.
  delete object;
}

TEST(TestObjects, GetNumberFromObjectCaseIsIncorrectButDontThrow)
{
  auto object = new TinyJSON::TJValueObject();
  object->set_number("a", 123.4);

  ASSERT_NO_THROW({
    auto n = object->get_number("A", true, false);
    ASSERT_EQ(0, n);
    });
  delete object;
}

TEST(TestObjects, GetNumberFromObjectCaseIsIncorrectButWeDontCare)
{
  auto object = new TinyJSON::TJValueObject();
  object->set_number("a", 123);

  ASSERT_NO_THROW({
    auto n = object->get_number("A", true, false);
    ASSERT_EQ(0, n);
    });

  delete object;
}

TEST(TestObjects, GetNumberFromObjectThatIsAStringNoThrow)
{
  auto object = new TinyJSON::TJValueObject();
  object->set_string("a", "not a number");

  ASSERT_ANY_THROW({
    auto n = object->get_number("a", false, false);
    });

  delete object;
}

TEST(TestObjects, GetNumberFromObjectThatIsAStringThrow)
{
  auto object = new TinyJSON::TJValueObject();
  object->set_string("a", "not a number");

  ASSERT_ANY_THROW({
    auto n = object->get_number("a", false, true);
    });

  delete object;
}

// Boolean
TEST(TestObjects, GetBooleanFromObjectCaseIsCorrect)
{
  auto object = new TinyJSON::TJValueObject();
  object->set_boolean("a", true);
  object->set_boolean("b", false);

  auto a = object->get_boolean("a");
  auto b = object->get_boolean("b");
  ASSERT_TRUE(a);
  ASSERT_FALSE(b);

  delete object;
}

TEST(TestObjects, GetBooleanFromObjectThatIsInteger)
{
  auto object = new TinyJSON::TJValueObject();
  object->set_number("a", 1);
  object->set_number("b", 0);

  auto a = object->get_boolean("a");
  auto b = object->get_boolean("b");
  ASSERT_TRUE(a);
  ASSERT_FALSE(b);

  delete object;
}

TEST(TestObjects, GetBooleanFromObjectThatIsFloat)
{
  auto object = new TinyJSON::TJValueObject();
  object->set_float("a", 1.0);
  object->set_float("b", 0.0);

  auto a = object->get_boolean("a");
  auto b = object->get_boolean("b");
  ASSERT_TRUE(a);
  ASSERT_FALSE(b);

  delete object;
}

TEST(TestObjects, GetBooleanFromObjectCaseIsIncorrect)
{
  auto object = new TinyJSON::TJValueObject();
  object->set_boolean("a", true);

  ASSERT_ANY_THROW({ auto b = object->get_boolean("A", true, true); });
  // We expect an exception here because the case is incorrect and we asked to throw.
  delete object;
}

TEST(TestObjects, GetBooleanFromObjectCaseIsIncorrectButDontThrow)
{
  auto object = new TinyJSON::TJValueObject();
  object->set_boolean("a", true);

  ASSERT_NO_THROW({
    auto b = object->get_boolean("A", true, false);
    ASSERT_FALSE(b);
    });
  delete object;
}

TEST(TestObjects, GetBooleanFromObjectCaseIsIncorrectButWeDontCare)
{
  auto object = new TinyJSON::TJValueObject();
  object->set_boolean("a", true);

  ASSERT_NO_THROW({
    auto b = object->get_boolean("A", true, false);
    ASSERT_FALSE(b);
    });

  delete object;
}

TEST(TestObjects, GetBooleanFromObjectThatIsAStringNoThrow)
{
  auto object = new TinyJSON::TJValueObject();
  object->set_string("a", "not a number");

  ASSERT_ANY_THROW({
    auto b = object->get_boolean("a", false, false);  //  strings are not supported.
    });

  delete object;
}

TEST(TestObjects, GetBooleanFromObjectNullIsFalse)
{
  auto object = new TinyJSON::TJValueObject();
  object->set_null("a");

  ASSERT_NO_THROW({
    auto b = object->get_boolean("a", false, false);
    ASSERT_FALSE(b);
    });

  delete object;
}

// Floats
TEST(TestObjects, GetFloatsFromObjectCaseIsCorrect)
{
  auto object = new TinyJSON::TJValueObject();
  object->set_floats("a", { 123.4, 42.7 });

  auto fs = object->get_floats("a");
  ASSERT_TRUE(fs.size() == 2);

  for( auto f : fs ) {
   ASSERT_TRUE(f == 123.4 || f == 42.7);
  }
  delete object;
}

TEST(TestObjects, GetFloatsFromObjectCaseIsincorrectButWeDontCare)
{
  auto object = new TinyJSON::TJValueObject();
  object->set_floats("a", { 123.25, 42.7 });

  ASSERT_NO_THROW(
    {
    auto fs = object->get_floats("A", true, false);
    ASSERT_TRUE(fs.size() == 0);
    });
  delete object;
}

TEST(TestObjects, GetFloatsFromObjectCaseIsincorrectWeThrow)
{
  auto object = new TinyJSON::TJValueObject();
  object->set_floats("a", { 123.25, 42.7 });

  ASSERT_ANY_THROW(
    {
    auto fs = object->get_floats("A", true, true);
    });
  delete object;
}

TEST(TestObjects, GetFloatsFromObjectJustOneNUmber)
{
  auto object = new TinyJSON::TJValueObject();
  object->set_float("a", 123.7);

  auto fs = object->get_floats("a");
  ASSERT_TRUE(fs.size() == 1);

  for (auto f : fs) {
    ASSERT_DOUBLE_EQ(123.7, f);
  }
  delete object;
}

TEST(TestObjects, GetFloatsFromObjectJustOneBoolean)
{
  auto object = new TinyJSON::TJValueObject();
  object->set_boolean("a", true);
  object->set_boolean("b", false);

  auto as = object->get_floats("a");
  ASSERT_TRUE(as.size() == 1);
  auto bs = object->get_floats("b");
  ASSERT_TRUE(bs.size() == 1);

  for (auto a : as) {
    ASSERT_DOUBLE_EQ(1.0, a);
  }
  for (auto b : bs) {
    ASSERT_DOUBLE_EQ(0.0, b);
  }
  delete object;
}

// Numbers
TEST(TestObjects, GetNumbersFromObjectCaseIsCorrect)
{
  auto object = new TinyJSON::TJValueObject();
  object->set_numbers("a", { 123, 42 });

  auto fs = object->get_numbers("a");
  ASSERT_TRUE(fs.size() == 2);

  for (auto f : fs) {
    ASSERT_TRUE(f == 123 || f == 42);
  }
  delete object;
}

TEST(TestObjects, GetNumbersFromObjectCaseIsincorrectButWeDontCare)
{
  auto object = new TinyJSON::TJValueObject();
  object->set_numbers("a", { 123, 42 });

  ASSERT_NO_THROW(
    {
    auto fs = object->get_numbers("A", true, false);
    ASSERT_TRUE(fs.size() == 0);
    });
  delete object;
}

TEST(TestObjects, GetNumbersFromObjectCaseIsincorrectWeThrow)
{
  auto object = new TinyJSON::TJValueObject();
  object->set_numbers("a", { 123, 42 });

  ASSERT_ANY_THROW(
    {
    auto fs = object->get_numbers("A", true, true);
    });
  delete object;
}

TEST(TestObjects, GetNumbersFromObjectJustOneNUmber)
{
  auto object = new TinyJSON::TJValueObject();
  object->set_number("a", 123 );

  auto fs = object->get_numbers("a");
  ASSERT_TRUE(fs.size() == 1);

  for (auto f : fs) {
    ASSERT_TRUE(f == 123);
  }
  delete object;
}

TEST(TestObjects, GetNumbersFromObjectJustOneBoolean)
{
  auto object = new TinyJSON::TJValueObject();
  object->set_boolean("a", true);
  object->set_boolean("b", false);

  auto as = object->get_numbers("a");
  ASSERT_TRUE(as.size() == 1);
  auto bs = object->get_numbers("b");
  ASSERT_TRUE(bs.size() == 1);

  for (auto a : as) {
    ASSERT_EQ(1, a);
  }
  for (auto b : bs) {
    ASSERT_EQ(0, b);
  }
  delete object;
}


TEST(TestObjects, GetStringFromObjectCaseInsensitive)
{
  auto object = new TinyJSON::TJValueObject();
  object->set_null("a");

  ASSERT_NO_THROW({
    auto b = object->get_string("a", false);
    ASSERT_STRCASEEQ("null", b);
    });

  delete object;
}

TEST(TestObjects, GetStringFromObjectCaseSensitive)
{
  auto object = new TinyJSON::TJValueObject();
  object->set_null("a");

  ASSERT_ANY_THROW({
    auto b = object->get_string("A", true, true);
    });

  delete object;
}

TEST(TestObjects, GetStringFromObjectCaseSensitiveNoThrow)
{
  auto object = new TinyJSON::TJValueObject();
  object->set_null("a");

  ASSERT_NO_THROW({
    auto b = object->get_string("A", true, false);
    ASSERT_STRCASEEQ("", b);
    });

  delete object;
}

TEST(TestObjects, GetFloatsAsVariousTypes)
{
  auto object = new TinyJSON::TJValueObject();
  object->set_floats("a", { 123.4, 42.7 });

  // double
  auto fds = object->get_floats<double>("a");
  ASSERT_TRUE(fds.size() == 2);

  for (auto f : fds) {
    ASSERT_TRUE(areDoublesEqual(f, 123.4) || areDoublesEqual(f, 42.7));
  }
  // float
  auto ffs = object->get_floats<float>("a");
  ASSERT_TRUE(ffs.size() == 2);

  for (auto f : ffs) {
    ASSERT_TRUE(areFloatsEqual(f, 123.4) || areFloatsEqual(f, 42.7));
  }
  delete object;
}

TEST(TestObjects, GetNumbersAsVariousTypes)
{
  auto object = new TinyJSON::TJValueObject();
  object->set_numbers("a", { 123, 42 });

  // int
  auto fints = object->get_numbers<int>("a");
  ASSERT_TRUE(fints.size() == 2);

  for (auto f : fints) {
    ASSERT_TRUE(f ==123 || f == 42);
  }

  // short
  auto fs = object->get_numbers<short>("a");
  ASSERT_TRUE(fs.size() == 2);

  for (auto f : fs) {
    ASSERT_TRUE(f == 123 || f == 42);
  }

  // long
  auto fl = object->get_numbers<long>("a");
  ASSERT_TRUE(fl.size() == 2);

  for (auto f : fl) {
    ASSERT_TRUE(f == 123 || f == 42);
  }
  delete object;
}

TEST(TestObjects, CloneMemoryLeaks)
{
  auto object = new TinyJSON::TJValueObject();
  object->set_string("a", "Hello");
  object->set_string("b", "World");
  object->set_string("c", "Bye");
  auto clone = object->clone();
  delete object;
  delete clone;
}
