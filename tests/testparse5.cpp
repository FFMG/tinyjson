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

TEST(TJParse5, TrailingCommaInObject)
{
  parse_options options;
  options.throw_exception = true;

  const TJCHAR* json = TJCHARPREFIX("{\"a\": \"hello\", \"b\": \"world\",}");
  auto* tj = TJ::parse5(json, options);
  ASSERT_NE(nullptr, tj);
  auto* jobj = dynamic_cast<TJValueObject*>(tj);
  ASSERT_NE(nullptr, jobj);
  ASSERT_EQ(2u, jobj->get_number_of_items());
  delete tj;
}

TEST(TJParse5, TrailingCommaInArray)
{
  parse_options options;
  options.throw_exception = true;

  const TJCHAR* json = TJCHARPREFIX("[\"hello\", \"world\",]");
  auto* tj = TJ::parse5(json, options);
  ASSERT_NE(nullptr, tj);
  auto* jarr = dynamic_cast<TJValueArray*>(tj);
  ASSERT_NE(nullptr, jarr);
  ASSERT_EQ(2u, jarr->get_number_of_items());
  delete tj;
}

TEST(TJParse5, MultipleTrailingCommasInObjectThrows)
{
  parse_options options;
  options.throw_exception = true;

  const TJCHAR* json = TJCHARPREFIX("{\"a\": \"hello\",,}");
  EXPECT_THROW(TJ::parse5(json, options), TJParseException);
}

TEST(TJParse5, MultipleTrailingCommasInArrayThrows)
{
  parse_options options;
  options.throw_exception = true;

  const TJCHAR* json = TJCHARPREFIX("[\"hello\",,]");
  EXPECT_THROW(TJ::parse5(json, options), TJParseException);
}

TEST(TJParse5, EmptyObjectWithOnlyCommaThrows)
{
  parse_options options;
  options.throw_exception = true;

  const TJCHAR* json = TJCHARPREFIX("{,}");
  EXPECT_THROW(TJ::parse5(json, options), TJParseException);
}

TEST(TJParse5, EmptyArrayWithOnlyCommaThrows)
{
  parse_options options;
  options.throw_exception = true;

  const TJCHAR* json = TJCHARPREFIX("[,]");
  EXPECT_THROW(TJ::parse5(json, options), TJParseException);
}

TEST(TJParse5, TrailingCommaAfterRootValueThrows)
{
  parse_options options;
  options.throw_exception = true;

  const TJCHAR* json = TJCHARPREFIX("true,");
  EXPECT_THROW(TJ::parse5(json, options), TJParseException);
}

TEST(TJParse, TrailingCommaInObjectNotAllowedInStandard)
{
  parse_options options;
  options.throw_exception = true;

  const TJCHAR* json = TJCHARPREFIX("{\"a\": \"hello\", \"b\": \"world\",}");
  EXPECT_THROW(TJ::parse(json, options), TJParseException);
}

TEST(TJParse, TrailingCommaInArrayNotAllowedInStandard)
{
  parse_options options;
  options.throw_exception = true;

  const TJCHAR* json = TJCHARPREFIX("[\"hello\", \"world\",]");
  EXPECT_THROW(TJ::parse(json, options), TJParseException);
}

TEST(TJParse, TrailingCommaAfterRootValueNotAllowedInStandard)
{
  parse_options options;
  options.throw_exception = true;

  const TJCHAR* json = TJCHARPREFIX("true,");
  EXPECT_THROW(TJ::parse(json, options), TJParseException);
}
