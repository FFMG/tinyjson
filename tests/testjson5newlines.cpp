// Licensed to Florent Guelfucci under one or more agreements.
// Florent Guelfucci licenses this file to you under the MIT license.
// See the LICENSE file in the project root for more information.
#include <gtest/gtest.h>
#include "../src/TinyJSON.h"
#include "testshelper.h"

using namespace TinyJSON;

// These tests are inspired by the official JSON5 test suite:
// https://github.com/json5/json5-tests/tree/master/new-lines

TEST(TJJSON5NewLines, CommentCR)
{
  const TJCHAR* json = TJCHARPREFIX("// comment\r{\"a\": 1}");
  parse_options options;
  options.specification = parse_options::json5_1_0_0;
  auto* tj = TJ::parse(json, options);
  ASSERT_NE(nullptr, tj);
  delete tj;
}

TEST(TJJSON5NewLines, CommentLF)
{
  const TJCHAR* json = TJCHARPREFIX("// comment\n{\"a\": 1}");
  parse_options options;
  options.specification = parse_options::json5_1_0_0;
  auto* tj = TJ::parse(json, options);
  ASSERT_NE(nullptr, tj);
  delete tj;
}

TEST(TJJSON5NewLines, CommentCRLF)
{
  const TJCHAR* json = TJCHARPREFIX("// comment\r\n{\"a\": 1}");
  parse_options options;
  options.specification = parse_options::json5_1_0_0;
  auto* tj = TJ::parse(json, options);
  ASSERT_NE(nullptr, tj);
  delete tj;
}
