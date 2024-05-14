#include <gtest/gtest.h>

namespace {
  int life() {
    return 42;
  }
}


TEST(TestBasic, TestSimple) {
  EXPECT_EQ(life(), 42);
}