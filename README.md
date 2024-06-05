# TinyJSON

TinyJSON is a fast and small C++ JSON parser

## Simple examples

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
  auto number_of_items = array_of_values->number_of_items();  // 3

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
- [] Add version number
- [x] Copyright
- [] Add 2 kinds of error handling, either throw or return an error structure.
- [] Automated build
- [] While we do not care about size, (on paper), it would be nice to implement limits in the code.
  - [] depth
  - [] string
  - [] numbers
- [] Automated build
- [] Run on linux/gcc/g++ or something other than visual studio.