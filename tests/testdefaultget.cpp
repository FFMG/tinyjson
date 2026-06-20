// Licensed to Florent Guelfucci under one or more agreements.
// Florent Guelfucci licenses this file to you under the MIT license.
// See the LICENSE file in the project root for more information.
#include <gtest/gtest.h>
#define TJ_INCLUDE_STD_STRING 1
#include "../src/TinyJSON.h"

using namespace TinyJSON;

TEST(TJDefaultGet, BasicValues) {
    parse_options options = {};
    options.throw_exception = true;
    options.strict = true;
    auto json = TJ::parse(R"({
        "int": 42,
        "float": 3.14,
        "bool": true,
        "string": "hello"
    })", options);

    ASSERT_NE(json, nullptr);
    auto obj = dynamic_cast<TJValueObject*>(json);
    ASSERT_NE(obj, nullptr);

    // Key exists
    EXPECT_EQ(obj->get<int>("int"), 42);
    EXPECT_NEAR(obj->get<double>("float"), 3.14, 1e-5);
    EXPECT_EQ(obj->get<bool>("bool"), true);
    EXPECT_STREQ(obj->get<const char*>("string"), "hello");
    EXPECT_EQ(obj->get<std::string>("string"), "hello");

    // Key does not exist
    EXPECT_THROW(obj->get<int>("not_found"), TinyJSON::TJParseException);
    EXPECT_THROW(obj->get<double>("not_found"), TinyJSON::TJParseException);
    EXPECT_THROW(obj->get<bool>("not_found"), TinyJSON::TJParseException);
    EXPECT_THROW(obj->get<const char*>("not_found"), TinyJSON::TJParseException);
    EXPECT_THROW(obj->get<std::string>("not_found"), TinyJSON::TJParseException);

    // Case sensitivity
    EXPECT_THROW(obj->get<int>("INT"), TinyJSON::TJParseException); // Case sensitive, not found
    EXPECT_THROW(obj->get<int>("INT"), TinyJSON::TJParseException); // Case insensitive, found

    delete json;
}

TEST(TJDefaultGet, StdStringKey) {
    parse_options options = {};
    options.throw_exception = true;
    options.strict = true;
    auto json = TJ::parse(R"({
        "int": 42
    })", options);

    ASSERT_NE(json, nullptr);
    auto obj = dynamic_cast<TJValueObject*>(json);
    ASSERT_NE(obj, nullptr);

    std::string key = "int";
    std::string missing = "missing";

    EXPECT_EQ(obj->get<int>(key), 42);
    EXPECT_THROW(obj->get<int>(missing), TinyJSON::TJParseException);

    delete json;
}

TEST(TJDefaultGet, TypeMismatch) {
    parse_options options = {};
    options.throw_exception = true;
    options.strict = true;
    auto json = TJ::parse(R"({
        "int": 42,
        "string": "hello"
    })", options);

    ASSERT_NE(json, nullptr);
    auto obj = dynamic_cast<TJValueObject*>(json);
    ASSERT_NE(obj, nullptr);

    // With try-catch in get(), it returns default_value on mismatch
    EXPECT_THROW(obj->get<int>("string"), TinyJSON::TJParseException);
    
    options.throw_exception = false;
    options.strict = false;
    obj->set_parse_options(options);
    EXPECT_STREQ(obj->get<const char*>("int"), "42");

    delete json;
}

struct CallbackTracker
{
  int* warning_count;
  int* trace_count;
  std::string* last_message;

  void operator()(parse_options::message_type type, const TJCHAR* message) const
  {
    if (type == parse_options::message_type::warning)
    {
      (*warning_count)++;
    }
    else if (type == parse_options::message_type::trace)
    {
      (*trace_count)++;
      if (message != nullptr)
      {
        *last_message = message;
      }
    }
  }
};

