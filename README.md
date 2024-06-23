# TinyJSON

TinyJSON is a fast and small C++ JSON parser

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

- MAJOR version when you make incompatible API changes
- MINOR version when you add functionality in a backward compatible manner
- PATCH version when you make backward compatible bug fixes

The version is set in the `TinyJSON.h` file.

```cpp
static const short TJ_VERSION_MAJOR = 0;
static const short TJ_VERSION_MINOR = 0;
static const short TJ_VERSION_PATCH = 1;
static const char TJ_VERSION_STRING[] = "0.0.1";
```

## Options

### Parse Options

- Depth: (`max_depth:64`) You can set how deep you want to allow the parsing to go.
- Throw: (`throw_exception:false`) If you want to throw exceptions or simply return null.
- Specification: (`specification:parse_options::rfc8259`) What specs will the parser be following/enforcing.
  - rfc4627
  - rfc7159
  - rfc8259

For example ...

```cpp
TinyJSON::parse_options options = {};
options.throw_exception = true;
options.max_depth = 10;

try
{
  auto blah = TinyJSON::TinyJSON::parse("[0123]", options)
  ...
}
catch(TinyJSON::TJParseException ex)
{
    ex.what(); // Numbers cannot have leading zeros
}
```

### Write Options

- Throw: (`throw_exception:false`) If you want to throw exceptions or simply return false.
- write_formating: (`write_formating::indented`) The formating we want to write the file with.
  - none
  - indented
- Byte order mark: (`byte_order_mark:none`)
  - none
  - uft8

For example ...

```cpp
TinyJSON::write_options options = {};
options.throw_exception = true;
options.write_formating = TinyJSON::formating::none;

try
{
  ...
  // get JSON somewhere or create it.
  ...
  if( TinyJSON::TinyJSON::write_file("file.json", *json, options))
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
    auto blah = TinyJSON::TinyJSON::parse("[0123]", options)
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
  options.write_formating = TinyJSON::formating::none;

  try
  {
    auto json = TinyJSON::TinyJSON::parse( "{ \"Hello\" : \"World\" }" );
    TinyJSON::TinyJSON::write_file("file.json", *json, options)
    ...
  }
  catch(TinyJSON::TJWriteException ex)
  {
     ex.what(); // some write error
  }
```

### Check if JSON is valid

You can simply call the `TinyJSON::TinyJSON::is_valid(...)` method

```cpp
    if(TinyJSON::TinyJSON::parse("[123,456]"))
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
  auto json = TinyJSON::TinyJSON::parse_file( "myfile.json" );
  ...
  delete json;
```

This will then return an object that you can inspect as per normal.

### Read a JSON string

To read a JSON string you simply need to call the static method `parse`

```cpp
  auto json = TinyJSON::TinyJSON::parse( "{ \"Hello\" : \"World\" }" );
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

### Write a JSON string

To write a JSON string you simply need to call the method `write_file` on the JSON object returned.

```cpp
  auto json = TinyJSON::TinyJSON::parse( "{ \"Hello\" : \"World\" }" );
  if( TinyJSON::TinyJSON::write_file("file.json", *json))
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
  auto json = TinyJSON::TinyJSON::parse( R"("This is a test \" with a quote")" );
  auto actual_string = json->dump_string(); 
  /*
    This is a test " with a quote
   */
  
  delete json;
```

The formating types are

```cpp
  TinyJSON::formating::indented
  TinyJSON::formating::none
```

### Objects

Each objects are read into `TJValue*` classes of type `TJValueObject`.

```cpp
  auto json = TinyJSON::TinyJSON::parse( "{ \"Hello\" : \"World\" }" );
  auto value = json->try_get_string("Hello"); //  "World"
  auto no_value = json->try_get_string("Life"); //  null

  delete json;
```

### Arrays

Each arrays are read into `TJValue*` classes of type `TJValueArray`.
Then each items in the array are also `TJValue*` of type string, number and so on.

```cpp
  auto json = TinyJSON::TinyJSON::parse( "[ 12, 14, 16 ]" );
  auto array_of_values = dynamic_cast<TinyJSON::TJValueArray*>(json);
  auto number_of_items = array_of_values->get_number_of_items();  // 3

  auto value1 = array_of_values->at(0);  //14
  auto value2 = array_of_values->at(2);  //16
  auto value3 = array_of_values->at(5);  // null

  delete json;
```

#### Data type

Each `TJValue*` item can be of different type

Assuming the array below with multiple items, you can query the type of each `TJValue*` and do something accordingly.

```cpp
  auto json = TinyJSON::TinyJSON::parse( "[ ... ]" );
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

This can be helpful is you are looking for a certain data type.

## Getting Started

All you need is to include the .h and .cpp file to your project and you are ready to go.

### Example code

## Building and testing the project

- Clone this repository
- init the submodules
  `git submodule --init --recursive`

## Data Range

### Numbers

The whole number ranges are +9223372036854775807 and -9223372036854775806

## TODO

- [] Make sure that all the code follow our own coding standards.
- [] Update comments
- [] Finish documentation how to use the code.
- [] Run/Create some benchmark tests, make sure they are up to date.
- [x] Arrays
- [x] Objects
- [x] Values
- [] More tests!
- [x] Add version number (started at version 0.0.1)
- [x] Copyright
- [x] Add 2 kinds of error handling, either throw or return an error structure.
- [] Automated build
- [] While we do not care about size, (on paper), it would be nice to implement limits in the code.
  - [x] depth
  - [] string
  - [] numbers
- [x] Automated build
- [x] Run on linux/gcc/g++ or something other than visual studio.
     `g++ -std=c++11 -Wall -Wextra -Werror -O3 src/tinyJSON.cpp -o a.exe`
- [] We need to add copy and move constructors to `TJValue` and the derived classes.
