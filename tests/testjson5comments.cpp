// Licensed to Florent Guelfucci under one or more agreements.
// Florent Guelfucci licenses this file to you under the MIT license.
// See the LICENSE file in the project root for more information.
#include <gtest/gtest.h>
#include "../src/TinyJSON.h"
#include "testshelper.h"

using namespace TinyJSON;

TEST(TJComment, SingleLineCommentAtRoot)
{
  const TJCHAR* json = TJCHARPREFIX("// this is a comment\n{\"a\": 1}");
  auto* tj = TJ::parse(json);
  ASSERT_NE(nullptr, tj);
  ASSERT_TRUE(tj->is_object());
  auto* obj = static_cast<TJValueObject*>(tj);
  ASSERT_EQ(1, obj->at(0)->value()->get_number());
  delete tj;
}

TEST(TJComment, MultiLineCommentAtRoot)
{
  const TJCHAR* json = TJCHARPREFIX("/* this is a \n multi line comment */ {\"a\": 1}");
  auto* tj = TJ::parse(json);
  ASSERT_NE(nullptr, tj);
  ASSERT_TRUE(tj->is_object());
  auto* obj = static_cast<TJValueObject*>(tj);
  ASSERT_EQ(1, obj->at(0)->value()->get_number());
  delete tj;
}

TEST(TJComment, CommentInObject)
{
  const TJCHAR* json = TJCHARPREFIX("{\"a\": 1, // comment\n \"b\": 2}");
  auto* tj = TJ::parse(json);
  ASSERT_NE(nullptr, tj);
  ASSERT_TRUE(tj->is_object());
  auto* obj = static_cast<TJValueObject*>(tj);
  // It should have 3 members: "a", "", "b"
  ASSERT_EQ(3, obj->get_number_of_items());
  ASSERT_TRUE(obj->at(1)->value()->is_comment());
  ASSERT_STREQ(TJCHARPREFIX("// comment"), static_cast<TJValueComment*>(obj->at(1)->value())->raw_value());
  delete tj;
}

TEST(TJComment, CommentInArray)
{
  const TJCHAR* json = TJCHARPREFIX("[1, /* comment */ 2]");
  auto* tj = TJ::parse(json);
  ASSERT_NE(nullptr, tj);
  ASSERT_TRUE(tj->is_array());
  auto* arr = static_cast<TJValueArray*>(tj);
  // It should have 3 elements: 1, comment, 2
  ASSERT_EQ(3, arr->get_number_of_items());
  ASSERT_TRUE(arr->at(1)->is_comment());
  ASSERT_STREQ(TJCHARPREFIX("/* comment */"), static_cast<TJValueComment*>(arr->at(1))->raw_value());
  delete tj;
}

TEST(TJComment, MinifyExcludesComments)
{
  const TJCHAR* json = TJCHARPREFIX("{\"a\": 1, // comment\n \"b\": [2, /* comment */ 3]}");
  auto* tj = TJ::parse(json);
  ASSERT_NE(nullptr, tj);
  
  const TJCHAR* minified = tj->dump(formating::minify);
  ASSERT_STREQ(TJCHARPREFIX("{\"a\":1,\"b\":[2,3]}"), minified);
  
  delete tj;
}

TEST(TJComment, IndentedIncludesComments)
{
  const TJCHAR* json = TJCHARPREFIX("{\n  \"a\": 1, // comment\n  \"b\": 2\n}");
  auto* tj = TJ::parse(json);
  ASSERT_NE(nullptr, tj);
  
  const TJCHAR* indented = tj->dump(formating::indented);
  // The exact indentation might vary, but it should contain the comment
  ASSERT_NE(nullptr, strstr(indented, "// comment"));
  
  delete tj;
}

TEST(TJComment, OnlyComment)
{
  const TJCHAR* json = TJCHARPREFIX("// only a comment");
  auto* tj = TJ::parse(json);
  ASSERT_NE(nullptr, tj);
  ASSERT_TRUE(tj->is_comment());
  ASSERT_STREQ(TJCHARPREFIX("// only a comment"), static_cast<TJValueComment*>(tj)->raw_value());
  delete tj;
}
