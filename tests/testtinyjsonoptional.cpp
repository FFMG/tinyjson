// Licensed to Florent Guelfucci under one or more agreements.
// Florent Guelfucci licenses this file to you under the MIT license.
// See the LICENSE file in the project root for more information.
#include <gtest/gtest.h>
#include <stdexcept>
#include <string>
#include "../src/TinyJSON.h"

using namespace TinyJSON;

TEST(TestOptional, DefaultConstructor) {
  Optional<int> opt;
  ASSERT_FALSE(opt.has_value());
  ASSERT_FALSE((bool)opt);
}

TEST(TestOptional, ValueConstructor) {
  Optional<int> opt(42);
  ASSERT_TRUE(opt.has_value());
  ASSERT_EQ(42, opt.value());
  ASSERT_EQ(42, *opt);
}

TEST(TestOptional, CopyConstructor) {
  Optional<int> opt1(42);
  Optional<int> opt2(opt1);
  ASSERT_TRUE(opt2.has_value());
  ASSERT_EQ(42, opt2.value());
}

TEST(TestOptional, MoveConstructor) {
  Optional<std::string> opt1(std::string("hello"));
  Optional<std::string> opt2(std::move(opt1));
  ASSERT_TRUE(opt2.has_value());
  ASSERT_EQ("hello", opt2.value());
}

TEST(TestOptional, CopyAssignment) {
  Optional<int> opt1(42);
  Optional<int> opt2;
  opt2 = opt1;
  ASSERT_TRUE(opt2.has_value());
  ASSERT_EQ(42, opt2.value());

  Optional<int> opt3;
  opt2 = opt3;
  ASSERT_FALSE(opt2.has_value());
}

TEST(TestOptional, MoveAssignment) {
  Optional<std::string> opt1(std::string("hello"));
  Optional<std::string> opt2;
  opt2 = std::move(opt1);
  ASSERT_TRUE(opt2.has_value());
  ASSERT_EQ("hello", opt2.value());
}

TEST(TestOptional, ValueOr) {
  Optional<int> opt1(42);
  ASSERT_EQ(42, opt1.value_or(0));

  Optional<int> opt2;
  ASSERT_EQ(0, opt2.value_or(0));
}

TEST(TestOptional, Reset) {
  Optional<int> opt(42);
  ASSERT_TRUE(opt.has_value());
  opt.reset();
  ASSERT_FALSE(opt.has_value());
}

TEST(TestOptional, ArrowOperator) {
  struct Foo { int x; };
  Optional<Foo> opt(Foo{ 42 });
  ASSERT_EQ(42, opt->x);
}

TEST(TestOptional, ExceptionOnNoValue) {
  Optional<int> opt;
  ASSERT_THROW(opt.value(), std::logic_error);
}

// Special class to track construction/destruction
struct Tracker {
  static int constructions;
  static int destructions;
  Tracker() { constructions++; }
  Tracker(const Tracker&) { constructions++; }
  Tracker(Tracker&&) noexcept { constructions++; }
  ~Tracker() { destructions++; }
  Tracker& operator=(const Tracker&) = default;
  Tracker& operator=(Tracker&&) = default;
};
int Tracker::constructions = 0;
int Tracker::destructions = 0;

TEST(TestOptional, Lifecycle) {
  Tracker::constructions = 0;
  Tracker::destructions = 0;
  {
    Optional<Tracker> opt;
    ASSERT_EQ(0, Tracker::constructions);
    opt = Tracker(); // temporary + construction in storage
  }
  // opt destroyed calls reset() -> destructor
  ASSERT_EQ(Tracker::constructions, Tracker::destructions);
}
