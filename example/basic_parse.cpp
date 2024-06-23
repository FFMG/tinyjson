// Licensed to Florent Guelfucci under one or more agreements.
// Florent Guelfucci licenses this file to you under the MIT license.
// See the LICENSE file in the project root for more information.
#include <iostream>
#include "../src/TinyJSON.h"
 
int main()
{
  auto json = R"({
    "number" : 12,
    "string" : "Hello world"
  })";

  if (!TinyJSON::TinyJSON::is_valid(json))
  {
    // this should have been valid!
    return -1;
  }

  auto tjjson = TinyJSON::TinyJSON::parse(json);
  if(nullptr == tjjson)
  {
    return -1;
  }

  if(tjjson->is_object())
  {
    auto tjobject = dynamic_cast<TinyJSON::TJValueObject*>(tjjson);
    std::cout << "Parsed an object with " << tjobject->get_number_of_items() << " item(s)\n";

    std::cout << "\nPretty dump:\n" << tjobject->dump() << "\n";
  }
  else
  {
    std::cout << "There was an issue parsing the object";
    delete tjjson;
    return -1;
  }
  delete tjjson;
  return 0;
}