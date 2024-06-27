// Licensed to Florent Guelfucci under one or more agreements.
// Florent Guelfucci licenses this file to you under the MIT license.
// See the LICENSE file in the project root for more information.
#include <iostream>
#include <string>
#include <random>
#include <map>
#include <regex>
#include <chrono>

#include "../src/TinyJSON.h"

std::string generateRandomString(size_t length) {
  const std::string characters = "!@#$%^&*()abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
  std::random_device rd;
  std::mt19937 generator(rd());
  std::uniform_int_distribution<> distribution(0, characters.size() - 1);

  std::string randomString;
  for (size_t i = 0; i < length; ++i) {
    randomString += characters[distribution(generator)];
  }
  return randomString;
}

int generateRandomNumber(int min, int max) {
  std::random_device rd; // Seed for the random number generator
  std::mt19937 generator(rd()); // Mersenne Twister random number generator
  std::uniform_int_distribution<> distribution(min, max); // Uniform distribution within [min, max]

  return distribution(generator);
}

bool object_shallow()
{
  // Get the start time
  auto start1 = std::chrono::high_resolution_clock::now();

  // create an empty object and add some items to it.
  auto object = new TinyJSON::TJValueObject();

  // then add a lot of items
  std::map<std::string, int> data;
  const int numbers_to_add = 100000;
  for (auto i = 0; i < numbers_to_add; ++i)
  {
    auto key = generateRandomString(10);  //  long string to prevent colisions.
    auto value = generateRandomNumber(0, 5000);
    object->set(key.c_str(), value);
    data.insert({ key, value });
  }

  auto end1 = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> duration1 = end1 - start1;
  std::cout << "Insert: " << duration1.count() << " seconds" << "\n";

  auto start2 = std::chrono::high_resolution_clock::now();

  // then search each and every item
  for (auto d : data)
  {
    auto key = d.first;
    auto value = d.second;
    auto tj = object->try_get_value(key.c_str());
    auto number = dynamic_cast<const TinyJSON::TJValueNumberInt*>(tj);
    if (number->get_number() != value)
    {
      std::cout << "  Error! Values mismatch\n";
      return false;
    }
  }

  auto end2 = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> duration2 = end2 - start2;
  std::cout << "Search: " << duration2.count() << " seconds << "\n";

  delete object;
  return true;
}

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
  if (!object_shallow())
  {
    return -1;
  }

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