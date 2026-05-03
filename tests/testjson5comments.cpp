// Licensed to Florent Guelfucci under one or more agreements.
// Florent Guelfucci licenses this file to you under the MIT license.
// See the LICENSE file in the project root for more information.
#include <gtest/gtest.h>
#include "../src/TinyJSON.h"
#include "testshelper.h"

using namespace TinyJSON;

TEST(TJComment, SingleLineCommentWithValueAtRoot)
{
  const TJCHAR* json = TJCHARPREFIX("// this is a comment\n{\"a\": 1}");
  auto* tj = TJ::parse5(json);
  ASSERT_NE(nullptr, tj);
  ASSERT_TRUE(tj->is_object());
  auto* obj = static_cast<TJValueObject*>(tj);
  ASSERT_EQ(1, obj->at(0)->value()->get_number());
  delete tj;
}

TEST(TJComment, MultiLineCommentWithValueAtRoot)
{
  const TJCHAR* json = TJCHARPREFIX("/* this is a \n multi line comment */ {\"a\": 1}");
  auto* tj = TJ::parse5(json);
  ASSERT_NE(nullptr, tj);
  ASSERT_TRUE(tj->is_object());
  auto* obj = static_cast<TJValueObject*>(tj);
  ASSERT_EQ(1, obj->at(0)->value()->get_number());
  delete tj;
}

TEST(TJComment, CommentInObject)
{
  const TJCHAR* json = TJCHARPREFIX("{\"a\": 1, // comment\n \"b\": 2}");
  auto* tj = TJ::parse5(json);
  ASSERT_NE(nullptr, tj);
  ASSERT_TRUE(tj->is_object());
  auto* obj = static_cast<TJValueObject*>(tj);
  // It should have 2 logical items: "a", "b"
  ASSERT_EQ(2, obj->get_number_of_items());
  // It should have 3 raw elements
  ASSERT_EQ(3, obj->get_number_of_elements());
  ASSERT_TRUE(obj->element_at(1)->value()->is_comment());
  ASSERT_STREQ(TJCHARPREFIX("// comment"), static_cast<TJValueComment*>(obj->element_at(1)->value())->raw_value());
  delete tj;
}

TEST(TJComment, CommentInArray)
{
  const TJCHAR* json = TJCHARPREFIX("[1, /* comment */ 2]");
  auto* tj = TJ::parse5(json);
  ASSERT_NE(nullptr, tj);
  ASSERT_TRUE(tj->is_array());
  auto* arr = static_cast<TJValueArray*>(tj);
  // It should have 2 logical items: 1, 2
  ASSERT_EQ(2, arr->get_number_of_items());
  // It should have 3 raw elements
  ASSERT_EQ(3, arr->get_number_of_elements());
  ASSERT_TRUE(arr->element_at(1)->is_comment());
  ASSERT_STREQ(TJCHARPREFIX("/* comment */"), static_cast<TJValueComment*>(arr->element_at(1))->raw_value());
  delete tj;
}

TEST(TJComment, MinifyExcludesComments)
{
  const TJCHAR* json = TJCHARPREFIX("{\"a\": 1, // comment\n \"b\": [2, /* comment */ 3]}");
  auto* tj = TJ::parse5(json);
  ASSERT_NE(nullptr, tj);
  
  const TJCHAR* minified = tj->dump(formating::minify);
  ASSERT_STREQ(TJCHARPREFIX("{\"a\":1,\"b\":[2,3]}"), minified);
  
  delete tj;
}

TEST(TJComment, IndentedIncludesComments)
{
  const TJCHAR* json = TJCHARPREFIX("{\n  \"a\": 1, // comment\n  \"b\": 2\n}");
  auto* tj = TJ::parse5(json);
  ASSERT_NE(nullptr, tj);
  
  const TJCHAR* indented = tj->dump(formating::indented);
  // The exact indentation might vary, but it should contain the comment
  ASSERT_NE(nullptr, strstr(indented, "// comment"));
  
  delete tj;
}

TEST(TJComment, OnlyCommentThrows)
{
  const TJCHAR* json = TJCHARPREFIX("// only a comment");
  auto* tj = TJ::parse5(json);
  ASSERT_EQ(nullptr, tj);
}

TEST(TJComment, MultipleCommentsOnlyThrows)
{
  const TJCHAR* json = TJCHARPREFIX("/* \n this is a \n multi line comment \n */ \n // hello");
  auto* tj = TJ::parse5(json);
  ASSERT_EQ(nullptr, tj);
}

TEST(TJComment, MultipleCommentsWithBooleanAtRoot)
{
  const TJCHAR* json = TJCHARPREFIX("/* \n this is a \n multi line comment \n */ \n // hello \n true");
  auto* tj = TJ::parse5(json);
  ASSERT_NE(nullptr, tj);
  ASSERT_TRUE(tj->is_true());
  delete tj;
}

TEST(TJComment, StandardParseRejectsComments)
{
  const TJCHAR* json = TJCHARPREFIX("// comment\n{\"a\": 1}");
  auto* tj = TJ::parse(json);
  ASSERT_EQ(nullptr, tj);
}

TEST(TJComment, RFC4627RejectsComments)
{
  const TJCHAR* json = TJCHARPREFIX("// comment\n{\"a\": 1}");
  parse_options options;
  options.specification = parse_options::rfc4627;
  auto* tj = TJ::parse(json, options);
  ASSERT_EQ(nullptr, tj);
}
