// Licensed to Florent Guelfucci under one or more agreements.
// Florent Guelfucci licenses this file to you under the MIT license.
// See the LICENSE file in the project root for more information.
#include <gtest/gtest.h>
#define TJ_USE_CHAR 1
#include "../src/TinyJSON.h"

#include <cstdio>
#include <fstream>
#include <functional>
#include <string>

namespace
{
  /// <summary>
  /// Keep track of the messages, (and where they happened), raised by the parser.
  /// </summary>
  struct position_tracker
  {
    int fatal_count = 0;
    int error_count = 0;
    int warning_count = 0;
    int trace_count = 0;

    // the last fatal/error/warning message.
    std::string message;
    int line = -2;
    int column = -2;

    // the last trace message.
    std::string trace_message;
    int trace_line = -2;
    int trace_column = -2;

    void operator()(TinyJSON::parse_options::message_type message_type, const TJCHAR* new_message, int new_line, int new_column)
    {
      switch (message_type)
      {
      case TinyJSON::parse_options::fatal:
        ++fatal_count;
        break;

      case TinyJSON::parse_options::error:
        ++error_count;
        break;

      case TinyJSON::parse_options::warning:
        ++warning_count;
        break;

      case TinyJSON::parse_options::trace:
        ++trace_count;
        trace_message = nullptr == new_message ? "" : new_message;
        trace_line = new_line;
        trace_column = new_column;
        return;

      default:
        return;
      }
      message = nullptr == new_message ? "" : new_message;
      line = new_line;
      column = new_column;
    }
  };

  position_tracker parse_and_track_with_options(const TJCHAR* source, TinyJSON::parse_options options)
  {
    position_tracker tracker;
    options.callback_function = std::ref(tracker);
    auto* json = TinyJSON::TJ::parse(source, options);
    delete json;
    return tracker;
  }

  position_tracker parse_and_track(const TJCHAR* source)
  {
    return parse_and_track_with_options(source, {});
  }

  position_tracker parse_and_track_with_specification(const TJCHAR* source, enum TinyJSON::parse_options::specification parse_specification)
  {
    TinyJSON::parse_options options = {};
    options.specification = parse_specification;
    return parse_and_track_with_options(source, options);
  }

  position_tracker parse5_and_track(const TJCHAR* source)
  {
    return parse_and_track_with_specification(source, TinyJSON::parse_options::json5_1_0_0);
  }

  ::testing::AssertionResult is_fatal_at(const position_tracker& tracker, const std::string& expected_message, int expected_line, int expected_column)
  {
    if (tracker.fatal_count != 1)
    {
      return ::testing::AssertionFailure() << "Expected 1 fatal message but got " << tracker.fatal_count << " (last message: '" << tracker.message << "')";
    }
    if (tracker.message != expected_message)
    {
      return ::testing::AssertionFailure() << "Expected message '" << expected_message << "' but got '" << tracker.message << "'";
    }
    if (tracker.line != expected_line || tracker.column != expected_column)
    {
      return ::testing::AssertionFailure() << "Expected line " << expected_line << ", column " << expected_column
        << " but got line " << tracker.line << ", column " << tracker.column << " for '" << tracker.message << "'";
    }
    return ::testing::AssertionSuccess();
  }

  ::testing::AssertionResult is_fatal_at(const position_tracker& tracker, int expected_line, int expected_column)
  {
    return is_fatal_at(tracker, tracker.message, expected_line, expected_column);
  }

  const char* line_column_file_name = "test_line_column_parse_file.json";
}

///////////////////////////////////////////////////////////////////////////////
// Basics
TEST(TestLineColumn, ErrorAtTheVeryFirstCharacter)
{
  auto tracker = parse_and_track("x");
  EXPECT_TRUE(is_fatal_at(tracker, "Unexpected Token while trying to read value.", 1, 1));
}

TEST(TestLineColumn, ErrorFurtherAlongTheFirstLine)
{
  auto tracker = parse_and_track("[1,2,x]");
  EXPECT_TRUE(is_fatal_at(tracker, "Unexpected Token while trying to read value.", 1, 6));
}

TEST(TestLineColumn, ErrorOnTheSecondLine)
{
  auto tracker = parse_and_track("[\n  x]");
  EXPECT_TRUE(is_fatal_at(tracker, "Unexpected Token while trying to read value.", 2, 3));
}

TEST(TestLineColumn, ErrorOnTheThirdLine)
{
  auto tracker = parse_and_track("[\n1,\n  x\n]");
  EXPECT_TRUE(is_fatal_at(tracker, "Unexpected Token while trying to read value.", 3, 3));
}

TEST(TestLineColumn, ErrorOnTheTenthLine)
{
  auto tracker = parse_and_track("[\n\n\n\n\n\n\n\n\n x]");
  EXPECT_TRUE(is_fatal_at(tracker, "Unexpected Token while trying to read value.", 10, 2));
}

