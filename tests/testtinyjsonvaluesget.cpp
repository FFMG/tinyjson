// Licensed to Florent Guelfucci under one or more agreements.
// Florent Guelfucci licenses this file to you under the MIT license.
// See the LICENSE file in the project root for more information.
#include <gtest/gtest.h>
#ifndef TJ_INCLUDE_STD_STRING
#define TJ_INCLUDE_STD_STRING 1
#endif // !TJ_INCLUDE_STD_STRING

#include "../src/TinyJSON.h"

#include <gtest/gtest.h>

TEST(TestValueGet, GetBoolean)
{
  auto json = TinyJSON::TJ::parse(R"(
    {
      "a": true,
      "b": 123,
      "c": 0,
      "d": 42.2,
      "e": "This is a string",
      "f": null 
    }
    )"
  );
  ASSERT_NE(nullptr, json);
  auto jobject = dynamic_cast<TinyJSON::TJValueObject*>(json);
  ASSERT_NE(nullptr, jobject);

  auto valuea = jobject->try_get_value("a");
  auto valueb = jobject->try_get_value("b");
  auto valuec = jobject->try_get_value("c");
  auto valued = jobject->try_get_value("d");
  auto valuee = jobject->try_get_value("e");
  auto valuef = jobject->try_get_value("f");

  ASSERT_TRUE(valuea->get_boolean());
  ASSERT_TRUE(valueb->get_boolean());
  ASSERT_FALSE(valuec->get_boolean());
  ASSERT_TRUE(valued->get_boolean());
  ASSERT_ANY_THROW(valuee->get_boolean());
  ASSERT_FALSE(valuef->get_boolean());  //  null is false

  delete json;
}

TEST(TestValueGet, GetStrictBoolean)
{
  auto json = TinyJSON::TJ::parse(R"(
    {
      "a": true,
      "b": 123,
      "c": 0,
      "d": 42.2,
      "e": "This is a string",
      "f": null 
    }
    )"
  );
  ASSERT_NE(nullptr, json);
  auto jobject = dynamic_cast<TinyJSON::TJValueObject*>(json);
  ASSERT_NE(nullptr, jobject);

  auto valuea = jobject->try_get_value("a");
  auto valueb = jobject->try_get_value("b");
  auto valuec = jobject->try_get_value("c");
  auto valued = jobject->try_get_value("d");
  auto valuee = jobject->try_get_value("e");
  auto valuef = jobject->try_get_value("f");

  ASSERT_TRUE(valuea->get_boolean(true));
  ASSERT_ANY_THROW(valueb->get_boolean(true));
  ASSERT_ANY_THROW(valuec->get_boolean(true));
  ASSERT_ANY_THROW(valued->get_boolean(true));
  ASSERT_ANY_THROW(valuee->get_boolean(true));
  ASSERT_ANY_THROW(valuef->get_boolean(true));

  delete json;
}

TEST(TestValueGet, GetNumber)
{
  auto json = TinyJSON::TJ::parse(R"(
    {
      "a": true,
      "b": 123,
      "c": 0,
      "d": 42.2,
      "e": "This is a string",
      "f": null 
    }
    )"
  );
  ASSERT_NE(nullptr, json);
  auto jobject = dynamic_cast<TinyJSON::TJValueObject*>(json);
  ASSERT_NE(nullptr, jobject);

  auto valuea = jobject->try_get_value("a");
  auto valueb = jobject->try_get_value("b");
  auto valuec = jobject->try_get_value("c");
  auto valued = jobject->try_get_value("d");
  auto valuee = jobject->try_get_value("e");
  auto valuef = jobject->try_get_value("f");

  ASSERT_EQ(1, valuea->get_number());
  ASSERT_EQ(123, valueb->get_number());
  ASSERT_EQ(0, valuec->get_number());
  ASSERT_EQ(42, valued->get_number());
  ASSERT_ANY_THROW(valuee->get_number());
  ASSERT_EQ(0, valuef->get_number());  //  null is zero

  delete json;
}

TEST(TestValueGet, GetStrictNumber)
{
  auto json = TinyJSON::TJ::parse(R"(
    {
      "a": true,
      "b": 123,
      "c": 0,
      "d": 42.2,
      "e": "This is a string",
      "f": null 
    }
    )"
  );
  ASSERT_NE(nullptr, json);
  auto jobject = dynamic_cast<TinyJSON::TJValueObject*>(json);
  ASSERT_NE(nullptr, jobject);

  auto valuea = jobject->try_get_value("a");
  auto valueb = jobject->try_get_value("b");
  auto valuec = jobject->try_get_value("c");
  auto valued = jobject->try_get_value("d");
  auto valuee = jobject->try_get_value("e");
  auto valuef = jobject->try_get_value("f");

  ASSERT_ANY_THROW(valuea->get_number(true));
  ASSERT_EQ(123, valueb->get_number(true));
  ASSERT_EQ(0, valuec->get_number(true));
  ASSERT_EQ(42, valued->get_number(true));
  ASSERT_ANY_THROW(valuee->get_number(true));
  ASSERT_ANY_THROW(valuef->get_number(true));

  delete json;
}

