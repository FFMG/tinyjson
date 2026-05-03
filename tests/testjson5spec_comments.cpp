// Licensed to Florent Guelfucci under one or more agreements.
// Florent Guelfucci licenses this file to you under the MIT license.
// See the LICENSE file in the project root for more information.

// These tests are based on the JSON5 specification test suite.
// Source: https://github.com/json5/json5-tests/tree/master/comments
// License: MIT

#include <gtest/gtest.h>
#include "../src/TinyJSON.h"
#include "testshelper.h"

using namespace TinyJSON;

TEST(TJJSON5SpecComments, BlockCommentFollowingArrayElement)
{
    const TJCHAR* json = TJCHARPREFIX("[1/* comment */,2]");
    auto* tj = TJ::parse5(json);
    ASSERT_NE(nullptr, tj);
    ASSERT_TRUE(tj->is_array());
    auto* arr = static_cast<TJValueArray*>(tj);
    ASSERT_EQ(2, arr->get_number_of_items());
    ASSERT_EQ(3, arr->get_number_of_elements());
    ASSERT_TRUE(arr->at(0)->is_number());
    ASSERT_TRUE(arr->at(1)->is_number());
    ASSERT_TRUE(arr->element_at(1)->is_comment());
    delete tj;
}

TEST(TJJSON5SpecComments, BlockCommentFollowingTopLevelValue)
{
    const TJCHAR* json = TJCHARPREFIX("1/* comment */");
    auto* tj = TJ::parse5(json);
    ASSERT_NE(nullptr, tj);
    ASSERT_TRUE(tj->is_number());
    delete tj;
}

TEST(TJJSON5SpecComments, BlockCommentInString)
{
    const TJCHAR* json = TJCHARPREFIX("\"/* comment */\"");
    auto* tj = TJ::parse5(json);
    ASSERT_NE(nullptr, tj);
    ASSERT_TRUE(tj->is_string());
    ASSERT_STREQ(TJCHARPREFIX("/* comment */"), tj->get_string());
    delete tj;
}

TEST(TJJSON5SpecComments, BlockCommentPrecedingTopLevelValue)
{
    const TJCHAR* json = TJCHARPREFIX("/* comment */1");
    auto* tj = TJ::parse5(json);
    ASSERT_NE(nullptr, tj);
    ASSERT_TRUE(tj->is_number());
    delete tj;
}

TEST(TJJSON5SpecComments, BlockCommentWithAsterisks)
{
    const TJCHAR* json1 = TJCHARPREFIX("/**/1");
    auto* tj1 = TJ::parse5(json1);
    ASSERT_NE(nullptr, tj1);
    ASSERT_TRUE(tj1->is_number());
    delete tj1;

    const TJCHAR* json2 = TJCHARPREFIX("/***/1");
    auto* tj2 = TJ::parse5(json2);
    ASSERT_NE(nullptr, tj2);
    ASSERT_TRUE(tj2->is_number());
    delete tj2;

    const TJCHAR* json3 = TJCHARPREFIX("/****/1");
    auto* tj3 = TJ::parse5(json3);
    ASSERT_NE(nullptr, tj3);
    ASSERT_TRUE(tj3->is_number());
    delete tj3;
}

TEST(TJJSON5SpecComments, InlineCommentFollowingArrayElement)
{
    const TJCHAR* json = TJCHARPREFIX("[1// comment\n,2]");
    auto* tj = TJ::parse5(json);
    ASSERT_NE(nullptr, tj);
    ASSERT_TRUE(tj->is_array());
    auto* arr = static_cast<TJValueArray*>(tj);
    ASSERT_EQ(2, arr->get_number_of_items());
    ASSERT_EQ(3, arr->get_number_of_elements());
    ASSERT_TRUE(arr->element_at(1)->is_comment());
    delete tj;
}

TEST(TJJSON5SpecComments, InlineCommentFollowingTopLevelValue)
{
    const TJCHAR* json = TJCHARPREFIX("1// comment");
    auto* tj = TJ::parse5(json);
    ASSERT_NE(nullptr, tj);
    ASSERT_TRUE(tj->is_number());
    delete tj;
}

TEST(TJJSON5SpecComments, InlineCommentInString)
{
    const TJCHAR* json = TJCHARPREFIX("\"// comment\"");
    auto* tj = TJ::parse5(json);
    ASSERT_NE(nullptr, tj);
    ASSERT_TRUE(tj->is_string());
    ASSERT_STREQ(TJCHARPREFIX("// comment"), tj->get_string());
    delete tj;
}

TEST(TJJSON5SpecComments, InlineCommentPrecedingTopLevelValue)
{
    const TJCHAR* json = TJCHARPREFIX("// comment\n1");
    auto* tj = TJ::parse5(json);
    ASSERT_NE(nullptr, tj);
    ASSERT_TRUE(tj->is_number());
    delete tj;
}

TEST(TJJSON5SpecComments, MultiLineBlockComment)
{
    const TJCHAR* json = TJCHARPREFIX("/*\nline 1\nline 2\n*/1");
    auto* tj = TJ::parse5(json);
    ASSERT_NE(nullptr, tj);
    ASSERT_TRUE(tj->is_number());
    delete tj;
}

TEST(TJJSON5SpecComments, LineCommentAtEOF)
{
    const TJCHAR* json = TJCHARPREFIX("1 // line comment");
    auto* tj = TJ::parse5(json);
    ASSERT_NE(nullptr, tj);
    ASSERT_TRUE(tj->is_number());
    delete tj;
}

TEST(TJJSON5SpecComments, BlockCommentAtEOF)
{
    const TJCHAR* json = TJCHARPREFIX("1 /* block comment */");
    auto* tj = TJ::parse5(json);
    ASSERT_NE(nullptr, tj);
    ASSERT_TRUE(tj->is_number());
    delete tj;
}