TEST(TestLineColumn, ErrorOnALongLine)
{
  std::string source = "[";
  for (int i = 0; i < 100; ++i)
  {
    source += "1,";
  }
  source += "x]";
  auto tracker = parse_and_track(source.c_str());
  EXPECT_TRUE(is_fatal_at(tracker, "Unexpected Token while trying to read value.", 1, 202));
}

TEST(TestLineColumn, ErrorAfterManyLines)
{
  std::string source = "[";
  for (int i = 0; i < 500; ++i)
  {
    source += "\n  1,";
  }
  source += "\n  x]";
  auto tracker = parse_and_track(source.c_str());
  EXPECT_TRUE(is_fatal_at(tracker, "Unexpected Token while trying to read value.", 502, 3));
}

///////////////////////////////////////////////////////////////////////////////
// New lines
TEST(TestLineColumn, LineFeedIsANewLine)
{
  auto tracker = parse_and_track("[1,\nx]");
  EXPECT_TRUE(is_fatal_at(tracker, 2, 1));
}

TEST(TestLineColumn, CarriageReturnLineFeedIsASingleNewLine)
{
  auto tracker = parse_and_track("[\r\n  x]");
  EXPECT_TRUE(is_fatal_at(tracker, 2, 3));
}

TEST(TestLineColumn, LoneCarriageReturnIsANewLine)
{
  auto tracker = parse_and_track("[\r  x]");
  EXPECT_TRUE(is_fatal_at(tracker, 2, 3));
}

TEST(TestLineColumn, TwoLoneCarriageReturnsAreTwoNewLines)
{
  auto tracker = parse_and_track("[\r\r x]");
  EXPECT_TRUE(is_fatal_at(tracker, 3, 2));
}

TEST(TestLineColumn, LineFeedCarriageReturnIsTwoNewLines)
{
  auto tracker = parse_and_track("[\n\r x]");
  EXPECT_TRUE(is_fatal_at(tracker, 3, 2));
}

TEST(TestLineColumn, MixedLineEndings)
{
  auto tracker = parse_and_track("[\n\r\n\r  x]");
  EXPECT_TRUE(is_fatal_at(tracker, 4, 3));
}

TEST(TestLineColumn, ManyCarriageReturnLineFeeds)
{
  auto tracker = parse_and_track("[\r\n1,\r\n2,\r\n3,\r\n    x\r\n]");
  EXPECT_TRUE(is_fatal_at(tracker, 5, 5));
}

TEST(TestLineColumn, BlankLinesBeforeTheError)
{
  auto tracker = parse_and_track("\n\n\nx");
  EXPECT_TRUE(is_fatal_at(tracker, "Unexpected Token while trying to read value.", 4, 1));
}

TEST(TestLineColumn, ColumnIsResetAfterANewLine)
{
  auto tracker = parse_and_track("[1,     \nx]");
  EXPECT_TRUE(is_fatal_at(tracker, 2, 1));
}

TEST(TestLineColumn, ColumnIsResetAfterACarriageReturnLineFeed)
{
  auto tracker = parse_and_track("[1,     \r\nx]");
  EXPECT_TRUE(is_fatal_at(tracker, 2, 1));
}

TEST(TestLineColumn, ErrorRightAfterANewLine)
{
  auto tracker = parse_and_track("[1,2,3\n,,4]");
  EXPECT_TRUE(is_fatal_at(tracker, "Found a comma out of order, (2 commas).", 2, 2));
}

TEST(TestLineColumn, SameDocumentWithLineFeedAndCarriageReturnLineFeed)
{
  auto tracker_line_feed = parse_and_track("{\n  \"a\": {\n    \"b\": [1, 2, tru]\n  }\n}");
  auto tracker_carriage_return = parse_and_track("{\r\n  \"a\": {\r\n    \"b\": [1, 2, tru]\r\n  }\r\n}");
  EXPECT_TRUE(is_fatal_at(tracker_line_feed, "Could not read the word 'true'.", 3, 17));
  EXPECT_TRUE(is_fatal_at(tracker_carriage_return, "Could not read the word 'true'.", 3, 17));
}

///////////////////////////////////////////////////////////////////////////////
// White spaces
TEST(TestLineColumn, TabsCountAsOneColumn)
{
  auto tracker = parse_and_track("[\t\tx]");
  EXPECT_TRUE(is_fatal_at(tracker, 1, 4));
}

TEST(TestLineColumn, TabsAfterANewLine)
{
  auto tracker = parse_and_track("[\n\t\t\tx]");
  EXPECT_TRUE(is_fatal_at(tracker, 2, 4));
}

TEST(TestLineColumn, LeadingWhiteSpaces)
{
  auto tracker = parse_and_track("   x");
  EXPECT_TRUE(is_fatal_at(tracker, 1, 4));
}

