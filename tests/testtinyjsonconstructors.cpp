// Licensed to Florent Guelfucci under one or more agreements.
// Florent Guelfucci licenses this file to you under the MIT license.
// See the LICENSE file in the project root for more information.
#include <gtest/gtest.h>
#define TJ_USE_CHAR 1
#include "../src/TinyJSON.h"

using namespace TinyJSON;

TEST(TestConstructors, TJValueBoolean) {
  TJValueBoolean b1(true);
  TJValueBoolean b2(b1); // Copy constructor
  EXPECT_TRUE(b2.is_true());
  
  TJValueBoolean b3(false);
  b3 = b1; // Copy assignment
  EXPECT_TRUE(b3.is_true());
  
  TJValueBoolean b4(std::move(b1)); // Move constructor
  EXPECT_TRUE(b4.is_true());
  
  TJValueBoolean b5(false);
  b5 = std::move(b4); // Move assignment
  EXPECT_TRUE(b5.is_true());
}

TEST(TestConstructors, TJValueNull) {
  TJValueNull n1;
  TJValueNull n2(n1);
  EXPECT_TRUE(n2.is_null());
  
  TJValueNull n3;
  n3 = n1;
  EXPECT_TRUE(n3.is_null());
  
  TJValueNull n4(std::move(n1));
  EXPECT_TRUE(n4.is_null());
  
  TJValueNull n5;
  n5 = std::move(n4);
  EXPECT_TRUE(n5.is_null());
}

TEST(TestConstructors, TJValueString) {
  TJValueString s1("hello");
  TJValueString s2(s1);
  EXPECT_STREQ("hello", s2.raw_value());
  
  TJValueString s3("world");
  s3 = s1;
  EXPECT_STREQ("hello", s3.raw_value());
  
  TJValueString s4(std::move(s1));
  EXPECT_STREQ("hello", s4.raw_value());
  EXPECT_STREQ("", s1.raw_value()); // s1 should be empty after move
  
  TJValueString s5("world");
  s5 = std::move(s4);
  EXPECT_STREQ("hello", s5.raw_value());
  EXPECT_STREQ("", s4.raw_value());
}

TEST(TestConstructors, TJValueNumberInt) {
  TJValueNumberInt i1(42);
  TJValueNumberInt i2(i1);
  EXPECT_EQ(42, i2.get_number());
  
  TJValueNumberInt i3(100);
  i3 = i1;
  EXPECT_EQ(42, i3.get_number());
  
  TJValueNumberInt i4(std::move(i1));
  EXPECT_EQ(42, i4.get_number());
  
  TJValueNumberInt i5(0);
  i5 = std::move(i4);
  EXPECT_EQ(42, i5.get_number());
}

TEST(TestConstructors, TJValueNumberFloat) {
  TJValueNumberFloat f1(3.14);
  TJValueNumberFloat f2(f1);
  EXPECT_NEAR(3.14, f2.get_number(), 0.001);
  
  TJValueNumberFloat f3(1.0);
  f3 = f1;
  EXPECT_NEAR(3.14, f3.get_number(), 0.001);
  
  TJValueNumberFloat f4(std::move(f1));
  EXPECT_NEAR(3.14, f4.get_number(), 0.001);
  
  TJValueNumberFloat f5(0.0);
  f5 = std::move(f4);
  EXPECT_NEAR(3.14, f5.get_number(), 0.001);
}

TEST(TestConstructors, TJValueArray) {
  TJValueArray a1;
  a1.add_number(1);
  a1.add_number(2);
  
  TJValueArray a2(a1);
  EXPECT_EQ(2, a2.get_number_of_items());
  EXPECT_EQ(1, a2.at(0)->get_number());
  EXPECT_EQ(2, a2.at(1)->get_number());
  
  TJValueArray a3;
  a3 = a1;
  EXPECT_EQ(2, a3.get_number_of_items());
  
  TJValueArray a4(std::move(a1));
  EXPECT_EQ(2, a4.get_number_of_items());
  EXPECT_EQ(0, a1.get_number_of_items()); // a1 should be empty
  
  TJValueArray a5;
  a5 = std::move(a4);
  EXPECT_EQ(2, a5.get_number_of_items());
  EXPECT_EQ(0, a4.get_number_of_items());
}

TEST(TestConstructors, TJValueObject) {
  TJValueObject o1;
  o1.set_number("a", 1);
  o1.set_number("b", 2);
  
  TJValueObject o2(o1);
  EXPECT_EQ(2, o2.get_number_of_items());
  EXPECT_EQ(1, o2.get_number("a"));
  EXPECT_EQ(2, o2.get_number("b"));
  
  TJValueObject o3;
  o3 = o1;
  EXPECT_EQ(2, o3.get_number_of_items());
  
  TJValueObject o4(std::move(o1));
  EXPECT_EQ(2, o4.get_number_of_items());
  EXPECT_EQ(0, o1.get_number_of_items());
  
  TJValueObject o5;
  o5 = std::move(o4);
  EXPECT_EQ(2, o5.get_number_of_items());
  EXPECT_EQ(0, o4.get_number_of_items());
}

TEST(TestConstructors, TJMember) {
  TJValueBoolean* val = new TJValueBoolean(true);
  TJMember m1("key", val);
  delete val; // m1 cloned it
  
  TJMember m2(m1);
  EXPECT_STREQ("key", m2.name());
  EXPECT_TRUE(m2.value()->is_true());
  
  TJMember m3("other", nullptr);
  m3 = m1;
  EXPECT_STREQ("key", m3.name());
  
  TJMember m4(std::move(m1));
  EXPECT_STREQ("key", m4.name());
  EXPECT_EQ(nullptr, m1.name());
  EXPECT_EQ(nullptr, m1.value());
  
  TJMember m5("other", nullptr);
  m5 = std::move(m4);
  EXPECT_STREQ("key", m5.name());
  EXPECT_EQ(nullptr, m4.name());
}

TEST(TestConstructors, DeepCopyVerification) {
  TJValueObject o1;
  o1.set_number("a", 1);
  
  TJValueObject o2(o1);
  o1.set_number("a", 2);
  
  EXPECT_EQ(1, o2.get_number("a")); // o2 should still be 1
  EXPECT_EQ(2, o1.get_number("a"));
}

TEST(TestConstructors, SelfAssignment) {
  TJValueString s1("hello");
  s1 = s1;
  EXPECT_STREQ("hello", s1.raw_value());
  
  TJValueObject o1;
  o1.set_number("a", 1);
  o1 = o1;
  EXPECT_EQ(1, o1.get_number("a"));
}