TEST(TestValueGet, GetFloat)
{
  auto json = TinyJSON::TJ::parse(R"(
    {
      "a": true,
      "b": 123,
      "c": 0,
      "d": 42.2,
      "e": "This is a string",
      "f": null 
    }
    )"
  );
  ASSERT_NE(nullptr, json);
  auto jobject = dynamic_cast<TinyJSON::TJValueObject*>(json);
  ASSERT_NE(nullptr, jobject);

  auto valuea = jobject->try_get_value("a");
  auto valueb = jobject->try_get_value("b");
  auto valuec = jobject->try_get_value("c");
  auto valued = jobject->try_get_value("d");
  auto valuee = jobject->try_get_value("e");
  auto valuef = jobject->try_get_value("f");

  ASSERT_DOUBLE_EQ(1.0, valuea->get_float());
  ASSERT_DOUBLE_EQ(123.0, valueb->get_float());
  ASSERT_DOUBLE_EQ(0.0, valuec->get_float());
  ASSERT_DOUBLE_EQ(42.2, valued->get_float());
  ASSERT_ANY_THROW(valuee->get_float());
  ASSERT_DOUBLE_EQ(0.0, valuef->get_float());  //  null is zero

  delete json;
}

TEST(TestValueGet, GetStrictFloat)
{
  auto json = TinyJSON::TJ::parse(R"(
    {
      "a": true,
      "b": 123,
      "c": 0,
      "d": 42.2,
      "e": "This is a string",
      "f": null 
    }
    )"
  );
  ASSERT_NE(nullptr, json);
  auto jobject = dynamic_cast<TinyJSON::TJValueObject*>(json);
  ASSERT_NE(nullptr, jobject);

  auto valuea = jobject->try_get_value("a");
  auto valueb = jobject->try_get_value("b");
  auto valuec = jobject->try_get_value("c");
  auto valued = jobject->try_get_value("d");
  auto valuee = jobject->try_get_value("e");
  auto valuef = jobject->try_get_value("f");

  ASSERT_ANY_THROW(valuea->get_float(true));
  ASSERT_DOUBLE_EQ(123.0, valueb->get_float(true));
  ASSERT_DOUBLE_EQ(0.0, valuec->get_float(true));
  ASSERT_DOUBLE_EQ(42.2, valued->get_float(true));
  ASSERT_ANY_THROW(valuee->get_float(true));
  ASSERT_ANY_THROW(valuef->get_float(true));

  delete json;
}

TEST(TestValueGet, GetString)
{
  auto json = TinyJSON::TJ::parse(R"(
    {
      "a": true,
      "b": 123,
      "c": 0,
      "d": 42.2,
      "e": "This is a string",
      "f": null 
    }
    )"
  );
  ASSERT_NE(nullptr, json);
  auto jobject = dynamic_cast<TinyJSON::TJValueObject*>(json);
  ASSERT_NE(nullptr, jobject);

  auto valuea = jobject->try_get_value("a");
  auto valueb = jobject->try_get_value("b");
  auto valuec = jobject->try_get_value("c");
  auto valued = jobject->try_get_value("d");
  auto valuee = jobject->try_get_value("e");
  auto valuef = jobject->try_get_value("f");

  ASSERT_STRCASEEQ("true", valuea->get_string());
  ASSERT_STRCASEEQ("123", valueb->get_string());
  ASSERT_STRCASEEQ("0", valuec->get_string());
  ASSERT_STRCASEEQ("42.2", valued->get_string());
  ASSERT_STRCASEEQ("This is a string", valuee->get_string());
  ASSERT_STRCASEEQ("null", valuef->get_string());

  delete json;
}

TEST(TestValueGet, GetStrictString)
{
  auto json = TinyJSON::TJ::parse(R"(
    {
      "a": true,
      "b": 123,
      "c": 0,
      "d": 42.2,
      "e": "This is a string",
      "f": null 
    }
    )"
  );
  ASSERT_NE(nullptr, json);
  auto jobject = dynamic_cast<TinyJSON::TJValueObject*>(json);
  ASSERT_NE(nullptr, jobject);

  auto valuea = jobject->try_get_value("a");
  auto valueb = jobject->try_get_value("b");
  auto valuec = jobject->try_get_value("c");
  auto valued = jobject->try_get_value("d");
  auto valuee = jobject->try_get_value("e");
  auto valuef = jobject->try_get_value("f");

  ASSERT_ANY_THROW(valuea->get_string(true));
  ASSERT_ANY_THROW(valueb->get_string(true));
  ASSERT_ANY_THROW(valuec->get_string(true));
  ASSERT_ANY_THROW(valued->get_string(true));
  ASSERT_STRCASEEQ("This is a string", valuee->get_string(true));
  ASSERT_ANY_THROW(valuef->get_string(true));

  delete json;
}

TEST(TestValueGet, GetStrictStringFromObjectWillThrow)
{
  auto json = TinyJSON::TJ::parse(R"(
    {
      "a": true,
      "b": 123,
      "c": 0,
      "d": 42.2,
      "e": "This is a string",
      "f": null 
    }
    )"
  );
  ASSERT_NE(nullptr, json);

  ASSERT_ANY_THROW(json->get_string());

  delete json;
}

TEST(TestValueGet, GetStrictStringFromArrayWillThrow)
{
  auto json = TinyJSON::TJ::parse(R"(
    [1,2,3,4,5]
    )"
  );
  ASSERT_NE(nullptr, json);

  ASSERT_ANY_THROW(json->get_string());

  delete json;
}