TEST(TestLineColumn, TrailingWhiteSpacesBeforeASecondValue)
{
  auto tracker = parse_and_track("[1]   x");
  EXPECT_TRUE(is_fatal_at(tracker, "Unexpected multiple JSON values in root.", 1, 7));
}

TEST(TestLineColumn, Utf8BomIsNotCounted)
{
  auto tracker = parse_and_track("\xEF\xBB\xBFx");
  EXPECT_TRUE(is_fatal_at(tracker, 1, 1));
}

TEST(TestLineColumn, Utf8BomIsNotCountedOnTheFirstLine)
{
  auto tracker = parse_and_track("\xEF\xBB\xBF[1,x]");
  EXPECT_TRUE(is_fatal_at(tracker, 1, 4));
}

TEST(TestLineColumn, Utf8BomWithNewLines)
{
  auto tracker = parse_and_track("\xEF\xBB\xBF\n x");
  EXPECT_TRUE(is_fatal_at(tracker, 2, 2));
}

TEST(TestLineColumn, Utf8CharactersCountAsCodeUnits)
{
  // "\xC3\xA9" is 'e' with an acute accent, two code units in utf-8.
  auto tracker = parse_and_track("[\"\xC3\xA9\", x]");
  EXPECT_TRUE(is_fatal_at(tracker, 1, 8));
}

///////////////////////////////////////////////////////////////////////////////
// Strings
TEST(TestLineColumn, StringIsNotClosed)
{
  auto tracker = parse_and_track("[\"abc");
  EXPECT_TRUE(is_fatal_at(tracker, "We could not close the string.", 1, 6));
}

TEST(TestLineColumn, StringIsNotClosedOnTheSecondLine)
{
  auto tracker = parse_and_track("[\n\"abc");
  EXPECT_TRUE(is_fatal_at(tracker, "We could not close the string.", 2, 5));
}

TEST(TestLineColumn, TabInsideAString)
{
  auto tracker = parse_and_track("[\"a\tb\"]");
  EXPECT_TRUE(is_fatal_at(tracker, "Invalid character inside the string.", 1, 4));
}

TEST(TestLineColumn, NewLineInsideAString)
{
  auto tracker = parse_and_track("[\"a\nb\"]");
  EXPECT_TRUE(is_fatal_at(tracker, "Invalid character inside the string.", 1, 4));
}

TEST(TestLineColumn, CarriageReturnInsideAString)
{
  auto tracker = parse_and_track("[1,\n\"a\rb\"]");
  EXPECT_TRUE(is_fatal_at(tracker, "Invalid character inside the string.", 2, 3));
}

TEST(TestLineColumn, InvalidEscapeInsideAString)
{
  auto tracker = parse_and_track("[\"a\\qb\"]");
  EXPECT_TRUE(is_fatal_at(tracker, "Invalid/unknown character after single reverse solidus.", 1, 4));
}

TEST(TestLineColumn, InvalidEscapeInsideAStringOnTheSecondLine)
{
  auto tracker = parse_and_track("{\n  \"a\": \"x\\q\"\n}");
  EXPECT_TRUE(is_fatal_at(tracker, "Invalid/unknown character after single reverse solidus.", 2, 10));
}

TEST(TestLineColumn, ValidEscapesBeforeTheError)
{
  auto tracker = parse_and_track("[\"\\n\\t\\\"\", x]");
  EXPECT_TRUE(is_fatal_at(tracker, "Unexpected Token while trying to read value.", 1, 12));
}

///////////////////////////////////////////////////////////////////////////////
// Numbers
TEST(TestLineColumn, LeadingZeroPointsToTheStartOfTheNumber)
{
  auto tracker = parse_and_track("[0123]");
  EXPECT_TRUE(is_fatal_at(tracker, "Numbers cannot have leading zeros.", 1, 2));
}

TEST(TestLineColumn, NegativeLeadingZeroPointsToTheSign)
{
  auto tracker = parse_and_track("[1, -0123]");
  EXPECT_TRUE(is_fatal_at(tracker, "Numbers cannot have leading zeros.", 1, 5));
}

TEST(TestLineColumn, LeadingZeroOnTheSecondLine)
{
  auto tracker = parse_and_track("[\n  0123]");
  EXPECT_TRUE(is_fatal_at(tracker, "Numbers cannot have leading zeros.", 2, 3));
}

TEST(TestLineColumn, MissingFractionDigit)
{
  auto tracker = parse_and_track("[12.]");
  EXPECT_TRUE(is_fatal_at(tracker, "Fraction does not have a number after the token '.'", 1, 5));
}

TEST(TestLineColumn, MissingExponentDigit)
{
  auto tracker = parse_and_track("[1e]");
  EXPECT_TRUE(is_fatal_at(tracker, "Number has exponent 'e' or 'E' but does not have a number.", 1, 4));
}

TEST(TestLineColumn, MissingExponentDigitAfterTheSign)
{
  auto tracker = parse_and_track("[1e+]");
  EXPECT_TRUE(is_fatal_at(tracker, "Number has exponent 'e' or 'E' but does not have a number.", 1, 5));
}

