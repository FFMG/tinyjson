#include <iostream>
#include "../src/TinyJSON.h"
 
int main()
{
  auto json = TinyJSON::TinyJSON::parse(R"({
    "number" : 12,
    "string" : "Hello world"
  })");
  if(nullptr == json)
  {
    return -1;
  }

  if(json->is_object())
  {
    auto tjobject = dynamic_cast<TinyJSON::TJValueObject*>(json);
    std::cout << "Parsed an object with " << tjobject->get_number_of_items() << " item(s)\n";

    std::cout << "\nPrety dump:\n" << tjobject->dump() << "\n";
  }
  else
  {
    std::cout << "There was an issue parsing the object";
  }
  delete json;
  return 0;
}