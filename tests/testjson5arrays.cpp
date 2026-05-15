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

TEST(TJJSON5Arrays, RemoveAtWithComments)
{
  const TJCHAR* json = TJCHARPREFIX("[\n  1, \n  2,\n  // some comment\n  3,\n  4\n]");
  parse_options options;
  options.specification = parse_options::json5_1_0_0;
  auto* tj = TJ::parse(json, options);
  ASSERT_NE(nullptr, tj);
  auto* jarr = dynamic_cast<TJValueArray*>(tj);
  ASSERT_NE(nullptr, jarr);
  ASSERT_EQ(4u, jarr->get_number_of_items());
  ASSERT_EQ(5u, jarr->get_number_of_elements());

  // Delete item before the comment (value 2, index 1)
  jarr->remove_at(1);
  ASSERT_EQ(3u, jarr->get_number_of_items());
  ASSERT_EQ(4u, jarr->get_number_of_elements());

  // Verify value 2 is gone and comment is still there at element index 1
  ASSERT_EQ(1.0, jarr->at(0)->get_float<double>());
  ASSERT_TRUE(jarr->element_at(1)->is_comment());
  ASSERT_EQ(3.0, jarr->at(1)->get_float<double>());

  // Delete item after the comment (value 3, index 1 in the new array)
  jarr->remove_at(1);
  ASSERT_EQ(2u, jarr->get_number_of_items());
  ASSERT_EQ(3u, jarr->get_number_of_elements());

  ASSERT_EQ(1.0, jarr->at(0)->get_float<double>());
  ASSERT_TRUE(jarr->element_at(1)->is_comment());
  ASSERT_EQ(4.0, jarr->at(1)->get_float<double>());

  delete tj;
}

TEST(TJJSON5Arrays, RemoveAllItemsKeepComment)
{
  const TJCHAR* json = TJCHARPREFIX("[\n  1, \n  2,\n  // some comment\n  3,\n  4\n]");
  parse_options options;
  options.specification = parse_options::json5_1_0_0;
  auto* tj = TJ::parse(json, options);
  ASSERT_NE(nullptr, tj);
  auto* jarr = dynamic_cast<TJValueArray*>(tj);
  ASSERT_NE(nullptr, jarr);

  // Remove all items (they are 4)
  jarr->remove_at(0);
  jarr->remove_at(0);
  jarr->remove_at(0);
  jarr->remove_at(0);

  ASSERT_EQ(0u, jarr->get_number_of_items());
  ASSERT_EQ(1u, jarr->get_number_of_elements());
  ASSERT_TRUE(jarr->element_at(0)->is_comment());

  delete tj;
}