TEST(TestLineColumn, SpaceInsideANumber)
{
  auto tracker = parse_and_track("[1 2]");
  EXPECT_TRUE(is_fatal_at(tracker, "Number has a space between it.", 1, 4));
}

///////////////////////////////////////////////////////////////////////////////
// Words
TEST(TestLineColumn, MisspelledTruePointsToTheStartOfTheWord)
{
  auto tracker = parse_and_track("[tru]");
  EXPECT_TRUE(is_fatal_at(tracker, "Could not read the word 'true'.", 1, 2));
}

TEST(TestLineColumn, MisspelledFalsePointsToTheStartOfTheWord)
{
  auto tracker = parse_and_track("[1, fals]");
  EXPECT_TRUE(is_fatal_at(tracker, "Could not read the word 'false'.", 1, 5));
}

TEST(TestLineColumn, MisspelledNullPointsToTheStartOfTheWord)
{
  auto tracker = parse_and_track("[nul]");
  EXPECT_TRUE(is_fatal_at(tracker, "Could not read the word 'null'.", 1, 2));
}

TEST(TestLineColumn, MisspelledNullOnTheSecondLine)
{
  auto tracker = parse_and_track("{\n  \"a\": nul\n}");
  EXPECT_TRUE(is_fatal_at(tracker, "Could not read the word 'null'.", 2, 8));
}

///////////////////////////////////////////////////////////////////////////////
// Objects
TEST(TestLineColumn, MissingColon)
{
  auto tracker = parse_and_track("{\"a\" 1}");
  EXPECT_TRUE(is_fatal_at(tracker, "Could not locate the expected colon after the key value.", 1, 6));
}

TEST(TestLineColumn, MissingColonAcrossLines)
{
  auto tracker = parse_and_track("{\n  \"a\"\n  1}");
  EXPECT_TRUE(is_fatal_at(tracker, "Could not locate the expected colon after the key value.", 3, 3));
}

TEST(TestLineColumn, MissingCommaInObject)
{
  auto tracker = parse_and_track("{\"a\":1 \"b\":2}");
  EXPECT_TRUE(is_fatal_at(tracker, "Expected a comma after the last element.", 1, 8));
}

TEST(TestLineColumn, TrailingCommaInObject)
{
  auto tracker = parse_and_track("{\"a\":1,}");
  EXPECT_TRUE(is_fatal_at(tracker, "Unexpected end of object, there was a ', ' after the last string.", 1, 8));
}

TEST(TestLineColumn, CommaOutOfOrderInObject)
{
  auto tracker = parse_and_track("{,\"a\":1}");
  EXPECT_TRUE(is_fatal_at(tracker, "Found a comma out of order.", 1, 2));
}

TEST(TestLineColumn, UnknownCharacterInObject)
{
  auto tracker = parse_and_track("{\n  x}");
  EXPECT_TRUE(is_fatal_at(tracker, "Unknown character.", 2, 3));
}

TEST(TestLineColumn, ObjectIsNotClosed)
{
  auto tracker = parse_and_track("{\"a\":1");
  EXPECT_TRUE(is_fatal_at(tracker, "End of the string was found and we didn't find what we needed.", 1, 7));
}

TEST(TestLineColumn, ObjectIsNotClosedOnTheThirdLine)
{
  auto tracker = parse_and_track("{\n  \"a\":1,\n  \"b\":2");
  EXPECT_TRUE(is_fatal_at(tracker, "End of the string was found and we didn't find what we needed.", 3, 8));
}

TEST(TestLineColumn, MaxDepthObject)
{
  TinyJSON::parse_options options = {};
  options.max_depth = 2;
  auto tracker = parse_and_track_with_options("{\"a\":{\"b\":1}}", options);
  EXPECT_TRUE(is_fatal_at(tracker, "Reached the max parse depth (object).", 1, 7));
}

///////////////////////////////////////////////////////////////////////////////
// Arrays
TEST(TestLineColumn, MissingCommaInArrayPointsToTheValue)
{
  auto tracker = parse_and_track("[true false]");
  EXPECT_TRUE(is_fatal_at(tracker, "We found a value but we expected a comma.", 1, 7));
}

TEST(TestLineColumn, MissingCommaInArrayAfterANumber)
{
  auto tracker = parse_and_track("[1, 2 \"a\"]");
  EXPECT_TRUE(is_fatal_at(tracker, "We found a value but we expected a comma.", 1, 7));
}

TEST(TestLineColumn, MissingCommaInArrayOnTheSecondLine)
{
  auto tracker = parse_and_track("[\n  \"a\"\n  \"b\"\n]");
  EXPECT_TRUE(is_fatal_at(tracker, "We found a value but we expected a comma.", 3, 3));
}

TEST(TestLineColumn, TrailingCommaInArray)
{
  auto tracker = parse_and_track("[1,]");
  EXPECT_TRUE(is_fatal_at(tracker, "Unexpected end of array, there was a ', ' after the last string.", 1, 4));
}

