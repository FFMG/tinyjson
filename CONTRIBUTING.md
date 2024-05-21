# Contributing to TinyJSON

Please remember that the number 1 goal of TinyJSON is performance, while pretty code is great, perforamance will will over most things.

## Testing

You code changes must include full set of tests.

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