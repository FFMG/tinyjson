// Licensed to Florent Guelfucci under one or more agreements.
// Florent Guelfucci licenses this file to you under the MIT license.
// See the LICENSE file in the project root for more information.
#include <gtest/gtest.h>
#define TJ_USE_CHAR 1
#include "../src/TinyJSON.h"
#include "testshelper.h"

TEST(TestDuplicateKeys, DuplicateKeysShouldTriggerTrace) {
  bool trace_triggered = false;
  TinyJSON::parse_options options;
  options.callback_function = [&](TinyJSON::parse_options::message_type message_type, const TJCHAR* message) {
    if (message_type == TinyJSON::parse_options::message_type::trace) {
      trace_triggered = true;
    }
  };

  auto json = TinyJSON::TJ::parse(R"({"a": 1, "a": 2})", options);
  ASSERT_NE(nullptr, json);
  
  const auto jobject = dynamic_cast<TinyJSON::TJValueObject*>(json);
  ASSERT_NE(nullptr, jobject);
  ASSERT_EQ(1, jobject->get_number_of_items());
  ASSERT_EQ(2, jobject->get_number("a"));

  EXPECT_TRUE(trace_triggered);

  delete json;
}

TEST(TestDuplicateKeys, DuplicateKeysInSetShouldTriggerTrace) {
  bool trace_triggered = false;
  TinyJSON::parse_options options;
  options.callback_function = [&](TinyJSON::parse_options::message_type message_type, const TJCHAR* message) {
    if (message_type == TinyJSON::parse_options::message_type::trace) {
      trace_triggered = true;
    }
  };

  TinyJSON::TJValueObject obj(options);
  obj.set("a", 1);
  EXPECT_FALSE(trace_triggered);
  
  obj.set("a", 2);
  EXPECT_TRUE(trace_triggered);

  ASSERT_EQ(1, obj.get_number_of_items());
  ASSERT_EQ(2, obj.get_number("a"));
}
