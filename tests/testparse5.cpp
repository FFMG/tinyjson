// Licensed to Florent Guelfucci under one or more agreements.
// Florent Guelfucci licenses this file to you under the MIT license.
// See the LICENSE file in the project root for more information.
#include <gtest/gtest.h>
#include "../src/TinyJSON.h"

using namespace TinyJSON;

TEST(TJParse5, Parse5DefaultsToJSON5)
{
  const TJCHAR* json = TJCHARPREFIX("// comment\n{\"a\": 1}");
  auto* tj = TJ::parse5(json);
  ASSERT_NE(nullptr, tj);
  ASSERT_TRUE(tj->is_object());
  delete tj;
}

TEST(TJParse5, Parse5RespectsOtherOptions)
{
  const TJCHAR* json = TJCHARPREFIX("invalid");
  parse_options options;
  options.throw_exception = true;
  
  // parse5 should still use JSON5 spec but also throw because of throw_exception=true
  EXPECT_THROW(TJ::parse5(json, options), TJParseException);
}
