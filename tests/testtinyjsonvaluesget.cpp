// Licensed to Florent Guelfucci under one or more agreements.
// Florent Guelfucci licenses this file to you under the MIT license.
// See the LICENSE file in the project root for more information.
#include <gtest/gtest.h>
#ifndef TJ_INCLUDE_STD_STRING
#define TJ_INCLUDE_STD_STRING 1
#endif // !TJ_INCLUDE_STD_STRING

#include "../src/TinyJSON.h"
#include "testshelper.h"

#include <gtest/gtest.h>

TEST(TestValueGet, GetBoolean)
{
  TinyJSON::parse_options options = {};
  options.throw_exception = true;
  options.strict = false;
  auto json = TinyJSON::TJ::parse(R"(
    {
      "a": true,
      "b": 123,
      "c": 0,
      "d": 42.2,
      "e": "This is a string",
      "f": null
    }
    )", options
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
  TinyJSON::parse_options options = {};
  options.throw_exception = true;
  options.strict = true;
  auto json = TinyJSON::TJ::parse(R"(
    {
      "a": true,
      "b": 123,
      "c": 0,
      "d": 42.2,
      "e": "This is a string",
      "f": null 
    }
    )", options
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
  ASSERT_ANY_THROW(valueb->get_boolean());
  ASSERT_ANY_THROW(valuec->get_boolean());
  ASSERT_ANY_THROW(valued->get_boolean());
  ASSERT_ANY_THROW(valuee->get_boolean());
  ASSERT_ANY_THROW(valuef->get_boolean());

  delete json;
}

TEST(TestValueGet, GetNumber)
{
  TinyJSON::parse_options options = {};
  options.throw_exception = true;
  options.strict = false;
  auto json = TinyJSON::TJ::parse(R"(
    {
      "a": true,
      "b": 123,
      "c": 0,
      "d": 42.2,
      "e": "This is a string",
      "f": null 
    }
    )", options
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
  TinyJSON::parse_options options = {};
  options.throw_exception = true;
  options.strict = true;
  auto json = TinyJSON::TJ::parse(R"(
    {
      "a": true,
      "b": 123,
      "c": 0,
      "d": 42.2,
      "e": "This is a string",
      "f": null 
    }
    )", options
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

  ASSERT_ANY_THROW(valuea->get_number());
  ASSERT_EQ(123, valueb->get_number());
  ASSERT_EQ(0, valuec->get_number());
  ASSERT_EQ(42, valued->get_number());
  ASSERT_ANY_THROW(valuee->get_number());
  ASSERT_ANY_THROW(valuef->get_number());

  delete json;
}

TEST(TestValueGet, GetFloat)
{
  TinyJSON::parse_options options = {};
  options.throw_exception = true;
  options.strict = false;
  auto json = TinyJSON::TJ::parse(R"(
    {
      "a": true,
      "b": 123,
      "c": 0,
      "d": 42.2,
      "e": "This is a string",
      "f": null 
    }
    )", options
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
  TinyJSON::parse_options options = {};
  options.throw_exception = true;
  options.strict = true;
  auto json = TinyJSON::TJ::parse(R"(
    {
      "a": true,
      "b": 123,
      "c": 0,
      "d": 42.2,
      "e": "This is a string",
      "f": null 
    }
    )", options
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

  ASSERT_ANY_THROW(valuea->get_float());
  ASSERT_DOUBLE_EQ(123.0, valueb->get_float());
  ASSERT_DOUBLE_EQ(0.0, valuec->get_float());
  ASSERT_DOUBLE_EQ(42.2, valued->get_float());
  ASSERT_ANY_THROW(valuee->get_float());
  ASSERT_ANY_THROW(valuef->get_float());

  delete json;
}

TEST(TestValueGet, GetString)
{
  TinyJSON::parse_options options = {};
  options.throw_exception = true;
  options.strict = false;
  auto json = TinyJSON::TJ::parse(R"(
    {
      "a": true,
      "b": 123,
      "c": 0,
      "d": 42.2,
      "e": "This is a string",
      "f": null 
    }
    )", options
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
  TinyJSON::parse_options options = {};
  options.throw_exception = true;
  options.strict = true;
  auto json = TinyJSON::TJ::parse(R"(
    {
      "a": true,
      "b": 123,
      "c": 0,
      "d": 42.2,
      "e": "This is a string",
      "f": null 
    }
    )", options
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

  ASSERT_ANY_THROW(valuea->get_string());
  ASSERT_ANY_THROW(valueb->get_string());
  ASSERT_ANY_THROW(valuec->get_string());
  ASSERT_ANY_THROW(valued->get_string());
  ASSERT_STRCASEEQ("This is a string", valuee->get_string());
  ASSERT_ANY_THROW(valuef->get_string());

  delete json;
}

