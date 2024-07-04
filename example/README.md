# Examples

## Basic Parse

This file does a very simple object parse.

```json
{
  "number" : 12,
  "string" : "Hello world"
}
```

## Options (c++20)

This shows how to pass options while parsing.

```cpp
...
  auto jsonex = TinyJSON::TJ::parse(R"({"number" : 12.e00})", {.throw_exception = true  });
...
```

## Options11 (c++11)

This test that the build works with c++ 11

```cpp
...
  TinyJSON::parse_options options = {};
  options.throw_exception = true;
  auto jsonex = TinyJSON::TJ::parse(R"({"number" : 12.e00})", options);
...
```

## User literals

User Literals are like shortcuts to the code, the user_literals code shows how to use it.

```cpp
  std::cout << "[  12,   13,  14]"_tj_minify;

  // or indent

  std::cout << "[12,13,14]"_tj_indent;
```

## Fast dump

This example shows us how to create and dump large data, this is mainly to test that there is not data loss.
