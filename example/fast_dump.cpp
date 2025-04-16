// Licensed to Florent Guelfucci under one or more agreements.
// Florent Guelfucci licenses this file to you under the MIT license.
// See the LICENSE file in the project root for more information.
#include <iostream>
#include <string>
#include <random>
#include <map>
#include <regex>
#include <chrono>
#include <cstring>

#include "../src/TinyJSON.h"

double calculateSizeInMegabytes(const char* str) {
  // Get the length of the string in bytes
  size_t lengthInBytes = std::strlen(str);

  // Convert bytes to megabytes
  double lengthInMegabytes = static_cast<double>(lengthInBytes) / (1024 * 1024);

  return lengthInMegabytes;
}

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

bool object_dump(const int numbers_to_add)
{
  // Get the start time
  auto start1 = std::chrono::high_resolution_clock::now();

  // create an empty object and add some items to it.
  auto object = new TinyJSON::TJValueObject();

  // then add a lot of items
  std::map<std::string, int> data;
  for (auto i = 0; i < numbers_to_add; ++i)
  {
    auto key = generateRandomString(20);  //  long string to prevent colisions.
    auto value = generateRandomNumber(0, 5000);
    object->set_number(key.c_str(), value);
    data[key] = value;
  }

  std::cout << "Added: " << data.size() << " items." << "\n";

  auto end1 = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> duration1 = end1 - start1;
  std::cout << "Insert: " << duration1.count() << " seconds" << "\n";

  auto start2 = std::chrono::high_resolution_clock::now();
  std::cout << "JSON Size: " << calculateSizeInMegabytes(object->dump()) << " mb\n";

  auto end2 = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> duration2 = end2 - start2;
  std::cout << "Dump: " << duration2.count() << " seconds\n";

  delete object;
  return true;
}

int main()
{
  if (!object_dump(1000))
  {
    return -1;
  }
  if (!object_dump(10000))
  {
    return -1;
  }
  if (!object_dump(100000))
  {
    return -1;
  }
  return 0;
}