// Licensed to Florent Guelfucci under one or more agreements.
// Florent Guelfucci licenses this file to you under the MIT license.
// See the LICENSE file in the project root for more information.
#include <iostream>
#include "../src/TinyJSON.h"

bool object_is_valid()
{
  std::cout << "Object IsValid\n";
  auto json = R"({
    "number" : 12,
    "string" : "Hello world"
  })";

  if (!TinyJSON::TJ::is_valid(json))
  {
    // this should have been valid!
    std::cout << "Bad!!!\n\n";
    return false;
  }
  std::cout << "Good\n\n";
  return true;
}

bool object_parse()
{
  std::cout << "Object Parse\n";
  auto json = R"({
    "number" : 12,
    "string" : "Hello world"
  })";

  auto tjjson = TinyJSON::TJ::parse(json);
  if (nullptr == tjjson)
  {
    std::cout << "Bad!!!\n\n";
    return false;
  }

  if (tjjson->is_object())
  {
    auto tjobject = dynamic_cast<TinyJSON::TJValueObject*>(tjjson);
    std::cout << "Parsed an object with " << tjobject->get_number_of_items() << " item(s)\n";

    std::cout << "\nPretty dump:\n" << tjobject->dump() << "\n";
  }
  else
  {
    std::cout << "There was an issue parsing the object\n";
    delete tjjson;
    std::cout << "Bad!!!\n\n";
    return false;
  }
  delete tjjson;
  std::cout << "Good\n\n";
  return true;
}

bool object_find_values()
{
  std::cout << "Object Find Value\n";
  auto json = R"({
    "number" : 12,
    "string" : "Outer Hello world",
    "object" : {
      "number" : 13,
      "string" : "Inner Hello world",
      "object" : {
        "number" : 14,
        "string" : "Inner Hello world"
      }
    }
  })";

  if (!TinyJSON::TJ::is_valid(json))
  {
    // this should have been valid!
    std::cout << "Bad!!!\n\n";
    return false;
  }

  auto tjjson = TinyJSON::TJ::parse(json);
  if (nullptr == tjjson)
  {
    std::cout << "Bad!!!\n\n";
    return false;
  }

  if (!tjjson->is_object())
  {
    std::cout << "Bad!!!\n\n";
    return false;
  }

  auto tjobject = dynamic_cast<TinyJSON::TJValueObject*>(tjjson);
  std::cout << "Parsed an object with " << tjobject->get_number_of_items() << " item(s)\n";

  auto o1 = dynamic_cast<const TinyJSON::TJValueObject*>(tjobject->try_get_value("object"));
  auto o2 = dynamic_cast<const TinyJSON::TJValueObject*>(o1->try_get_value("object"));
  auto o3 = o2->try_get_value("string");
  std::cout << "\nPretty dump:\n" << o3->dump() << "\n";
  std::cout << "Inner value: [object][object][string]" << o3->dump_string() << "\n";

  std::cout << "Good\n\n";
  return true;
}

int main()
{
  if (!object_is_valid())
  {
    return -1;
  }

  if (!object_parse())
  {
    return -1;
  }
  if (!object_find_values())
  {
    return -1;
  }
  return 0;
}