TEST(TestLineColumn, CommaOutOfOrderInArray)
{
  auto tracker = parse_and_track("[,1]");
  EXPECT_TRUE(is_fatal_at(tracker, "Found a comma out of order, (2 commas).", 1, 2));
}

TEST(TestLineColumn, TwoCommasInArray)
{
  auto tracker = parse_and_track("[1,,2]");
  EXPECT_TRUE(is_fatal_at(tracker, "Found a comma out of order, (2 commas).", 1, 4));
}

TEST(TestLineColumn, ArrayIsNotClosed)
{
  auto tracker = parse_and_track("[1,2");
  EXPECT_TRUE(is_fatal_at(tracker, "End of the string was found and we didn't find what we needed.", 1, 5));
}

TEST(TestLineColumn, ArrayIsNotClosedOnTheThirdLine)
{
  auto tracker = parse_and_track("[\n1,\n2");
  EXPECT_TRUE(is_fatal_at(tracker, "End of the string was found and we didn't find what we needed.", 3, 2));
}

TEST(TestLineColumn, MaxDepthArray)
{
  TinyJSON::parse_options options = {};
  options.max_depth = 2;
  auto tracker = parse_and_track_with_options("[[1]]", options);
  EXPECT_TRUE(is_fatal_at(tracker, "Reached the max parse depth (array).", 1, 3));
}

TEST(TestLineColumn, MaxDepthArrayAcrossLines)
{
  TinyJSON::parse_options options = {};
  options.max_depth = 2;
  auto tracker = parse_and_track_with_options("[\n  [\n    1\n  ]\n]", options);
  EXPECT_TRUE(is_fatal_at(tracker, "Reached the max parse depth (array).", 2, 4));
}

///////////////////////////////////////////////////////////////////////////////
// Root
TEST(TestLineColumn, MultipleRootValues)
{
  auto tracker = parse_and_track("[1] [2]");
  EXPECT_TRUE(is_fatal_at(tracker, "Unexpected multiple JSON values in root.", 1, 5));
}

TEST(TestLineColumn, MultipleRootValuesOnTheSecondLine)
{
  auto tracker = parse_and_track("{}\n{}");
  EXPECT_TRUE(is_fatal_at(tracker, "Unexpected multiple JSON values in root.", 2, 1));
}

TEST(TestLineColumn, Rfc4627RootMustBeObjectOrArray)
{
  auto tracker = parse_and_track_with_specification("\n  \"abc\"", TinyJSON::parse_options::rfc4627);
  EXPECT_TRUE(is_fatal_at(tracker, "RFC 4627: A JSON text must be either an object or an array.", 2, 3));
}

TEST(TestLineColumn, Rfc4627RootNumber)
{
  auto tracker = parse_and_track_with_specification("  42", TinyJSON::parse_options::rfc4627);
  EXPECT_TRUE(is_fatal_at(tracker, "RFC 4627: A JSON text must be either an object or an array.", 1, 3));
}

TEST(TestLineColumn, EmptySource)
{
  auto tracker = parse_and_track("");
  EXPECT_TRUE(is_fatal_at(tracker, "The JSON text is empty or contains no value.", 1, 1));
}

TEST(TestLineColumn, WhiteSpaceOnlySource)
{
  auto tracker = parse_and_track("   \n  ");
  EXPECT_TRUE(is_fatal_at(tracker, "The JSON text is empty or contains no value.", 2, 3));
}

TEST(TestLineColumn, IsValidAlsoReportsThePosition)
{
  position_tracker tracker;
  TinyJSON::parse_options options = {};
  options.callback_function = std::ref(tracker);
  EXPECT_FALSE(TinyJSON::TJ::is_valid("[1,\nx]", options));
  EXPECT_TRUE(is_fatal_at(tracker, "Unexpected Token while trying to read value.", 2, 1));
}

///////////////////////////////////////////////////////////////////////////////
// Json5
TEST(TestLineColumn, Json5ErrorAfterASingleLineComment)
{
  auto tracker = parse5_and_track("// comment\n[1,x]");
  EXPECT_TRUE(is_fatal_at(tracker, "Unexpected Token while trying to read value.", 2, 4));
}

TEST(TestLineColumn, Json5ErrorAfterAMultiLineComment)
{
  auto tracker = parse5_and_track("/* line1\nline2\n*/ [1,x]");
  EXPECT_TRUE(is_fatal_at(tracker, "Unexpected Token while trying to read value.", 3, 7));
}

TEST(TestLineColumn, Json5ErrorAfterAMultiLineCommentWithCarriageReturnLineFeed)
{
  auto tracker = parse5_and_track("/* line1\r\nline2\r\n*/ [1,x]");
  EXPECT_TRUE(is_fatal_at(tracker, "Unexpected Token while trying to read value.", 3, 7));
}

