// Licensed to Florent Guelfucci under one or more agreements.
// Florent Guelfucci licenses this file to you under the MIT license.
// See the LICENSE file in the project root for more information.
#include <gtest/gtest.h>
#include "../src/TinyJSON.h"
#include "testshelper.h"

using namespace TinyJSON;

TEST(TJ5Comment, SingleLineCommentWithValueAtRoot)
{
  const TJCHAR* json = TJCHARPREFIX("// this is a comment\n{\"a\": 1}");
  auto* tj = TJ::parse5(json);
  ASSERT_NE(nullptr, tj);
  ASSERT_TRUE(tj->is_object());
  auto* obj = static_cast<TJValueObject*>(tj);
  ASSERT_EQ(1, obj->at(0)->value()->get_number());
  delete tj;
}

TEST(TJ5Comment, MultiLineCommentWithValueAtRoot)
{
  const TJCHAR* json = TJCHARPREFIX("/* this is a \n multi line comment */ {\"a\": 1}");
  auto* tj = TJ::parse5(json);
  ASSERT_NE(nullptr, tj);
  ASSERT_TRUE(tj->is_object());
  auto* obj = static_cast<TJValueObject*>(tj);
  ASSERT_EQ(1, obj->at(0)->value()->get_number());
  delete tj;
}

TEST(TJ5Comment, CommentInObject)
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

TEST(TJ5Comment, CommentInArray)
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

TEST(TJ5Comment, MinifyExcludesComments)
{
  const TJCHAR* json = TJCHARPREFIX("{\"a\": 1, // comment\n \"b\": [2, /* comment */ 3]}");
  auto* tj = TJ::parse5(json);
  ASSERT_NE(nullptr, tj);
  
  const TJCHAR* minified = tj->dump(formatting::minify);
  ASSERT_STREQ(TJCHARPREFIX("{\"a\":1,\"b\":[2,3]}"), minified);
  
  delete tj;
}

TEST(TJ5Comment, IndentedIncludesComments)
{
  const TJCHAR* json = TJCHARPREFIX("{\n  \"a\": 1, // comment\n  \"b\": 2\n}");
  auto* tj = TJ::parse5(json);
  ASSERT_NE(nullptr, tj);
  
  const TJCHAR* indented = tj->dump(formatting::indented);
  // The exact indentation might vary, but it should contain the comment
  ASSERT_NE(nullptr, strstr(indented, "// comment"));
  
  delete tj;
}

TEST(TJ5Comment, OnlyCommentThrows)
{
  const TJCHAR* json = TJCHARPREFIX("// only a comment");
  auto* tj = TJ::parse5(json);
  ASSERT_EQ(nullptr, tj);
}

TEST(TJ5Comment, MultipleCommentsOnlyThrows)
{
  const TJCHAR* json = TJCHARPREFIX("/* \n this is a \n multi line comment \n */ \n // hello");
  auto* tj = TJ::parse5(json);
  ASSERT_EQ(nullptr, tj);
}

TEST(TJ5Comment, MultipleCommentsWithBooleanAtRoot)
{
  const TJCHAR* json = TJCHARPREFIX("/* \n this is a \n multi line comment \n */ \n // hello \n true");
  auto* tj = TJ::parse5(json);
  ASSERT_NE(nullptr, tj);
  ASSERT_TRUE(tj->is_true());
  delete tj;
}

TEST(TJ5Comment, StandardParseRejectsComments)
{
  const TJCHAR* json = TJCHARPREFIX("// comment\n{\"a\": 1}");
  auto* tj = TJ::parse(json);
  ASSERT_EQ(nullptr, tj);
}

TEST(TJ5Comment, RFC4627RejectsComments)
{
  const TJCHAR* json = TJCHARPREFIX("// comment\n{\"a\": 1}");
  parse_options options;
  options.specification = parse_options::rfc4627;
  auto* tj = TJ::parse(json, options);
  ASSERT_EQ(nullptr, tj);
}

TEST(TJ5Comment, CommentAsFirstItemInArrayList)
{
  {
    auto json = TinyJSON::TJ::parse5(R"(
[
  // open a long trade
  "a-text"
]
)"
);
    ASSERT_NE(nullptr, json);
    auto* arr = static_cast<TJValueArray*>(json);
    ASSERT_NE(nullptr, arr);
    ASSERT_EQ(2, arr->get_number_of_elements()); // Comment + string
  }
}

TEST(TJ5Comment, CommentAsFirstItemInObectList)
{
  {
    auto json = TinyJSON::TJ::parse5(R"(
{
  "a": "a-text"
}
)"
);
    ASSERT_NE(nullptr, json);
    auto* obj = static_cast<TJValueObject*>(json);
    ASSERT_NE(nullptr, obj);
    ASSERT_STRCASEEQ("a-text", obj->get_string("a"));
  }
  {
    auto json = TinyJSON::TJ::parse5(R"(
{
  // open a long trade
  "a": "a-text"
}
)"
);
    ASSERT_NE(nullptr, json);
    auto* obj = static_cast<TJValueObject*>(json);
    ASSERT_NE(nullptr, obj);
    ASSERT_STRCASEEQ("a-text", obj->get_string("a"));
  }
}