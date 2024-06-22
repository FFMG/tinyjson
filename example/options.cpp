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

    std::cout << "\nNo Indent dump:\n" << tjobject->dump(TinyJSON::formating::none) << "\n";
  }
  else
  {
    std::cout << "There was an issue parsing the object";
    delete json;
    return -1;
  }

  // cleanup
  delete json;  

  // throw an exception
  try
  {
    auto jsonex = TinyJSON::TinyJSON::parse(R"({"number" : 12.e00})", {.throw_exception = true  });
    std::cout << "There was an issue throwing an exceptions!";
    delete jsonex;  
    return -1;
  }
  catch(const std::exception& e)
  {
    std::cerr << "Caught Expected Exception!" << e.what() << '\n';
  }
  return 0;
}