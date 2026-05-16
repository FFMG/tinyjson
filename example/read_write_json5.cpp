// Licensed to Florent Guelfucci under one or more agreements.
// Florent Guelfucci licenses this file to you under the MIT license.
// See the LICENSE file in the project root for more information.
#include "../src/TinyJSON.h"
#include <iostream>
#include <cassert>

using namespace TinyJSON;

int main()
{
  const char* json5_content = R"({
    // This is a project name
    project: 'TinyJSON',
    version: 0x1, // Hex number
    fast: true,
    data: [
      /* multi line
         comment */
      100, 
      200, 
      300, // trailing comma
    ],
    metadata: {
      sub1: +1.5, // Explicit plus
      sub2: "test string",
    }
  })";

  parse_options options;
  options.specification = parse_options::json5_1_0_0;
  
  auto* root = TJ::parse(json5_content, options);
  if (!root) {
    std::cerr << "Failed to parse initial JSON5 content." << std::endl;
    return 1;
  }

  // Write to a file
  std::string filename = "test_io_json5.json";
  write_options w_options;
  w_options.write_formatting = formatting::indented;
  bool success = root->write_file(filename.c_str(), w_options);
  if (!success) {
    std::cerr << "Failed to write JSON5 file." << std::endl;
    delete root;
    return 1;
  }

  // Read back the file
  // Since write_file outputs standard JSON (with comments preserved), 
  // we read it back using JSON5 specification to be safe.
  auto* parsed = TJ::parse_file(filename.c_str(), options);
  if (!parsed) {
    std::cerr << "Failed to read JSON5 file." << std::endl;
    delete root;
    return 1;
  }

  // Compare the dumped output to verify data matches
  std::string original_dump = root->dump(formatting::minify);
  std::string parsed_dump = parsed->dump(formatting::minify);

  if (original_dump != parsed_dump) {
    std::cerr << "Data mismatch between original and read JSON5!" << std::endl;
    std::cerr << "Original: " << original_dump << std::endl;
    std::cerr << "Parsed:   " << parsed_dump << std::endl;
    delete root;
    delete parsed;
    return 1;
  }

  std::cout << "Successfully wrote, read, and verified JSON5 file." << std::endl;
  delete root;
  delete parsed;
  return 0;
}