TEST(TestValueGet, GetStrictStringFromObjectWillThrow)
{
  TinyJSON::parse_options options = {};
  options.throw_exception = true;
  options.strict = true;
  auto json = TinyJSON::TJ::parse(R"(
    {
      "a": true,
      "b": 123,
      "c": 0,
      "d": 42.2,
      "e": "This is a string",
      "f": null 
    }
    )", options
  );
  ASSERT_NE(nullptr, json);

  ASSERT_ANY_THROW(json->get_string());

  delete json;
}

TEST(TestValueGet, GetStrictStringFromArrayWillThrow)
{
  TinyJSON::parse_options options = {};
  options.throw_exception = true;
  options.strict = true;
  auto json = TinyJSON::TJ::parse(R"(
    [1,2,3,4,5]
    )", options
  );
  ASSERT_NE(nullptr, json);

  ASSERT_ANY_THROW(json->get_string());

  delete json;
}

TEST(TestOperatorAccess, AccessByKeyAndIndex)
{
  TinyJSON::parse_options options = {};
  options.throw_exception = true;
  options.strict = false;
  auto json = TinyJSON::TJ::parse(R"(
    {
      "a": 1,
      "b": "text",
      "c": [10, 20, 30],
      "d": { "e": 5 }
    }
    )", options
  );

  ASSERT_NE(nullptr, json);

  ASSERT_EQ(1, (*json)["a"].as<int>());
  ASSERT_EQ(std::string("text"), (*json)["b"].as<std::string>());
  ASSERT_EQ(20, (*json)["c"][1].as<int>());
  ASSERT_EQ(5, (*json)["d"]["e"].as<int>());

  ASSERT_EQ(1, (*json)[0].as<int>());

  delete json;
}

TEST(TestOperatorAccess, AccessArrayRoot)
{
  TinyJSON::parse_options options = {};
  options.throw_exception = true;
  options.strict = false;
  auto json = TinyJSON::TJ::parse(R"(
    [1, 2, 3]
    )", options
  );

  ASSERT_NE(nullptr, json);
  ASSERT_EQ(2, (*json)[1].as<int>());

  delete json;
}

TEST(TestOperatorAccess, StrictMissingKeyThrows)
{
  TinyJSON::parse_options options = {};
  options.throw_exception = true;
  options.strict = true;
  auto json = TinyJSON::TJ::parse(R"(
    { "a": 1 }
    )", options
  );

  ASSERT_NE(nullptr, json);
  ASSERT_ANY_THROW((*json)["missing"].as<int>());

  delete json;
}

TEST(TestOperatorAccess, StdStringKeyOwnership)
{
  TinyJSON::parse_options options = {};
  options.throw_exception = true;
  options.strict = false;
  auto json = TinyJSON::TJ::parse(R"(
    {
      "a": 1,
      "b": 2
    }
    )", options
  );

  ASSERT_NE(nullptr, json);

  std::string key = "a";
  key.reserve(8);
  auto accessor = (*json)[key];
  key[0] = 'b';

  ASSERT_EQ(1, accessor.as<int>());

  auto temp_accessor = (*json)[std::string("a")];
  ASSERT_EQ(1, temp_accessor.as<int>());

  delete json;
}

TEST(TestOperatorAccess, ChainedAccessDefaultsOnMissingIntermediate)
{
  TinyJSON::parse_options options = {};
  options.throw_exception = true;
  options.strict = false;
  auto json = TinyJSON::TJ::parse(R"(
    {
      "a": { "b": 1 }
    }
    )", options
  );

  ASSERT_NE(nullptr, json);

  ASSERT_EQ(1, (*json)["a"]["b"].as<int>());
  ASSERT_EQ(0, (*json)["missing"]["b"].as<int>());

  delete json;
}

TEST(TestOperatorAccess, ObjectIndexOrderingAndBounds)
{
  TinyJSON::parse_options options = {};
  options.throw_exception = true;
  options.strict = false;
  auto json = TinyJSON::TJ::parse(R"(
    {
      "first": 1,
      "second": 2,
      "third": 3
    }
    )", options
  );

  ASSERT_NE(nullptr, json);

  ASSERT_EQ(1, (*json)[0].as<int>());
  ASSERT_EQ(2, (*json)[1].as<int>());
  ASSERT_EQ(3, (*json)[2].as<int>());
  ASSERT_EQ(0, (*json)[3].as<int>());

  delete json;
}
