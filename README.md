# TinyJSON

A modern C++ JSON library engineered for performance-critical applications. TinyJSON provides a robust and efficient solution for parsing JSON data, focusing on speed, memory efficiency, and ease of integration. Whether you're building a high-throughput server or a resource-constrained embedded application, TinyJSON delivers a no-compromise approach to JSON parsing.

* Zero-dependency: Easy to integrate into any C++ project.
* Minimal memory overhead: Ideal for resource-constrained environments.
* High-performance parsing: Benchmarked for speed against other leading parsers.
* Simple API: Intuitive and easy to use.

## Contents

* [Supported JSON](#supported-json)
* [Simple examples](#simple-examples)
* [Supported C++](#supported-cpp)
* [TODOs](#todo)
  
## Supported JSON

[RFC 8259](https://www.wikidata.org/wiki/Q47322594) (replaces [RFC 7159](https://www.wikidata.org/wiki/Q47470410))

See more in the [specifications document](./specifications.md).

One of the important change is that prior to RFC 8259 only `objects` and `arrays` could be at the root.

```json
{
  "meaning" : 42
}
```

or

```json
[
  42, 12, 3.14
]
```

But since [RFC 8259](https://www.wikidata.org/wiki/Q47322594) any of the other elements are valid, (`string`, `true`, `false`, `null`, `number`)

```json
true
```

## Data types

| JSON   | c++/TinyJSON                                      |
| ------ | ------------------------------------------------- |
| object | TJValueObject                                     |
| array  | TJValueArray                                      |
| string | TJValueString                                     |
| number | TJValueNumber (longl long, long double, exponent) |
| true   | TJValueBoolean (true)                             |
| false  | TJValueBoolean (false)                            |
| null   | TJValueNull (nullptr)                             |

## Simple examples

### Version Control

We follow Semantic Versioning 2.0.0, [semver.org](https://semver.org/)

* MAJOR version when you make incompatible API changes
* MINOR version when you add functionality in a backward compatible manner
* PATCH version when you make backward compatible bug fixes

The version is set in the `TinyJSON.h` file.

```cpp
static const short TJ_VERSION_MAJOR = 0;
static const short TJ_VERSION_MINOR = 0;
static const short TJ_VERSION_PATCH = 1;
static const char TJ_VERSION_STRING[] = "0.0.1";
```

## Options

### Parse Options

* Depth: (`max_depth:64`) You can set how deep you want to allow the parsing to go.
* Throw: (`throw_exception:false`) If you want to throw exceptions or simply return null.
* Specification: (`specification:parse_options::rfc8259`) What specs will the parser be following/enforcing.
  * rfc4627
  * rfc7159
  * rfc8259

For example ...

```cpp
TinyJSON::parse_options options = {};
options.throw_exception = true;
options.max_depth = 10;

try
{
  auto blah = TinyJSON::TJ::parse("[0123]", options)
  ...
}
catch(TinyJSON::TJParseException ex)
{
    ex.what(); // Numbers cannot have leading zeros
}
```

### Write Options

* Throw: (`throw_exception:false`) If you want to throw exceptions or simply return false.
* write_formating: (`write_formating::indented`) The formating we want to write the file with.
  * none
  * indented
* Byte order mark: (`byte_order_mark:none`)
  * none
  * uft8

For example ...

```cpp
TinyJSON::write_options options = {};
options.throw_exception = true;
options.write_formating = TinyJSON::formating::minify;

try
{
  ...
  // get JSON somewhere or create it.
  ...
  if( TinyJSON::TJ::write_file("file.json", *json, options))
  {
    // all good
  }
  ...
}
catch(TinyJSON::TJParseException ex)
{
    ex.what(); // what?
}
```

### Exceptions

#### Parsing Exception

The parsing exception is `TinyJSON::TJParseException` and can be made optional in the `TinyJSON::parse_options` flag.

```cpp
  TinyJSON::parse_options options = {};
  options.throw_exception = true;
  try
  {
    auto blah = TinyJSON::TJ::parse("[0123]", options)
    ...
  }
  catch(TinyJSON::TJParseException ex)
  {
     ex.what(); // Numbers cannot have leading zeros
  }
```

#### Write Exception

The parsing exception is `TinyJSON::TJParseException` and can be made optional in the `TinyJSON::parse_options` flag.

```cpp
  TinyJSON::write_options options = {};
  options.throw_exception = true;
  options.write_formating = TinyJSON::formating::minify;

  try
  {
    auto json = TinyJSON::TJ::parse( "{ \"Hello\" : \"World\" }" );
    TinyJSON::TJ::write_file("file.json", *json, options)
    ...
  }
  catch(TinyJSON::TJWriteException ex)
  {
     ex.what(); // some write error
  }
```

### Check if JSON is valid

You can simply call the `TinyJSON::TJ::is_valid(...)` method

```cpp
    if(TinyJSON::TJ::is_valid("[123,456]"))
    {
      // do this
    }
    else
    {
      // do that
    }
```

### Read a JSON file

To read a JSON file you simply need to call the static method `parse_file`, the extention does not have to be `.json`

```cpp
  auto json = TinyJSON::TJ::parse_file( "myfile.json" );
  ...
  delete json;
```

This will then return an object that you can inspect as per normal.

### Read a JSON string

To read a JSON string you simply need to call the static method `parse`

```cpp
  auto json = TinyJSON::TJ::parse( "{ \"Hello\" : \"World\" }" );
  ...
  delete json;
```

This will then return an object that you can inspect.

```cpp
  ...
  auto value = json->try_get_string("Hello"); //  "World"
  auto no_value = json->try_get_string("Life"); //  null
  ...
```

You can also use literals

```cpp
using namespace TinyJSON;
...
auto json = "[12,13,14]"_tj;

// use it.

...
delete json;
...
```

Or just use the string directly

You can convert the json to 'pretty' indented json

```cpp
#define TJ_INCLUDE_STD_STRING 1
#include "TinyJSON.h"
using namespace TinyJSON;
...
// output a pretty JSON
std::cout << "[12,13,14]"_tj_indent;
...
```

Or `minify` code.

```cpp
#define TJ_INCLUDE_STD_STRING 1
#include "TinyJSON.h"
using namespace TinyJSON;
...
// output a pretty JSON
std::cout << "[12 , 13 , 14]"_tj_minify;
...
```

### Search an object by key

There are 2 ways to find a key, case sensitive or insensitive, (the default is case sensitive).

You can load the json either by parsing or creating it manually.

```cpp
  auto object = new TinyJSON::TJValueObject();
  object->set_string("a", "Hello");
  object->set_string("b", "World");
  object->set_string("c", "Bye");
  ...
  // search for something
  auto case_sensitive = object->try_get_value("Hello");
  auto case_insensitive = object->try_get_value("HelLO", false);  // all good as we don't care about case.
  auto case_insensitive_not_found = object->try_get_value("HelLO");  // = null as the search was case sensitive
  ...
  delete object;
```

### Check if key exists

You can use `has_key` to check if an object contains a specific key.

```cpp
  auto json = TinyJSON::TJ::parse(R"({"a": 42})");
  auto jobject = dynamic_cast<TinyJSON::TJValueObject*>(json);

  if (jobject->has_key("a")) 
  {
    // do something
  }

  // Case insensitive check
  bool exists = jobject->has_key("A", false); // returns true
```

### How to create a JSON value?

```cpp
  auto object = new TinyJSON::TJValueObject();
  object->set_string("a", "Hello");
  object->set_string("b", "World");
  object->set_string("c", "Bye");
  object->set_boolean("d", true);
  object->set_float("e", 42.15);

  auto dump = object->dump();

  /*
  {
    "a": "Hello",
    "b": "World",
    "c": "Bye",
    "d": true,
    "e": 42.15
  }
  */
```

You can also remove a value

### Write a JSON string

```cpp
  auto object = new TinyJSON::TJValueObject();
  object->set_string("a", "Hello");
  object->set_string("b", "World");
  object->set_string("c", "Bye");
  object->set_boolean("d", true);

  object->pop("b");

  auto dump = object->dump();

  /*
  {
    "a": "Hello",
    "b": "World",
    "c": "Bye",
    "d": true
  }
  */
```

To write a JSON string you simply need to call the method `write_file` on the JSON object returned.

```cpp
  auto json = TinyJSON::TJ::parse( "{ \"Hello\" : \"World\" }" );
  if( TinyJSON::TJ::write_file("file.json", *json))
  {
    // all good
  }
  else
  {
    // something broke
  }
  
  delete json;
```

### UTF-8 DOM

Regardless if you are parsing a file or parsing a string, TinyJSON will parse the file for UTF-8 DOM if there is one.

### Using a JSON string

While you can `dump` a string to (re)write a json string you might want to use it directly in your code.

```cpp
  auto json = TinyJSON::TJ::parse( R"("This is a test \" with a quote")" );
  auto actual_string = json->dump_string(); 
  /*
    This is a test " with a quote
   */
  
  delete json;
```

The formating types are

```cpp
  TinyJSON::formating::indented
  TinyJSON::formating::minify
```

### Objects

Each objects are read into `TJValue*` classes of type `TJValueObject`.

```cpp
  auto json = TinyJSON::TJ::parse( "{ \"Hello\" : \"World\" }" );
  auto value = json->try_get_string("Hello"); //  "World"
  auto no_value = json->try_get_string("Life"); //  null

  delete json;
```

or

```cpp
  auto json = TinyJSON::TJ::parse( "{ \"Hello\" : \"World\" }" );
  auto value = json->get_string("Hello"); //  "World"
  auto no_value = json->get_string("Life", true, false); //  null

  delete json;
```


#### Adding values to an object

You can dynamically create a new object and then add a named values to it.

```cpp
  auto object = new TinyJSON::TJValueObject();
  object->set_number("a", 42);
  object->set_float("b", 0.005);
  object->set_null("c");

  ...

  delete json;
```

### Arrays

Each arrays are read into `TJValue*` classes of type `TJValueArray`.
Then each items in the array are also `TJValue*` of type string, number and so on.

```cpp
  auto json = TinyJSON::TJ::parse( "[ 12, 14, 16 ]" );
  auto array_of_values = dynamic_cast<TinyJSON::TJValueArray*>(json);
  auto number_of_items = array_of_values->get_number_of_items();  // 3

  auto value1 = array_of_values->at(0);  //14
  auto value2 = array_of_values->at(2);  //16
  auto value3 = array_of_values->at(5);  // null

  delete json;
```

#### Adding values to an arrays

You can dynamically create a new array and then add string, numbers and so on.

```cpp
  auto json = new TinyJSON::TJValueArray();
  json->add_boolean(true);
  json->add_boolean(false);
  json->add_number(42);

  ...

  delete json;
```

### Data type

Each `TJValue*` item can be of different type

Assuming the array below with multiple items, you can query the type of each `TJValue*` and do something accordingly.

```cpp
  auto json = TinyJSON::TJ::parse( "[ ... ]" );
  auto array_of_values = dynamic_cast<TinyJSON::TJValueArray*>(json);

  auto value = array_of_values->at(0);

  if(value->is_object()) { /**/}
  if(value->is_array()) { /**/}
  if(value->is_string()) { /**/}
  if(value->is_number()) { /**/}
  if(value->is_true()) { /**/}
  if(value->is_false()) { /**/}
  if(value->is_null()) { /**/}

  delete json;
```

### Iterate values

```cpp
  auto json = TinyJSON::TJ::parse(R"(
    1,
    2,
    3
  ])");
  for (auto& a : *json)
  {
    // do stuff
  }
  delete json;
  ...
```

NB: You can technically everything... 

```cpp
  auto json = TinyJSON::TJ::parse("2.5");
  for (auto& a : *json)
  {
    // do stuff
  }
  ...
  delete json;
```

### Get values

#### Generic Get values

The generic `get<...>( ... )` method provides a convenient way to retrieve values of various types directly.

For `TJValueObject` (key-based):

```cpp
auto json = TinyJSON::TJ::parse(R"({"a": 42, "b": "hello", "c": true})");
auto obj = dynamic_cast<TJValueObject*>(json);

int a = obj->get<int>("a");              // 42
std::string b = obj->get<std::string>("b"); // "hello"
bool c = obj->get<bool>("c");            // true
```

#### Get values with default

The `get_or<...>(key, default_value)` method allows you to specify a default value if the key is not found or the type is incorrect.

```cpp
auto json = TinyJSON::TJ::parse(R"({"a": 42})");
auto obj = dynamic_cast<TJValueObject*>(json);

int a = obj->get_or<int>("a", 0);        // 42
int b = obj->get_or<int>("b", 100);      // 100 (key not found)
std::string c = obj->get_or<std::string>("c", "default"); // "default"
```

The `get` method on objects also supports two optional parameters:
- `case_sensitive`: Defaults to `true`.
- `throw_if_not_found`: Defaults to `false`.

For any `TJValue*` (direct value):

```cpp
auto val = obj->try_get_value("a");
int a = val->get<int>(); // 42
```

The generic `get` also supports `std::vector` for arrays of numbers or floats:

```cpp
auto json = TinyJSON::TJ::parse(R"({"ints": [1, 2, 3], "floats": [1.1, 2.2]})");
auto obj = dynamic_cast<TJValueObject*>(json);

auto ints = obj->get<std::vector<int>>("ints");
auto doubles = obj->get<std::vector<double>>("floats");
```

```cpp
auto json = TinyJSON::TJ::parse(R"(
  {
    "a": true,
    "b": 123,
    "c": 0,
    "d": 42.2
  }
  )"
);

auto valuea = jobject->try_get_value("a");
if( valuea->get_boolean())
{
  // value a is true
}

auto valuea = jobject->try_get_value("b");
if( valueb->get_boolean())
{
  // value b is not a boolean but it is non zero
}

```

#### Strict Get values

You can get a value from any TJValue*, (as long as the value can actually be converted)

- get_number()
- get_float()
- get_string()
- get_boolean()
- get_numbers()
- get_floats()


```cpp
auto json = TinyJSON::TJ::parse(R"(
  {
    "a": true,
    "b": 123,
    "c": 0,
    "d": 42.2
  }
  )"
);

auto valuea = jobject->try_get_value("a");
if( valuea->get_boolean())
{
  // value a is true
}

auto valuea = jobject->try_get_value("b");
if( valueb->get_boolean(true))  //  THROW! "b" is not a boolean
{
  // value b is not a boolean but it is non zero
}

```

This can be helpful is you are looking for a certain data type.

### Set values

#### Generic Set values

The generic `set<...>( ... )` method provides a convenient way to set values of various types directly.

For `TJValueObject` (key-based):

```cpp
TinyJSON::TJValueObject obj;

obj.set("a", 42);                  // set int
obj.set("b", "hello");             // set string
obj.set("c", true);                // set boolean
obj.set("d", 3.14);                // set double
```

The generic `set` also supports `std::string` and `std::vector` for arrays of numbers or floats:

```cpp
TinyJSON::TJValueObject obj;

std::string val = "hello";
obj.set("b", val);                 // set std::string

std::vector<int> ints = {1, 2, 3};
obj.set("ints", ints);             // set std::vector<int>
```

#### Strict Set values

You can also use specific set methods:

- set_number(key, value)
- set_float(key, value)
- set_string(key, value)
- set_boolean(key, value)
- set_null(key)

```cpp
TinyJSON::TJValueObject obj;

obj.set_number("a", 42);
obj.set_string("b", "hello");
obj.set_boolean("c", true);
obj.set_null("d");
```

## Getting Started

All you need is to include the .h and .cpp file to your project and you are ready to go.

### Example code

## Building and testing the project

* Clone this repository
* init the submodules
  `git submodule --init --recursive`

## Data Range

### Numbers

The whole number ranges are +9223372036854775807 and -9223372036854775806

## TODO

* [x] Make sure that all the code follow our own coding standards.
* [] Update comments
* [x] Finish documentation how to use the code.
* [] Run/Create some benchmark tests, make sure they are up to date.
* [x] Arrays
* [x] Objects
* [x] Values
* [x] More tests!
* [x] Add version number (started at version 0.0.1)
* [x] Copyright
* [x] Add 2 kinds of error handling, either throw or return an error structure.
* [] While we do not care about size, (on paper), it would be nice to implement limits in the code.
  * [x] depth
  * [] string
  * [] numbers
* [x] Automated build
* [x] Run on linux/gcc/g++ or something other than visual studio.
     `g++ -std=c++11 -Wall -Wextra -Werror -O3 src/tinyJSON.cpp -o a.exe`
* [] We need to add copy and move constructors to `TJValue` and the derived classes.
* [] Add Macro(s) explaination like `TJ_INCLUDE_STD_STRING` for example.
* [] Add support for JSON5
     <https://json5.org>
  📄 <https://github.com/json5/json5>

## Supported CPP

The supported versions of c++ are c++ 11 and later.