TEST(TJDefaultGetOr, BooleanDefault)
{
  parse_options options = {};
  options.throw_exception = true;
  options.strict = true;

  auto json = TJ::parse(R"({})", options);
  ASSERT_NE(json, nullptr);
  auto obj = dynamic_cast<TJValueObject*>(json);
  ASSERT_NE(obj, nullptr);

  // Default is true, should return true
  EXPECT_TRUE(obj->get_or<bool>("missing", true));

  // Default is false, should return false
  EXPECT_FALSE(obj->get_or<bool>("missing2", false));

  // Try with non-strict, no-throw options
  options.throw_exception = false;
  options.strict = false;
  obj->set_parse_options(options);

  EXPECT_TRUE(obj->get_or<bool>("missing", true));
  EXPECT_FALSE(obj->get_or<bool>("missing2", false));

  delete json;
}

TEST(TJDefaultGetOr, IntDefault)
{
  parse_options options = {};
  options.throw_exception = true;
  options.strict = true;

  auto json = TJ::parse(R"({})", options);
  ASSERT_NE(json, nullptr);
  auto obj = dynamic_cast<TJValueObject*>(json);
  ASSERT_NE(obj, nullptr);

  // Default is 42, should return 42 (not 0)
  EXPECT_EQ(obj->get_or<int>("missing", 42), 42);

  // Try with non-strict, no-throw options
  options.throw_exception = false;
  options.strict = false;
  obj->set_parse_options(options);

  EXPECT_EQ(obj->get_or<int>("missing", 42), 42);

  delete json;
}

TEST(TJDefaultGetOr, LongDefault)
{
  parse_options options = {};
  options.throw_exception = true;
  options.strict = true;

  auto json = TJ::parse(R"({})", options);
  ASSERT_NE(json, nullptr);
  auto obj = dynamic_cast<TJValueObject*>(json);
  ASSERT_NE(obj, nullptr);

  // Default is 999L, should return 999L
  EXPECT_EQ(obj->get_or<long>("missing", 999L), 999L);
  EXPECT_EQ(obj->get_or<long long>("missing2", 9999LL), 9999LL);

  // Try with non-strict, no-throw options
  options.throw_exception = false;
  options.strict = false;
  obj->set_parse_options(options);

  EXPECT_EQ(obj->get_or<long>("missing", 999L), 999L);
  EXPECT_EQ(obj->get_or<long long>("missing2", 9999LL), 9999LL);

  delete json;
}

TEST(TJDefaultGetOr, FloatDefault)
{
  parse_options options = {};
  options.throw_exception = true;
  options.strict = true;

  auto json = TJ::parse(R"({})", options);
  ASSERT_NE(json, nullptr);
  auto obj = dynamic_cast<TJValueObject*>(json);
  ASSERT_NE(obj, nullptr);

  // Default is 3.14f / 2.718, should return default
  EXPECT_NEAR(obj->get_or<float>("missing", 3.14f), 3.14f, 1e-5f);
  EXPECT_NEAR(obj->get_or<double>("missing2", 2.718), 2.718, 1e-5);

  // Try with non-strict, no-throw options
  options.throw_exception = false;
  options.strict = false;
  obj->set_parse_options(options);

  EXPECT_NEAR(obj->get_or<float>("missing", 3.14f), 3.14f, 1e-5f);
  EXPECT_NEAR(obj->get_or<double>("missing2", 2.718), 2.718, 1e-5);

  delete json;
}

TEST(TJDefaultGetOr, WarningAndTraceLogging)
{
  int warning_count = 0;
  int trace_count = 0;
  std::string last_message = "";
  parse_options options = {};
  CallbackTracker tracker = { &warning_count, &trace_count, &last_message };
  options.callback_function = tracker;

  auto* json = TJ::parse("{}", options);
  auto* obj = dynamic_cast<TJValueObject*>(json);
  ASSERT_NE(nullptr, obj);

  // Calling get_or
  bool val = obj->get_or<bool>("missing", true);
  EXPECT_TRUE(val);

  // Should have triggered 1 trace and 0 warnings
  EXPECT_EQ(0, warning_count);
  EXPECT_EQ(1, trace_count);
  EXPECT_EQ("The key 'missing' was not found!", last_message);

  delete json;
}
