# Contributing to TinyJSON

Please remember that the number 1 goal of TinyJSON is performance, while pretty code is great, perforamance will will over most things.

## Testing

Your code changes must include a full set of tests. TinyJSON uses [GoogleTest](https://github.com/google/googletest) for its test suite.

### How to build and run tests

To run the tests, you will need CMake installed.

1. **Initialize Submodules**:
   TinyJSON uses GoogleTest as a submodule.
   ```bash
   git submodule update --init --recursive
   ```

2. **Configure and Build**:
   ```bash
   mkdir build
   cd build
   cmake ..
   cmake --build .
   ```

3. **Run Tests**:
   ```bash
   ctest
   ```

On Windows with Visual Studio, you can also open the project folder directly in VS and use the Test Explorer.

## C++ coding style

We use bot CamelCase and snake_case

- Use CamelCase in a class, struct and namespace name
- Use snake_case for variable, function name 
- Use CamelCase for Enums

All classes must be broken down in public/protected/private, with the variables then the methods.

```cpp
class Blah
{
  public:
   // variable 

   // methods

  protected:
   // variable 

   // methods

  private:
   // variable 

   // methods
}
```

### Enums

Enums are lower case

```cpp
enum class formatting
{
  none,
  indented
};
```

### Macro or static constexpr?

In almost all cases choose to use `static constexpr`, this will get you thread safety.

You can use UPERCASE to reflect that this is a const.

```cpp
static constexpr short TJ_MAX_NUMBER_OF_DIGGITS = 19;
static constexpr short TJ_DEFAULT_STRING_READ_SIZE = 10;
```

Macros are used in cases where code is repeated.
But in such cases, the name should make it very clear what the use case might be.

```cpp
#define TJ_CASE_SIGN          case '-': \
                              case '+': 
```
