// Licensed to Florent Guelfucci under one or more agreements.
// Florent Guelfucci licenses this file to you under the MIT license.
// See the LICENSE file in the project root for more information.
#include <gtest/gtest.h>
#include "../src/TinyJSON.h"
#include "testshelper.h"

using namespace TinyJSON;

// These tests are inspired by the official JSON5 test suite:
// https://github.com/json5/json5-tests/tree/master/strings

TEST(TJJSON5Strings, MultiLineDoubleQuoted)
{
  const TJCHAR* json = TJCHARPREFIX("\"line 1 \\\nline 2\"");
  parse_options options;
  options.specification = parse_options::json5_1_0_0;
  auto* tj = TJ::parse(json, options);
  ASSERT_NE(nullptr, tj);
  EXPECT_STREQ(TJCHARPREFIX("line 1 line 2"), tj->get_string());
}

TEST(TJJSON5Strings, MultiLineSingleQuoted)
{
  const TJCHAR* json = TJCHARPREFIX("'line 1 \\\r\nline 2'");
  parse_options options;
  options.specification = parse_options::json5_1_0_0;
  auto* tj = TJ::parse(json, options);
  ASSERT_NE(nullptr, tj);
  EXPECT_STREQ(TJCHARPREFIX("line 1 line 2"), tj->get_string());
}

TEST(TJJSON5Strings, IdentityEscapes)
{
  const TJCHAR* json = TJCHARPREFIX("'\\A\\B\\/\\C'");
  parse_options options;
  options.specification = parse_options::json5_1_0_0;
  auto* tj = TJ::parse(json, options);
  ASSERT_NE(nullptr, tj);
  EXPECT_STREQ(TJCHARPREFIX("AB/C"), tj->get_string());
}

TEST(TJJSON5Strings, SpecialEscapes)
{
  const TJCHAR* json = TJCHARPREFIX("'\\v\\0'");
  parse_options options;
  options.specification = parse_options::json5_1_0_0;
  auto* tj = TJ::parse(json, options);
  ASSERT_NE(nullptr, tj);
  TJCHAR expected[3] = { static_cast<TJCHAR>(0x0B), static_cast<TJCHAR>(0x00), '\0' };
  EXPECT_STREQ(expected, tj->get_string());
}

TEST(TJJSON5Strings, HexEscapes)
{
  const TJCHAR* json = TJCHARPREFIX("'\\x41\\x42'");
  parse_options options;
  options.specification = parse_options::json5_1_0_0;
  auto* tj = TJ::parse(json, options);
  ASSERT_NE(nullptr, tj);
  EXPECT_STREQ(TJCHARPREFIX("AB"), tj->get_string());
}

TEST(TJJSON5Strings, ExtendedUnicodeEscape)
{
  // Grinning face emoji U+1F600
  const TJCHAR* json = TJCHARPREFIX("'\\u{1F600}'");
  parse_options options;
  options.specification = parse_options::json5_1_0_0;
  auto* tj = TJ::parse(json, options);
  ASSERT_NE(nullptr, tj);
#if TJ_USE_CHAR == 1
  // UTF-8 for U+1F600 is 0xF0 0x9F 0x98 0x80
  const char* expected = "\xF0\x9F\x98\x80";
  EXPECT_STREQ(expected, tj->get_string());
#else
  // WCHAR or other
  EXPECT_EQ(0x1F600, static_cast<unsigned int>(tj->get_string()[0]));
#endif
}

TEST(TJJSON5Strings, StandardJsonRejectsMultiLine)
{
  const TJCHAR* json = TJCHARPREFIX("\"line 1 \\\nline 2\"");
  parse_options options;
  options.specification = parse_options::rfc8259;
  auto* tj = TJ::parse(json, options);
  ASSERT_EQ(nullptr, tj);
}

TEST(TJJSON5Strings, StandardJsonRejectsIdentityEscape)
{
  const TJCHAR* json = TJCHARPREFIX("\"\\A\"");
  parse_options options;
  options.specification = parse_options::rfc8259;
  auto* tj = TJ::parse(json, options);
  ASSERT_EQ(nullptr, tj);
}

TEST(TJJSON5Strings, Json5RejectsEscapedDigits)
{
  // \1 is not a valid escape in JSON5 (to avoid octal confusion)
  const TJCHAR* json = TJCHARPREFIX("'\\1'");
  parse_options options;
  options.specification = parse_options::json5_1_0_0;
  auto* tj = TJ::parse(json, options);
  ASSERT_EQ(nullptr, tj);
}

TEST(TJJSON5Strings, NullFollowedByDigitRejected)
{
  // \0 followed by 5 is rejected in JSON5 (to avoid octal confusion)
  const TJCHAR* json = TJCHARPREFIX("'\\05'");
  parse_options options;
  options.specification = parse_options::json5_1_0_0;
  auto* tj = TJ::parse(json, options);
  ASSERT_EQ(nullptr, tj);
}
