// Licensed to Florent Guelfucci under one or more agreements.
// Florent Guelfucci licenses this file to you under the MIT license.
// See the LICENSE file in the project root for more information.
#include <gtest/gtest.h>
#define TJ_INCLUDE_STD_STRING 1
#include "../src/TinyJSON.h"

using namespace TinyJSON;

TEST(TJGenericGet, SingleValues) {
    auto json = TJ::parse(R"({
        "int": 42,
        "float": 3.14,
        "bool": true,
        "string": "hello"
    })");

    ASSERT_NE(json, nullptr);
    auto obj = dynamic_cast<TJValueObject*>(json);
    ASSERT_NE(obj, nullptr);

    // Generic get by key
    EXPECT_EQ(obj->get<int>("int"), 42);
    EXPECT_NEAR(obj->get<double>("float"), 3.14, 1e-5);
    EXPECT_EQ(obj->get<bool>("bool"), true);
    EXPECT_STREQ(obj->get<const char*>("string"), "hello");
    EXPECT_EQ(obj->get<std::string>("string"), "hello");

    // Generic get from value
    auto valInt = obj->try_get_value("int");
    EXPECT_EQ(valInt->get<int>(), 42);
    EXPECT_EQ(valInt->get<long long>(), 42LL);

    auto valFloat = obj->try_get_value("float");
    EXPECT_NEAR(valFloat->get<float>(), 3.14f, 1e-5);
    EXPECT_NEAR(valFloat->get<double>(), 3.14, 1e-5);

    delete json;
}

TEST(TJGenericGet, Vectors) {
    auto json = TJ::parse(R"({
        "ints": [1, 2, 3],
        "floats": [1.1, 2.2, 3.3]
    })");

    ASSERT_NE(json, nullptr);
    auto obj = dynamic_cast<TJValueObject*>(json);
    ASSERT_NE(obj, nullptr);

    // Generic get vector by key
    auto ints = obj->get<std::vector<int>>("ints");
    ASSERT_EQ(ints.size(), 3);
    EXPECT_EQ(ints[0], 1);
    EXPECT_EQ(ints[1], 2);
    EXPECT_EQ(ints[2], 3);

    auto floats = obj->get<std::vector<double>>("floats");
    ASSERT_EQ(floats.size(), 3);
    EXPECT_NEAR(floats[0], 1.1, 1e-5);
    EXPECT_NEAR(floats[1], 2.2, 1e-5);
    EXPECT_NEAR(floats[2], 3.3, 1e-5);

    // Generic get vector from value (array)
    auto valInts = obj->try_get_value("ints");
    auto ints2 = valInts->get<std::vector<long>>();
    ASSERT_EQ(ints2.size(), 3);
    EXPECT_EQ(ints2[0], 1L);

    delete json;
}
