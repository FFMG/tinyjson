// Licensed to Florent Guelfucci under one or more agreements.
// Florent Guelfucci licenses this file to you under the MIT license.
// See the LICENSE file in the project root for more information.
#include <iostream>

#ifndef TJ_INCLUDE_STD_STRING
#define TJ_INCLUDE_STD_STRING 1
#endif // !TJ_INCLUDE_STD_STRING

#include "../src/TinyJSON.h"

using namespace TinyJSON; 
int main()
{
  auto json = R"({
    "number" : 12,
    "string" : "Hello world"
  })";

  if (!TJ::is_valid(json))
  {
    // this should have been valid!
    return -1;
  }

  auto tjjson = R"({
    "number" : 12,
    "string" : "Hello world"
  })"_tj;

  if(nullptr == tjjson)
  {
    return -1;
  }

  if(tjjson->is_object())
  {
    auto tjobject = dynamic_cast<TJValueObject*>(tjjson);
    std::cout << "Parsed an object with " << tjobject->get_number_of_items() << " item(s)\n";

    std::cout << "\n====\nPretty dump:\n" << tjobject->dump() << "\n====\n";
  }
  else
  {
    std::cout << "There was an issue parsing the object";
    delete tjjson;
    return -1;
  }
  delete tjjson;

  // output a pretty JSON
  std::cout << "\n====\nPretty JSON Array text.\n" << "[12,13,14]"_tj_indent << "\n====\n";

  // output a non-indented JSON
  std::cout << "\n====\nNot indented JSON Array text.\n" << "[  12,   13,  14]"_tj_minify << "\n====\n";

  return 0;
}