TEST(TestLineColumn, Json5MultiLineCommentIsNotClosed)
{
  auto tracker = parse5_and_track("[1, /* abc\n def");
  EXPECT_TRUE(is_fatal_at(tracker, "Multi-line comment was not closed.", 2, 5));
}

TEST(TestLineColumn, Json5InvalidCommentStart)
{
  auto tracker = parse5_and_track("[1, /x]");
  EXPECT_TRUE(is_fatal_at(tracker, "Invalid comment start.", 1, 6));
}

TEST(TestLineColumn, Json5OnlyAComment)
{
  auto tracker = parse5_and_track("/* x */");
  EXPECT_TRUE(is_fatal_at(tracker, "A JSON text cannot contain only a comment.", 1, 8));
}

TEST(TestLineColumn, Json5OnlyASingleLineComment)
{
  auto tracker = parse5_and_track("// hi\n");
  EXPECT_TRUE(is_fatal_at(tracker, "A JSON text cannot contain only a comment.", 2, 1));
}

TEST(TestLineColumn, CommentsAreRejectedOutsideJson5)
{
  auto tracker = parse_and_track("[1,\n // c\n2]");
  EXPECT_TRUE(is_fatal_at(tracker, "Comments are only allowed in json5_1_0_0 specification.", 2, 2));
}

TEST(TestLineColumn, SingleQuotesAreRejectedOutsideJson5)
{
  auto tracker = parse_and_track("[\n 'a']");
  EXPECT_TRUE(is_fatal_at(tracker, "Single quotes are only allowed in json5_1_0_0 specification.", 2, 2));
}

TEST(TestLineColumn, SingleQuotedKeysAreRejectedOutsideJson5)
{
  auto tracker = parse_and_track("{'a':1}");
  EXPECT_TRUE(is_fatal_at(tracker, "Single quotes are only allowed in json5_1_0_0 specification.", 1, 2));
}

TEST(TestLineColumn, Json5UnquotedKeyMissingColon)
{
  auto tracker = parse5_and_track("{\n  abc 1}");
  EXPECT_TRUE(is_fatal_at(tracker, "Could not locate the expected colon after the unquoted key.", 2, 7));
}

TEST(TestLineColumn, Json5NewLineInsideAStringIsCounted)
{
  auto tracker = parse5_and_track("['a\nb', x]");
  EXPECT_TRUE(is_fatal_at(tracker, "Unexpected Token while trying to read value.", 2, 5));
}

TEST(TestLineColumn, Json5ErrorDeepInsideADocument)
{
  auto tracker = parse5_and_track("{\n  // a comment\n  a: 1,\n  b: [\n    'x',\n    tru\n  ]\n}");
  EXPECT_TRUE(is_fatal_at(tracker, "Could not read the word 'true'.", 6, 5));
}

///////////////////////////////////////////////////////////////////////////////
// Exceptions
TEST(TestLineColumn, ExceptionHasTheSameLineAndColumnAsTheCallback)
{
  position_tracker tracker;
  TinyJSON::parse_options options = {};
  options.throw_exception = true;
  options.callback_function = std::ref(tracker);
  try
  {
    auto* json = TinyJSON::TJ::parse("[1,\n  x]", options);
    delete json;
    FAIL() << "Expected a TJParseException";
  }
  catch (const TinyJSON::TJParseException& ex)
  {
    EXPECT_EQ(2, ex.line());
    EXPECT_EQ(3, ex.column());
    EXPECT_STREQ("Unexpected Token while trying to read value.", ex.what());
  }
  EXPECT_TRUE(is_fatal_at(tracker, "Unexpected Token while trying to read value.", 2, 3));
}

TEST(TestLineColumn, ExceptionFromParse5HasTheLineAndColumn)
{
  TinyJSON::parse_options options = {};
  options.throw_exception = true;
  try
  {
    auto* json = TinyJSON::TJ::parse5("{\n  a: 1,\n  b: nul\n}", options);
    delete json;
    FAIL() << "Expected a TJParseException";
  }
  catch (const TinyJSON::TJParseException& ex)
  {
    EXPECT_EQ(3, ex.line());
    EXPECT_EQ(6, ex.column());
    EXPECT_STREQ("Could not read the word 'null'.", ex.what());
  }
}

TEST(TestLineColumn, ExceptionDefaultsToUnknownPosition)
{
  TinyJSON::TJParseException ex("Something went wrong");
  EXPECT_EQ(-1, ex.line());
  EXPECT_EQ(-1, ex.column());
  EXPECT_STREQ("Something went wrong", ex.what());
}

TEST(TestLineColumn, ExceptionCopyKeepsTheLineAndColumn)
{
  TinyJSON::TJParseException ex("Something went wrong", 3, 4);
  TinyJSON::TJParseException copy(ex);
  EXPECT_EQ(3, copy.line());
  EXPECT_EQ(4, copy.column());
  EXPECT_STREQ("Something went wrong", copy.what());

  TinyJSON::TJParseException assigned("Something else");
  assigned = ex;
  EXPECT_EQ(3, assigned.line());
  EXPECT_EQ(4, assigned.column());
  EXPECT_STREQ("Something went wrong", assigned.what());
}

