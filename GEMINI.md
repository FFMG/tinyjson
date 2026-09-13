# TinyJSON Project Context

TinyJSON is a modern, high-performance C++ JSON library designed for performance-critical and resource-constrained applications. It is zero-dependency and supports RFC 8259.

## General

- My name is Florent
- I am a c++ developer

## Project Overview

- **Main Technologies:** C++ (C++11 for library, C++17 for tests), CMake, GoogleTest.
- **Architecture:** A single-header-ish design (though split into `TinyJSON.h` and `TinyJSON.cpp`) focusing on a fast, minimal-overhead DOM parser.
- **Key Features:** Zero-dependency, minimal memory overhead, supports UTF-8 BOM, customizable via macros (e.g., optional `std::string` or `std::vector` usage).

## Building and Running

### Specific to this machine

- This is a windows 11 machine
- I only have visual studio 2022

### Prerequisites
- CMake 3.28 or later.
- A C++ compiler supporting C++17 (for tests).
- Git (to initialize submodules).

### Commands
```bash
# Initialize submodules (GoogleTest)
git submodule update --init --recursive

# Configure and build
mkdir build
cd build
cmake ..
cmake --build .

# Run tests
ctest
```

### Linux/GCC Manual Build (Minimal)
```bash
g++ -std=c++11 -Wall -Wextra -Werror -O3 src/TinyJSON.cpp -o tinyjson_lib
```

## Development Conventions

- At the top of each files, add the following

// Licensed to Florent Guelfucci under one or more agreements.
// Florent Guelfucci licenses this file to you under the MIT license.
// See the LICENSE file in the project root for more information.

- If the above comment is not present, please flag it and ask me if I want to add it.


- Use 2 spaces, always, never tabs
- **Performance First:** Performance is the primary goal. "Pretty" code is secondary to execution speed and memory efficiency.
- **Coding Style:**
  - **Namespaces/Classes/Structs:** `CamelCase`.
  - **Functions/Variables:** `snake_case`.
  - **Enums:** `CamelCase` for the type, `lower_case` for values.
  - **Constants:** Use `static constexpr` (e.g., `TJ_MAX_DIGITS`) instead of macros where possible for thread safety.
  - **Macros:** Used for code repetition (e.g., `TJ_CASE_DIGIT`), named in `UPPERCASE` with `TJ_` prefix.
- **Class Structure:** Organized by visibility: `public`, then `protected`, then `private`. Within each section, variables precede methods.
- **Testing:** New features or bug fixes must include a full set of GoogleTest-based tests in the `tests/` directory.
- Never commit to git unless I ask you
- Unless expicitely asked, make sure the code supports c++11 or later, use MACRO if performance would improve ... but c++ must still be supported.

## Key Files
- `src/TinyJSON.h`: Main header containing the API and configuration macros.
- `src/TinyJSON.cpp`: Implementation of the parser and writer.
- `tests/`: Extensive test suite using GoogleTest.
- `example/`: Usage examples demonstrating various features like basic parsing, options, and literals.
- `specifications.md`: Detailed information on supported JSON standards.
