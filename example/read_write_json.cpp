// Licensed to Florent Guelfucci under one or more agreements.
// Florent Guelfucci licenses this file to you under the MIT license.
// See the LICENSE file in the project root for more information.
#include "../src/TinyJSON.h"
#include <iostream>
#include <cassert>

using namespace TinyJSON;

int main()
{
  TJValueObject root;
  root.set("project", "TinyJSON");
  root.set("version", 1);
  root.set("fast", true);

  TJValueArray* arr = new TJValueArray();
  arr->add_number(100);
  arr->add_number(200);
  arr->add_number(300);
  root.set("data", arr);

  TJValueObject* nested = new TJValueObject();
  nested->set("sub1", 1.5);
  nested->set("sub2", "test string");
  root.set("metadata", nested);

  // Write to a file
  std::string filename = "test_io_json.json";
  bool success = TinyJSON::TJ::write_file(filename.c_str(), root);
  if (!success) {
    std::cerr << "Failed to write JSON file." << std::endl;
    return 1;
  }

  // Read back the file
  auto* parsed = TJ::parse_file(filename.c_str());
  if (!parsed) {
    std::cerr << "Failed to read JSON file." << std::endl;
    return 1;
  }

  // Compare the dumped output to verify data matches
  std::string original_dump = root.dump(formatting::minify);
  std::string parsed_dump = parsed->dump(formatting::minify);

  if (original_dump != parsed_dump) {
    std::cerr << "Data mismatch between original and read JSON!" << std::endl;
    std::cerr << "Original: " << original_dump << std::endl;
    std::cerr << "Parsed:   " << parsed_dump << std::endl;
    delete parsed;
    return 1;
  }

  std::cout << "Successfully wrote, read, and verified JSON file." << std::endl;
  delete parsed;
  return 0;
}
