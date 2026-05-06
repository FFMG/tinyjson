// Licensed to Florent Guelfucci under one or more agreements.
// Florent Guelfucci licenses this file to you under the MIT license.
// See the LICENSE file in the project root for more information.
#include <gtest/gtest.h>
#include "../src/TinyJSON.h"
#include "testshelper.h"

using namespace TinyJSON;

// These tests are inspired by the official JSON5 test suite:
// https://github.com/json5/json5-tests/tree/master/arrays

TEST(TJJSON5Arrays, TrailingComma)
{
  const TJCHAR* json = TJCHARPREFIX("[1, 2, 3,]");
  parse_options options;
  options.specification = parse_options::json5_1_0_0;
  auto* tj = TJ::parse(json, options);
  ASSERT_NE(nullptr, tj);
  auto* jarr = dynamic_cast<TJValueArray*>(tj);
  ASSERT_NE(nullptr, jarr);
  ASSERT_EQ(3u, jarr->get_number_of_items());
  delete tj;
}

TEST(TJJSON5Arrays, LeadingCommaRejected)
{
  const TJCHAR* json = TJCHARPREFIX("[,1]");
  parse_options options;
  options.specification = parse_options::json5_1_0_0;
  auto* tj = TJ::parse(json, options);
  ASSERT_EQ(nullptr, tj);
}

TEST(TJJSON5Arrays, NoCommaRejected)
{
  const TJCHAR* json = TJCHARPREFIX("[1 2]");
  parse_options options;
  options.specification = parse_options::json5_1_0_0;
  auto* tj = TJ::parse(json, options);
  ASSERT_EQ(nullptr, tj);
}

TEST(TJJSON5Arrays, MultipleTrailingCommasRejected)
{
  const TJCHAR* json = TJCHARPREFIX("[1, 2,,]");
  parse_options options;
  options.specification = parse_options::json5_1_0_0;
  auto* tj = TJ::parse(json, options);
  ASSERT_EQ(nullptr, tj);
}
