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
