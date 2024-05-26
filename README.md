# TinyJSON

TinyJSON is a fast and small C++ JSON parser

## Simple example

In versy simple terms 

```cpp
  auto json = TinyJSON::TinyJSON::Parse( "{ \"Hello\" : \"World\" }" );
  auto value = json->try_get_string("Hello"); //  "World"
  auto no_value = json->try_get_string("Life"); //  null
```

## Getting Started

All you need is to include the .h and .cpp file to your project and you are ready to go.

### Example code

## Building and testing the project

- Clone this repository
- init the submodules
  `git submodule --init --recursive`

## TODO

[] Make sure that all the code follow our own coding standards.
[] Update comments
[] Finish documentation how to use the code.
[] Run/Create some benchmark tests, make sure they are up to date.
[] Arrays
[] Objects
[] Values
[] More tests!
[] Add version number
[x] Copyright
[] Add 2 kinds of error handling, either throw or return an error structure.
[] Automated build
[] While we do not care about size, (on paper), it would be nice to implement limits in the code.
   [] depth
   [] string
   [] numbers
[] Automated build