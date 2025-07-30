#pragma once
// Licensed to Florent Guelfucci under one or more agreements.
// Florent Guelfucci licenses this file to you under the MIT license.
// See the LICENSE file in the project root for more information.
#include <cmath>

inline bool areDoublesEqual(double a, double b, double epsilon = 1e-9) {
  return std::fabs(a - b) < epsilon;
}

inline bool areFloatsEqual(float a, float b, float epsilon = 1e-9) {
  return std::fabsf(a - b) < epsilon;
}