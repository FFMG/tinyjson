#pragma once
// Licensed to Florent Guelfucci under one or more agreements.
// Florent Guelfucci licenses this file to you under the MIT license.
// See the LICENSE file in the project root for more information.
#include <cmath>
#include <gtest/gtest.h>
#include <regex>
#include <vector>
#include <string>

inline bool areDoublesEqual(double a, double b, double epsilon = 1e-9) {
  return std::fabs(a - b) < epsilon;
}

inline bool areFloatsEqual(float a, float b, float epsilon = 1e-9) {
  return std::fabs(a - b) < epsilon;
}

inline void ExpectJSONNear(const char* json_string, const std::string& pattern, const std::vector<double>& expected_values, double tolerance = 1e-5) {
  std::string str(json_string);
  std::regex regex_pattern(pattern);
  std::smatch match;
  ASSERT_TRUE(std::regex_search(str, match, regex_pattern)) << "JSON output did not match regex pattern: " << json_string;
  ASSERT_EQ(expected_values.size() + 1, (size_t)match.size()) << "Number of expected values does not match regex groups";
  for (size_t i = 0; i < expected_values.size(); ++i) {
    EXPECT_NEAR(expected_values[i], std::stod(match[i + 1].str()), tolerance) << "Mismatch at index " << i;
  }
}