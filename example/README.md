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
auto jsonex = TinyJSON::TinyJSON::parse(R"({"number" : 12.e00})", {.throw_exception = true  });
...
```