// Licensed to Florent Guelfucci under one or more agreements.
// Florent Guelfucci licenses this file to you under the MIT license.
// See the LICENSE file in the project root for more information.
#include <gtest/gtest.h>
#define TJ_USE_CHAR 1
#include "../src/TinyJSON.h"

TEST(TestVersion, CheckVersionMajor) {
  ASSERT_EQ(0, TJ_VERSION_MAJOR);
}

TEST(TestVersion, CheckVersionMinor) {
  ASSERT_EQ(0, TJ_VERSION_MINOR);
}

TEST(TestVersion, CheckVersionPatch) {
  ASSERT_EQ(1, TJ_VERSION_PATCH);
}

TEST(TestVersion, CheckVersionString) {
  ASSERT_STREQ("0.0.1", TJ_VERSION_STRING);
}
