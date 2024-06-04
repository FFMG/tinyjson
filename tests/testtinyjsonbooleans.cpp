// Licensed to Florent Guelfucci under one or more agreements.
// Florent Guelfucci licenses this file to you under the MIT license.
// See the LICENSE file in the project root for more information.
#include <gtest/gtest.h>
#include "../src/TinyJSON.h"

TEST(TestBooleans, BooleanIsAfterMissingColon) {
  auto json = TinyJSON::TinyJSON::parse(R"(
    {
      "a" true
    }
    )"
  );
  ASSERT_EQ(nullptr, json);
}

TEST(TestBooleans, CheckForTrue) {
  auto json = TinyJSON::TinyJSON::parse(R"(
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

  auto boolean = dynamic_cast<const TinyJSON::TJValueBoolean*>(jobject->try_get_value("a"));
  ASSERT_NE(nullptr, boolean);
  ASSERT_TRUE(boolean->is_true());

  delete json;
}

TEST(TestBooleans, CheckForFalse) {
  auto json = TinyJSON::TinyJSON::parse(R"(
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

  auto boolean = dynamic_cast<const TinyJSON::TJValueBoolean*>(jobject->try_get_value("a"));
  ASSERT_NE(nullptr, boolean);
  ASSERT_FALSE(boolean->is_true());

  delete json;
}

TEST(TestBooleans, TrueBooleanNotSpelledProperly1) {
  auto json = TinyJSON::TinyJSON::parse(R"(
    {
      "a" : t
    }
    )"
  );
  ASSERT_EQ(nullptr, json);
}

TEST(TestBooleans, TrueBooleanNotSpelledProperly2) {
  auto json = TinyJSON::TinyJSON::parse(R"(
    {
      "a" : tr
    }
    )"
  );
  ASSERT_EQ(nullptr, json);
}

TEST(TestBooleans, TrueBooleanNotSpelledProperly3) {
  auto json = TinyJSON::TinyJSON::parse(R"(
    {
      "a" : tru
    }
    )"
  );
  ASSERT_EQ(nullptr, json);
}

TEST(TestBooleans, FalseBooleanNotSpelledProperly1) {
  auto json = TinyJSON::TinyJSON::parse(R"(
    {
      "a" : f
    }
    )"
  );
  ASSERT_EQ(nullptr, json);
}

TEST(TestBooleans, FalseBooleanNotSpelledProperly2) {
  auto json = TinyJSON::TinyJSON::parse(R"(
    {
      "a" : fa
    }
    )"
  );
  ASSERT_EQ(nullptr, json);
}

TEST(TestBooleans, FalseBooleanNotSpelledProperly3) {
  auto json = TinyJSON::TinyJSON::parse(R"(
    {
      "a" : fal
    }
    )"
  );
  ASSERT_EQ(nullptr, json);
}

TEST(TestBooleans, FalseBooleanNotSpelledProperly4) {
  auto json = TinyJSON::TinyJSON::parse(R"(
    {
      "a" : fals
    }
    )"
  );
  ASSERT_EQ(nullptr, json);
}

TEST(TestBooleans, CheckThatValueIsBooleanAndCorrectValue) {
  auto json = TinyJSON::TinyJSON::parse(R"(
    {
      "a" : false,
      "b" : true
    }
    )"
  );

  ASSERT_NE(nullptr, json);
  auto jobject = dynamic_cast<TinyJSON::TJValueObject*>(json);
  ASSERT_NE(nullptr, jobject);

  auto booleana = jobject->try_get_value("a");
  ASSERT_NE(nullptr, booleana);

  ASSERT_FALSE(booleana->is_object());
  ASSERT_FALSE(booleana->is_array());
  ASSERT_FALSE(booleana->is_string());
  ASSERT_FALSE(booleana->is_number());
  ASSERT_FALSE(booleana->is_true());
  ASSERT_TRUE(booleana->is_false());
  ASSERT_FALSE(booleana->is_null());

  auto booleanb = jobject->try_get_value("b");
  ASSERT_NE(nullptr, booleanb);

  ASSERT_FALSE(booleanb->is_object());
  ASSERT_FALSE(booleanb->is_array());
  ASSERT_FALSE(booleanb->is_string());
  ASSERT_FALSE(booleanb->is_number());
  ASSERT_TRUE(booleanb->is_true());
  ASSERT_FALSE(booleanb->is_false());
  ASSERT_FALSE(booleanb->is_null());

  delete json;
}

TEST(TestBooleans, CheckThatValueIsBooleanAndCorrectValueInArray) {
  auto json = TinyJSON::TinyJSON::parse(R"(
    [
      false,
      true
    ]
    )"
  );

  ASSERT_NE(nullptr, json);
  auto jarray = dynamic_cast<TinyJSON::TJValueArray*>(json);
  ASSERT_NE(nullptr, jarray);

  auto booleana = jarray->at(0);
  ASSERT_NE(nullptr, booleana);

  ASSERT_FALSE(booleana->is_object());
  ASSERT_FALSE(booleana->is_array());
  ASSERT_FALSE(booleana->is_string());
  ASSERT_FALSE(booleana->is_number());
  ASSERT_FALSE(booleana->is_true());
  ASSERT_TRUE(booleana->is_false());
  ASSERT_FALSE(booleana->is_null());

  auto booleanb = jarray->at(1);
  ASSERT_NE(nullptr, booleanb);

  ASSERT_FALSE(booleanb->is_object());
  ASSERT_FALSE(booleanb->is_array());
  ASSERT_FALSE(booleanb->is_string());
  ASSERT_FALSE(booleanb->is_number());
  ASSERT_TRUE(booleanb->is_true());
  ASSERT_FALSE(booleanb->is_false());
  ASSERT_FALSE(booleanb->is_null());

  delete json;
}
