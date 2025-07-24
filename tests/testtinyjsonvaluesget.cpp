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
      "d": 42.2
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

  ASSERT_TRUE(valuea->get_boolean());
  ASSERT_TRUE(valueb->get_boolean());
  ASSERT_FALSE(valuec->get_boolean());
  ASSERT_TRUE(valued->get_boolean());

  delete json;
}

TEST(TestValueGet, GetStrictBoolean)
{
  auto json = TinyJSON::TJ::parse(R"(
    {
      "a": true,
      "b": 123,
      "c": 0,
      "d": 42.2
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

  ASSERT_TRUE(valuea->get_boolean(true));
  ASSERT_ANY_THROW(valueb->get_boolean(true));
  ASSERT_ANY_THROW(valuec->get_boolean(true));
  ASSERT_ANY_THROW(valued->get_boolean(true));

  delete json;
}