///////////////////////////////////////////////////////////////////////////////
// Unknown positions
TEST(TestLineColumn, NullSourceHasNoPosition)
{
  auto tracker = parse_and_track(nullptr);
  EXPECT_TRUE(is_fatal_at(tracker, "The given source is null.", -1, -1));
}

TEST(TestLineColumn, NullSourceExceptionHasNoPosition)
{
  TinyJSON::parse_options options = {};
  options.throw_exception = true;
  try
  {
    auto* json = TinyJSON::TJ::parse(nullptr, options);
    delete json;
    FAIL() << "Expected a TJParseException";
  }
  catch (const TinyJSON::TJParseException& ex)
  {
    EXPECT_EQ(-1, ex.line());
    EXPECT_EQ(-1, ex.column());
  }
}

TEST(TestLineColumn, NullFilePathHasNoPosition)
{
  position_tracker tracker;
  TinyJSON::parse_options options = {};
  options.callback_function = std::ref(tracker);
  auto* json = TinyJSON::TJ::parse_file(nullptr, options);
  EXPECT_EQ(nullptr, json);
  EXPECT_TRUE(is_fatal_at(tracker, "The given file path is null!", -1, -1));
}

TEST(TestLineColumn, MissingFileHasNoPosition)
{
  position_tracker tracker;
  TinyJSON::parse_options options = {};
  options.callback_function = std::ref(tracker);
  auto* json = TinyJSON::TJ::parse_file("this_line_column_file_does_not_exist.json", options);
  EXPECT_EQ(nullptr, json);
  EXPECT_EQ(1, tracker.error_count);
  EXPECT_EQ(-1, tracker.line);
  EXPECT_EQ(-1, tracker.column);
}

TEST(TestLineColumn, DirectoryHasNoPosition)
{
  position_tracker tracker;
  TinyJSON::parse_options options = {};
  options.callback_function = std::ref(tracker);
  auto* json = TinyJSON::TJ::parse_file(".", options);
  EXPECT_EQ(nullptr, json);
  EXPECT_EQ(1, tracker.error_count);
  EXPECT_EQ(-1, tracker.line);
  EXPECT_EQ(-1, tracker.column);
}

TEST(TestLineColumn, KeyNotFoundWarningHasNoPosition)
{
  position_tracker tracker;
  TinyJSON::parse_options options = {};
  options.callback_function = std::ref(tracker);
  auto* json = TinyJSON::TJ::parse("{\n  \"a\": 1\n}", options);
  ASSERT_NE(nullptr, json);
  auto* object = dynamic_cast<TinyJSON::TJValueObject*>(json);
  ASSERT_NE(nullptr, object);

  object->try_get_string("missing");
  EXPECT_EQ(1, tracker.warning_count);
  EXPECT_EQ("The key 'missing' was not found!", tracker.message);
  EXPECT_EQ(-1, tracker.line);
  EXPECT_EQ(-1, tracker.column);
  delete json;
}

TEST(TestLineColumn, StrictKeyNotFoundHasNoPosition)
{
  position_tracker tracker;
  TinyJSON::parse_options options = {};
  options.strict = true;
  options.throw_exception = true;
  options.callback_function = std::ref(tracker);
  auto* json = TinyJSON::TJ::parse("{\n  \"a\": 1\n}", options);
  ASSERT_NE(nullptr, json);
  auto* object = dynamic_cast<TinyJSON::TJValueObject*>(json);
  ASSERT_NE(nullptr, object);

  try
  {
    object->get_string("missing");
    FAIL() << "Expected a TJParseException";
  }
  catch (const TinyJSON::TJParseException& ex)
  {
    EXPECT_EQ(-1, ex.line());
    EXPECT_EQ(-1, ex.column());
  }
  EXPECT_TRUE(is_fatal_at(tracker, "The key 'missing' was not found!", -1, -1));
  delete json;
}

TEST(TestLineColumn, ConversionErrorHasNoPosition)
{
  position_tracker tracker;
  TinyJSON::parse_options options = {};
  options.strict = true;
  options.throw_exception = true;
  options.callback_function = std::ref(tracker);
  auto* json = TinyJSON::TJ::parse("\n\n  \"abc\"", options);
  ASSERT_NE(nullptr, json);

  try
  {
    json->get_number();
    FAIL() << "Expected a TJParseException";
  }
  catch (const TinyJSON::TJParseException& ex)
  {
    EXPECT_EQ(-1, ex.line());
    EXPECT_EQ(-1, ex.column());
  }
  EXPECT_TRUE(is_fatal_at(tracker, "The value is not a number!", -1, -1));
  delete json;
}

