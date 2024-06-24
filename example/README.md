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
