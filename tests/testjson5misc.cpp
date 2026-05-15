// Licensed to Florent Guelfucci under one or more agreements.
// Florent Guelfucci licenses this file to you under the MIT license.
// See the LICENSE file in the project root for more information.
#include <gtest/gtest.h>
#include "../src/TinyJSON.h"
#include "testshelper.h"

using namespace TinyJSON;

// These tests are inspired by the official JSON5 test suite:
// https://github.com/json5/json5-tests/tree/master/misc

TEST(TJJSON5Misc, ValidWhitespace)
{
  // \t\n\v\f\r space
  const TJCHAR* json = TJCHARPREFIX(" \t\n\v\f\r {\"a\": 1} ");
  parse_options options;
  options.specification = parse_options::json5_1_0_0;
  options.throw_exception = true;
  try {
    auto* tj = TJ::parse(json, options);
    ASSERT_NE(nullptr, tj);
    delete tj;
  } catch (const TJParseException& e) {
    FAIL() << "Parsing failed with message: " << e.what();
  }
}

TEST(TJJSON5Misc, EmptyFileRejected)
{
  const TJCHAR* json = TJCHARPREFIX("");
  parse_options options;
  options.specification = parse_options::json5_1_0_0;
  auto* tj = TJ::parse(json, options);
  ASSERT_EQ(nullptr, tj);
}
