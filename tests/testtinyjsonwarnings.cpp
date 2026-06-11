// Licensed to Florent Guelfucci under one or more agreements.
// Florent Guelfucci licenses this file to you under the MIT license.
// See the LICENSE file in the project root for more information.
#include <gtest/gtest.h>
#define TJ_USE_CHAR 1
#include "../src/TinyJSON.h"

template <typename F>
void assert_warning_fired(TinyJSON::TJValueObject* obj, int& warning_count, std::string& last_warning_message, F&& func)
{
  warning_count = 0;
  last_warning_message = "";
  func(obj);
  EXPECT_EQ(1, warning_count);
  EXPECT_EQ("The key was not found!", last_warning_message);
}

TEST(TestWarnings, GetterWarnings)
{
  int warning_count = 0;
  std::string last_warning_message = "";
  TinyJSON::parse_options options = {};
  options.callback_function = [&](TinyJSON::parse_options::message_type type, const TJCHAR* message)
  {
    if (type == TinyJSON::parse_options::message_type::warning)
    {
      warning_count++;
      if (message)
      {
        last_warning_message = message;
      }
    }
  };

  auto* json = TinyJSON::TJ::parse("{}", options);
  auto* obj = dynamic_cast<TinyJSON::TJValueObject*>(json);
  ASSERT_NE(nullptr, obj);

  // 1. try_get_string
  assert_warning_fired(obj, warning_count, last_warning_message, [](TinyJSON::TJValueObject* o)
  {
    o->try_get_string("missing");
  });

  // 2. get_boolean
  assert_warning_fired(obj, warning_count, last_warning_message, [](TinyJSON::TJValueObject* o)
  {
    o->get_boolean("missing");
  });

  // 3. get_string
  assert_warning_fired(obj, warning_count, last_warning_message, [](TinyJSON::TJValueObject* o)
  {
    o->get_string("missing");
  });

  // 4. get_number (long long)
  assert_warning_fired(obj, warning_count, last_warning_message, [](TinyJSON::TJValueObject* o)
  {
    o->get_number("missing");
  });

  // 5. get_numbers (std::vector<long long>)
  assert_warning_fired(obj, warning_count, last_warning_message, [](TinyJSON::TJValueObject* o)
  {
    o->get_numbers("missing");
  });

  // 6. get_float (long double)
  assert_warning_fired(obj, warning_count, last_warning_message, [](TinyJSON::TJValueObject* o)
  {
    o->get_float("missing");
  });

  // 7. get_floats (std::vector<long double>)
  assert_warning_fired(obj, warning_count, last_warning_message, [](TinyJSON::TJValueObject* o)
  {
    o->get_floats("missing");
  });

  // 8. get_number<int>
  assert_warning_fired(obj, warning_count, last_warning_message, [](TinyJSON::TJValueObject* o)
  {
    o->get_number<int>("missing");
  });

  // 9. get_numbers<int>
  assert_warning_fired(obj, warning_count, last_warning_message, [](TinyJSON::TJValueObject* o)
  {
    o->get_numbers<int>("missing");
  });

  // 10. get_float<double>
  assert_warning_fired(obj, warning_count, last_warning_message, [](TinyJSON::TJValueObject* o)
  {
    o->get_float<double>("missing");
  });

  // 11. get_floats<double>
  assert_warning_fired(obj, warning_count, last_warning_message, [](TinyJSON::TJValueObject* o)
  {
    o->get_floats<double>("missing");
  });

  // 12. get<bool>
  assert_warning_fired(obj, warning_count, last_warning_message, [](TinyJSON::TJValueObject* o)
  {
    o->get<bool>("missing");
  });

  // 13. get<const TJCHAR*>
  assert_warning_fired(obj, warning_count, last_warning_message, [](TinyJSON::TJValueObject* o)
  {
    o->get<const TJCHAR*>("missing");
  });

  // 14. get<int>
  assert_warning_fired(obj, warning_count, last_warning_message, [](TinyJSON::TJValueObject* o)
  {
    o->get<int>("missing");
  });

  // 15. get<double>
  assert_warning_fired(obj, warning_count, last_warning_message, [](TinyJSON::TJValueObject* o)
  {
    o->get<double>("missing");
  });

  // 16. get<std::vector<int>>
  assert_warning_fired(obj, warning_count, last_warning_message, [](TinyJSON::TJValueObject* o)
  {
    o->get<std::vector<int>>("missing");
  });

  // 17. get<std::vector<double>>
  assert_warning_fired(obj, warning_count, last_warning_message, [](TinyJSON::TJValueObject* o)
  {
    o->get<std::vector<double>>("missing");
  });

#if TJ_INCLUDE_STD_STRING == 1
  // 18. get<std::string>
  assert_warning_fired(obj, warning_count, last_warning_message, [](TinyJSON::TJValueObject* o)
  {
    o->get<std::string>("missing");
  });

  // 19. get<int> with std::string key
  assert_warning_fired(obj, warning_count, last_warning_message, [](TinyJSON::TJValueObject* o)
  {
    o->get<int>(std::string("missing"));
  });
#endif

  delete json;
}
