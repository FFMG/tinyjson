// Licensed to Florent Guelfucci under one or more agreements.
// Florent Guelfucci licenses this file to you under the MIT license.
// See the LICENSE file in the project root for more information.
#include <gtest/gtest.h>
#include "../src/TinyJSON.h"

TEST(TestSet, GenericSetInt) {
  TinyJSON::TJValueObject obj;
  obj.set(TJCHARPREFIX("key"), 42);
  ASSERT_EQ(42, obj.get<int>(TJCHARPREFIX("key")));
}

TEST(TestSet, GenericSetLongLong) {
  TinyJSON::TJValueObject obj;
  long long val = 1234567890123LL;
  obj.set(TJCHARPREFIX("key"), val);
  ASSERT_EQ(val, obj.get<long long>(TJCHARPREFIX("key")));
}

TEST(TestSet, GenericSetFloat) {
  TinyJSON::TJValueObject obj;
  obj.set(TJCHARPREFIX("key"), 3.14f);
  ASSERT_NEAR(3.14, obj.get<float>(TJCHARPREFIX("key")), 0.001);
}

TEST(TestSet, GenericSetDouble) {
  TinyJSON::TJValueObject obj;
  obj.set(TJCHARPREFIX("key"), 2.71828);
  ASSERT_NEAR(2.71828, obj.get<double>(TJCHARPREFIX("key")), 0.00001);
}

TEST(TestSet, GenericSetBool) {
  TinyJSON::TJValueObject obj;
  obj.set(TJCHARPREFIX("key"), true);
  ASSERT_TRUE(obj.get<bool>(TJCHARPREFIX("key")));
  obj.set(TJCHARPREFIX("key"), false);
  ASSERT_FALSE(obj.get<bool>(TJCHARPREFIX("key")));
}

TEST(TestSet, GenericSetString) {
  TinyJSON::TJValueObject obj;
  obj.set(TJCHARPREFIX("key"), TJCHARPREFIX("value"));
  ASSERT_STREQ(TJCHARPREFIX("value"), obj.get<const TJCHAR*>(TJCHARPREFIX("key")));
}

#if TJ_INCLUDE_STD_STRING == 1
TEST(TestSet, GenericSetStdString) {
  TinyJSON::TJValueObject obj;
  std::string value = "std_value";
  obj.set(TJCHARPREFIX("key"), value);
  ASSERT_STREQ(TJCHARPREFIX("std_value"), obj.get<const TJCHAR*>(TJCHARPREFIX("key")));
}

TEST(TestSet, SetWithStdStringKey) {
  TinyJSON::TJValueObject obj;
  std::string key = "std_key";
  obj.set(key, 123);
  ASSERT_EQ(123, obj.get<int>(TJCHARPREFIX("std_key")));
}
#endif

TEST(TestSet, GenericSetVector) {
  TinyJSON::TJValueObject obj;
  std::vector<int> vec = {1, 2, 3};
  obj.set(TJCHARPREFIX("key"), vec);
  auto result = obj.get<std::vector<int>>(TJCHARPREFIX("key"));
  ASSERT_EQ(3, result.size());
  ASSERT_EQ(1, result[0]);
  ASSERT_EQ(2, result[1]);
  ASSERT_EQ(3, result[2]);
}

TEST(TestSet, GenericSetVectorFloat) {
  TinyJSON::TJValueObject obj;
  std::vector<float> vec = {1.1f, 2.2f};
  obj.set(TJCHARPREFIX("key"), vec);
  auto result = obj.get<std::vector<float>>(TJCHARPREFIX("key"));
  ASSERT_EQ(2, result.size());
  ASSERT_NEAR(1.1, result[0], 0.001);
  ASSERT_NEAR(2.2, result[1], 0.001);
}