///////////////////////////////////////////////////////////////////////////////
// Files
TEST(TestLineColumn, ParseFileReportsThePosition)
{
  {
    std::ofstream file(line_column_file_name, std::ios::binary);
    file << "{\n  \"a\": 1,\n  \"b\": x\n}";
  }

  position_tracker tracker;
  TinyJSON::parse_options options = {};
  options.callback_function = std::ref(tracker);
  auto* json = TinyJSON::TJ::parse_file(line_column_file_name, options);
  EXPECT_EQ(nullptr, json);
  EXPECT_TRUE(is_fatal_at(tracker, "Unexpected Token while trying to read value.", 3, 8));

  std::remove(line_column_file_name);
}

TEST(TestLineColumn, ParseFileWithBomAndCarriageReturnLineFeed)
{
  {
    std::ofstream file(line_column_file_name, std::ios::binary);
    file << "\xEF\xBB\xBF{\r\n  \"a\": 1,\r\n  \"b\": x\r\n}";
  }

  position_tracker tracker;
  TinyJSON::parse_options options = {};
  options.throw_exception = true;
  options.callback_function = std::ref(tracker);
  try
  {
    auto* json = TinyJSON::TJ::parse_file(line_column_file_name, options);
    delete json;
    FAIL() << "Expected a TJParseException";
  }
  catch (const TinyJSON::TJParseException& ex)
  {
    EXPECT_EQ(3, ex.line());
    EXPECT_EQ(8, ex.column());
  }
  EXPECT_TRUE(is_fatal_at(tracker, "Unexpected Token while trying to read value.", 3, 8));

  std::remove(line_column_file_name);
}

///////////////////////////////////////////////////////////////////////////////
// Duplicate keys
TEST(TestLineColumn, DuplicateKeyTracePointsToTheSecondKey)
{
  auto tracker = parse_and_track("{\"a\":1,\"a\":2}");
  EXPECT_EQ(0, tracker.fatal_count);
  EXPECT_EQ(1, tracker.trace_count);
  EXPECT_EQ("Duplicate key found, overwriting.", tracker.trace_message);
  EXPECT_EQ(1, tracker.trace_line);
  EXPECT_EQ(8, tracker.trace_column);
}

TEST(TestLineColumn, DuplicateKeyTraceOnALaterLine)
{
  auto tracker = parse_and_track("{\n  \"a\": 1,\n  \"a\": 2\n}");
  EXPECT_EQ(0, tracker.fatal_count);
  EXPECT_EQ(1, tracker.trace_count);
  EXPECT_EQ(3, tracker.trace_line);
  EXPECT_EQ(3, tracker.trace_column);
}

TEST(TestLineColumn, DuplicateKeyTraceInANestedObject)
{
  auto tracker = parse_and_track("{\n  \"x\": {\n    \"a\": 1,\n    \"a\": 2\n  }\n}");
  EXPECT_EQ(0, tracker.fatal_count);
  EXPECT_EQ(1, tracker.trace_count);
  EXPECT_EQ(4, tracker.trace_line);
  EXPECT_EQ(5, tracker.trace_column);
}

TEST(TestLineColumn, Json5DuplicateUnquotedKeyTrace)
{
  auto tracker = parse5_and_track("{a:1,\n a:2}");
  EXPECT_EQ(0, tracker.fatal_count);
  EXPECT_EQ(1, tracker.trace_count);
  EXPECT_EQ(2, tracker.trace_line);
  EXPECT_EQ(2, tracker.trace_column);
}

TEST(TestLineColumn, DuplicateKeyFromSetHasNoPosition)
{
  position_tracker tracker;
  TinyJSON::parse_options options = {};
  options.callback_function = std::ref(tracker);
  TinyJSON::TJValueObject object(options);
  object.set("a", 1);
  object.set("a", 2);
  EXPECT_EQ(1, tracker.trace_count);
  EXPECT_EQ(-1, tracker.trace_line);
  EXPECT_EQ(-1, tracker.trace_column);
}

///////////////////////////////////////////////////////////////////////////////
// Success
TEST(TestLineColumn, ValidMultiLineDocumentHasNoMessages)
{
  auto tracker = parse_and_track("{\n  \"a\": [1, 2, 3],\r\n  \"b\": {\"c\": true}\r  \n}\n");
  EXPECT_EQ(0, tracker.fatal_count);
  EXPECT_EQ(0, tracker.error_count);
  EXPECT_EQ(0, tracker.warning_count);
  EXPECT_EQ(0, tracker.trace_count);
}

TEST(TestLineColumn, ValidJson5MultiLineDocumentHasNoMessages)
{
  auto tracker = parse5_and_track("// comment\n{\n  a: 1, /* block\n comment */\n  b: 'x',\n}\n");
  EXPECT_EQ(0, tracker.fatal_count);
  EXPECT_EQ(0, tracker.error_count);
  EXPECT_EQ(0, tracker.warning_count);
}
