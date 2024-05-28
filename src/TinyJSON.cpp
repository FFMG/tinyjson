// Licensed to Florent Guelfucci under one or more agreements.
// Florent Guelfucci licenses this file to you under the MIT license.
// See the LICENSE file in the project root for more information.
#include "TinyJSON.h"

#include <algorithm>
#include <cmath> 

#define TJ_MAX_NUMBER_OF_DIGGITS 19

#define TJ_CASE_SIGN          case '-': \
                              case '+': 

#define TJ_CASE_DIGIT         case '0': \
                              case '1': \
                              case '2': \
                              case '3': \
                              case '4': \
                              case '5': \
                              case '6': \
                              case '7': \
                              case '8': \
                              case '9': 

#define TJ_CASE_SPACE         case ' ':  \
                              case '\t': \
                              case '\n': \
                              case '\r':

#define TJ_CASE_START_STRING  case '"':

#define TJ_CASE_COMMA         case ',':

#define TJ_CASE_COLON         case ':':

#define TJ_CASE_BEGIN_OBJECT  case '{':

#define TJ_CASE_END_OBJECT    case '}':

namespace TinyJSON
{
  ///////////////////////////////////////
  /// TinyJSON
  TinyJSON::TinyJSON()
  {
  }

  TJValue* TinyJSON::parse(const char* src)
  {
    const char* p = src;

    return start(p);
  }

  TJValue* TinyJSON::start(const char*& p)
  {
    // at the root we can only have an array or an object.
    // not both and not more than one
    TJValue* object_found = nullptr;
    while (*p != '\0') 
    {
      char c = *p;
      switch (c)
      {
      TJ_CASE_SPACE
        p++;
        break;

      TJ_CASE_BEGIN_OBJECT
      {
        auto tjvalue_object = TJMember::try_continue_read_object(++p);
        if (nullptr == tjvalue_object)
        {
          // Error:  something went wrong, the error was logged.
          return nullptr;
        }
        object_found = tjvalue_object;
      }
      break;

      default:
        //  no idea what this is.
        // clear our memory and return the error.
        delete object_found;
        return nullptr;
      }
    }

    // return if we found anything.
    return object_found;
  }

  ///////////////////////////////////////
  /// TJMember
  TJMember::TJMember(const char* string, const TJValue* value):
    _string(nullptr),
    _value(nullptr)
  {
    _string = new char[std::strlen(string)+1];
    std::strcpy(_string, string);
  }

  TJMember::TJMember(char* string, TJValue* value) :
    _string(string),
    _value(value)
  {
  }

  TJMember::~TJMember()
  {
    free_string();
    free_value();
  }

  const char* TJMember::to_string() const
  {
    if (nullptr == _value)
    {
      return false;
    }
    return _value->to_string();
  }

  void TJMember::free_string()
  {
    if (nullptr != _string)
    {
      delete[] _string;
    }
    _string = nullptr;
  }

  void TJMember::free_value()
  {
    delete _value;
    _value = nullptr;
  }

  TJMember* TJMember::try_read_string_and_value(const char*& p)
  {
    // first we look for the string, all the elements are supposed to have one.
    const auto& string = try_read_string(p);
    if (string == nullptr)
    {
      //  ERROR: could not read the string.
      return nullptr;
    }

    // then we look for the colon
    // only white spaces and the colon are allowed here.
    const char* colon_location = nullptr;
    while (*p != '\0')
    {
      char c = *p;
      switch (c)
      {
      TJ_CASE_SPACE
        p++;
        break;

      TJ_CASE_COLON
      if(colon_location != nullptr )
      {
        // ERROR: More than one colon located.
        delete[] string;
        return nullptr;
      }
      p++;
      colon_location = p;
      break;

      TJ_CASE_START_STRING
      {
        if (colon_location == nullptr)
        {
          // ERROR: Found a string before the colon
          delete[] string;
          return nullptr;
        }
        auto value = try_read_string(p);
        if (nullptr == value)
        {
          //  ERROR could not read the string properly.
          delete[] string;
          return nullptr;
        }

        // whave read the string
        // no need to try and move further forward.
        return new TJMember(string, new TJValueString(value));
      }

      case 't':
        if (!try_read_true(p))
        {
          //  ERROR could not read the word 'true'
          delete[] string;
          return nullptr;
        }
        return new TJMember(string, new TJValueTrue());

      case 'f':
        if (!try_read_false(p))
        {
          //  ERROR could not read the word 'true'
          delete[] string;
          return nullptr;
        }
        return new TJMember(string, new TJValueFalse());

      case 'n':
        if (!try_read_null(p))
        {
          //  ERROR could not read the word 'true'
          delete[] string;
          return nullptr;
        }
        return new TJMember(string, new TJValueNull());

      TJ_CASE_DIGIT
      TJ_CASE_SIGN
        {
          auto number = try_read_number(p);
          if (nullptr == number)
          {
            //  ERROR could not read the word 'true'
            delete[] string;
            return nullptr;
          }
          return new TJMember(string, number);
        }

      TJ_CASE_BEGIN_OBJECT
        {
          // an object within the object
          auto tjvalue_object = try_continue_read_object(++p);
          if (tjvalue_object == nullptr)
          {
            // Error:  something went wrong, the error was logged.
            delete tjvalue_object;
            return nullptr;
          }
          return new TJMember(string, tjvalue_object);
        }

      default:
        // ERROR: expected colon after the string
        delete[] string;
        return nullptr;
      }
    }

    // ERROR: Unable to read a string and/or value
    delete[] string;
    return nullptr;
  }

  bool TJMember::try_read_true(const char*& p)
  {
    if (*p != 't')
    {
      return false;
    }
    if (*(p + 1) != 'r')
    {
      return false;
    }
    if (*(p + 2) != 'u')
    {
      return false;
    }
    if (*(p + 3) != 'e')
    {
      return false;
    }

    p += 4;

    // all good.
    return true;
  }

  bool TJMember::try_read_false(const char*& p)
  {
    if (*p != 'f')
    {
      return false;
    }
    if (*(p + 1) != 'a')
    {
      return false;
    }
    if (*(p + 2) != 'l')
    {
      return false;
    }
    if (*(p + 3) != 's')
    {
      return false;
    }
    if (*(p + 4) != 'e')
    {
      return false;
    }

    p += 5;

    // all good.
    return true;
  }

  TJValue* TJMember::try_read_number(const char*& p)
  {
    bool is_negative = false;
    if (*p == '-')
    {
      is_negative = true;
      p++;
    }

    // then try and read the digit(s).
    auto possible_number = try_read_whole_number(p);
    if (nullptr == possible_number)
    {
      // ERROR: Could not locate he number.
      return nullptr;
    }

    // convert that number to an unsigned long, long
    const auto& unsigned_whole_number = std::strtoull(possible_number, nullptr, 10);
    delete[] possible_number;

    // read the faction if there is one.
    unsigned long long unsigned_fraction = 0;
    unsigned int fraction_exponent = 0;
    if (*p == '.')
    {
      p++;
      const auto& possible_fraction_number = try_read_whole_number_as_fraction(p);
      if (nullptr == possible_fraction_number)
      {
        // ERROR: we cannot have a number like '-12.' or '42.
        return nullptr;
      }

      // so 001 become exponent = 3
      fraction_exponent = std::strlen(possible_fraction_number);
      unsigned_fraction = std::strtoull(possible_fraction_number, nullptr, 10);
      delete [] possible_fraction_number;
    }

    // try read the exponent if there is one.
    long long exponent = 0;
    if (*p == 'e' || *p == 'E')
    {
      p++;

      bool is_negative_exponent = false;
      if (*p == '-')
      {
        is_negative_exponent = true;
        p++;
      }
      if (*p == '+')
      {
        is_negative_exponent = false;
        p++;
      }
      const auto& possible_exponent = try_read_whole_number(p);
      if (nullptr == possible_exponent)
      {
        // ERROR: we cannot have a number like '-12.' or '42.
        return nullptr;
      }

      const auto& unsigned_exponent = std::strtoull(possible_exponent, nullptr, 10);
      delete[] possible_exponent;

      if (0 == unsigned_exponent)
      {
        // ERROR: we cannot have an exponent with zero.
        return nullptr;
      }
      exponent = is_negative_exponent ? unsigned_exponent * -1 : unsigned_exponent;
    }
    return try_create_number_from_parts(is_negative, unsigned_whole_number, unsigned_fraction, fraction_exponent, exponent);
  }

  TJValue* TJMember::try_create_number_from_parts(const bool& is_negative, const unsigned long long& unsigned_whole_number, const unsigned long long& unsigned_fraction, const unsigned int& fraction_exponent, const long long& exponent)
  {
    // no exponent number is int or float
    if (exponent == 0)
    {
      if (unsigned_fraction == 0)
      {
        // zero is a positive number
        return new TJValueNumberInt(unsigned_whole_number, unsigned_whole_number == 0 ? false : is_negative);
      }
      return new TJValueNumberFloat(unsigned_whole_number, unsigned_fraction, fraction_exponent, is_negative);
    }

    // if the number is something like 123.456 with e=2
    // then the number will become 12345.6 e=0
    // so we need the number of digits.
    auto number_of_digit_whole = get_number_of_digits(unsigned_whole_number);
    auto number_of_digit_fraction = fraction_exponent;

    // if the number of digits is more than (LLONG_MAX = 9 223 372 036 854 775 807)
    // then we have to use exponent.
    if (exponent + number_of_digit_whole + number_of_digit_fraction > TJ_MAX_NUMBER_OF_DIGGITS)
    {
      // whatever the number we have to convert it to a single digit whole
      // and 19 numbers fraction and the rest is just the exponent.
      auto divider = std::pow(10, number_of_digit_whole - 1);
      auto shifted_unsigned_whole_number = static_cast<unsigned long long>(unsigned_whole_number / divider);

      // then if the number of decimals is less then 20-number_of_whole_digits then we can just shift it all down
      if ((number_of_digit_whole + number_of_digit_fraction -1) > TJ_MAX_NUMBER_OF_DIGGITS)
      {
        return nullptr;
      }

      // we need to multiply the remainder by the total number of fraction diggits
      const auto whole_number_remainder = static_cast<const unsigned long long>((unsigned_whole_number - (shifted_unsigned_whole_number * divider)) * std::pow(10, number_of_digit_fraction));
      const auto shitfted_unsigned_fraction = whole_number_remainder + unsigned_fraction;
      const auto shitfted_fraction_exponent = fraction_exponent + number_of_digit_whole - 1;
      const auto shitfted_exponent = exponent + number_of_digit_whole - 1;

      return new TJValueNumberExponent(shifted_unsigned_whole_number, shitfted_unsigned_fraction, shitfted_fraction_exponent, shitfted_exponent, is_negative);
    }

    // positive exponent.
    if (exponent > 0)
    {
      // more fractions than exponents so we can 
      // move the fraction to the whole number, (and maybe add zeros)
      if (fraction_exponent <= exponent)
      {
        auto shifted_unsigned_whole_number = unsigned_whole_number * std::pow(10, fraction_exponent);
        shifted_unsigned_whole_number += unsigned_fraction;
        if (exponent - fraction_exponent > 0)
        {
          shifted_unsigned_whole_number = shifted_unsigned_whole_number * std::pow(10, exponent - fraction_exponent);
        }
        return new TJValueNumberInt(shifted_unsigned_whole_number, is_negative);
      }

      // we cannot shift the full amount so we have something like 123.456e2
      // we need to multiply the whole number by exponent
      auto multiplier = std::pow(10, exponent);
      auto divider = std::pow(10, fraction_exponent - exponent);
      auto shifted_unsigned_whole_number = unsigned_whole_number * multiplier;
      // devide the fraction by fraction_digits - exponent
      auto number_to_add = static_cast<unsigned long long>(unsigned_fraction / divider);
      // add it to the wholenuber
      shifted_unsigned_whole_number += number_to_add;

      // finally we need to convert the fraction to just what we have not used.
      auto shifted_unsigned_fraction = (unsigned_fraction)-(number_to_add * divider);

      // we know it is a float.
      return new TJValueNumberFloat(shifted_unsigned_whole_number, shifted_unsigned_fraction, (fraction_exponent - exponent), is_negative);
    }

    // the exponent is negative, so we want to shift the whole number by the number of exponent,
    auto positive_exponent = -1 * exponent;
    auto multiplier = std::pow(10, number_of_digit_whole - positive_exponent);
    auto divider = std::pow(10, positive_exponent);

    // shift the while number
    auto shifted_unsigned_whole_number = static_cast<unsigned long long>(unsigned_whole_number / divider);

    // finally we need to convert the fraction to just what we have not used.
    auto shifted_unsigned_fraction = 0;
    if (unsigned_fraction > 0)
    {
      auto shifted_unsigned_fraction = static_cast<unsigned long long>(unsigned_fraction / divider);
    }
    const auto number_to_add = unsigned_whole_number - (shifted_unsigned_whole_number * divider);
    shifted_unsigned_fraction += number_to_add;
    if (shifted_unsigned_fraction == 0)
    {
      // it is an intiger
      return new TJValueNumberInt(shifted_unsigned_whole_number, is_negative);
    }

    // caluclate the fraction
    // if we have something like 1200012e-3 then the whole number is 12 and the fraction is 0.00012
    // but the while fraction number is '12' so we are shifting number of exponent + len of whole_fraction -1
    const unsigned int shifted_fraction_exponent = positive_exponent;

    // we know it is a float.
    return new TJValueNumberFloat(shifted_unsigned_whole_number, shifted_unsigned_fraction, shifted_fraction_exponent, is_negative);
  }

  int TJMember::get_number_of_digits(const unsigned long long& number )
  {
    unsigned long long truncated_number = number;
    int count = 0;
    do
    {
      truncated_number /= 10;
      ++count;
    } while (truncated_number != 0);
    return count;
  }

  bool TJMember::try_read_null(const char*& p)
  {
    if (*p != 'n')
    {
      return false;
    }
    if (*(p + 1) != 'u')
    {
      return false;
    }
    if (*(p + 2) != 'l')
    {
      return false;
    }
    if (*(p + 3) != 'l')
    {
      return false;
    }

    p += 4;

    // all good.
    return true;
  }

  char* TJMember::try_read_whole_number_as_fraction(const char*& p)
  {
    // try read the number
    auto whole_number = try_read_whole_number(p);
    if (nullptr == whole_number)
    {
      return nullptr;
    }
    // trip the trailling zeros are they are not needed in a fraction
    // but we need to make sure that we have at least one
    // so 42.000000 becomes 42.0
    auto len = std::strlen(whole_number);
    while (len > 1 && whole_number[len - 1] == '0')
    {
      whole_number[len - 1] = '\0';
      --len;
    }
    return whole_number;
  }

  char* TJMember::try_read_whole_number(const char*& p)
  {
    const char* start = nullptr;
    const char* end = nullptr;
    while (*p != '\0')
    {
      char c = *p;
      switch (c)
      {
      TJ_CASE_DIGIT
        if (nullptr == start)
        {
          start = p; // this is the start
        }
        p++;
        break;

        break;

      default:
        // if we are still in the string, then we are good.
        if (nullptr == start)
        {
          // ERROR: unknown character
          return nullptr;
        }

        // Calculate the length of the text inside the quotes
        const auto& length = p - start;

        // Allocate memory for the result string
        char* result = new char[length + 1];
        std::strncpy(result, start, length);
        result[length] = '\0'; // Null-terminate the string
        return result;
      }
    }

    // // ERROR: we could not close the object.
    return nullptr;
  }

  char* TJMember::try_read_string(const char*& p)
  {
    const char* firstQuote = nullptr;
    const char* secondQuote = nullptr;
    while (*p != '\0')
    {
      char c = *p;
      switch (c)
      {
        TJ_CASE_SPACE
          p++;
          break;

        TJ_CASE_START_STRING
          if (nullptr == firstQuote)
          {
            p++;
            firstQuote = p;  //  we need to add one to remove the quote.
          }
          else
          {
            secondQuote = p; //  we need to remove one as we do not want the quote.

            p++;

            // Calculate the length of the text inside the quotes
            const auto& length = secondQuote - firstQuote;

            // Allocate memory for the result string
            char* result = new char[length + 1];
            std::strncpy(result, firstQuote, length);
            result[length] = '\0'; // Null-terminate the string
            return result;
          }
        break;

      default:
        // if we are still in the string, then we are good.
        if (nullptr == firstQuote)
        {
          // ERROR: unknown character
          return nullptr;
        }
        p++;
        break;
      }
    }

    // // ERROR: we could not close the object.
    return nullptr;
  }

  TJValue* TJMember::try_read_object(const char*& p)
  {
    if (*p != '{')
    {
      return nullptr;
    }
    ++p;
    return try_continue_read_object(p);
  }

  void TJMember::free_members(std::vector<TJMember*>* members)
  {
    if (members == nullptr)
    {
      return;
    }

    for each (auto var in *members)
    {
      delete var;
    }
    delete members;
    members = nullptr;
  }

  TJValue* TJMember::try_continue_read_object(const char*& p)
  {
    //  assume no members in that object.
    bool found_comma = false;
    std::vector<TJMember*>* members = nullptr;
    bool after_string = false;
    bool waiting_for_a_string = false;
    while (*p != '\0')
    {
      char c = *p;
      switch (c)
      {
      TJ_CASE_SPACE
        p++;
        break;

      TJ_CASE_END_OBJECT
        // but is it what we expected?
        if (waiting_for_a_string)
        {
          // ERROR: unexpected end of object, there was a "," after
          //        the last string and we expected a string now, not a close "}"
          free_members(members);
          return false;
        }
        p++;

        // we are done, we found it.
        // we give the ownership of the members over.
        return new TJValueObject(members);

      TJ_CASE_START_STRING
      {
        // we got our string, no longer waiting for one.
        waiting_for_a_string = false;

        // we are no longer after the string
        after_string = false;

        // if we have members then it means we must have a comma
        // as we are expecting the elements to be separated by a comma
        // if we have no elements then it is the first one and it does not matter.
        if (members != nullptr && !found_comma)
        {
          // ERROR: expected a comma after the last element
          free_members(members);
          return nullptr;
        }

        // read the actual string and value
        // that's the way it has to be.
        auto element = TJMember::try_read_string_and_value(p);
        if (element == nullptr)
        {
          // ERROR: There was an error reading the name and/or the value
          free_members(members);
          return nullptr;
        }
        if (nullptr == members)
        {
          members = new std::vector<TJMember*>();
        }

        found_comma = false;
        members->push_back(element);
        after_string = true;
      }
      break;

      TJ_CASE_COMMA
        if (!after_string)
        {
          // ERROR: found a comma out of order
          free_members(members);
          return nullptr;
        }
        // we are no longer after the string
        after_string = false;
        waiting_for_a_string = true;
        found_comma = true;
        p++;
        break;

      default:
        // ERROR: unknown character
        free_members(members);
        return nullptr;
      }
    }

    // ERROR end of the string was found and we didn't find what we needed.
    free_members(members);
    return nullptr;
  }

  ///////////////////////////////////////
  /// TJValue
  TJValue::TJValue()
  {
  }

  ///////////////////////////////////////
  /// TJValue string
  TJValueString::TJValueString(const char* value) :
    _value(nullptr)
  {
  }

  TJValueString::TJValueString(char* value) :
    _value(value)
  {
  }

  TJValueString::~TJValueString()
  {
    free_value();
  }

  void TJValueString::free_value()
  {
    if (nullptr != _value)
    {
      delete[] _value;
    }
    _value = nullptr;
  }

  const char* TJValueString::to_string() const
  {
    return _value;
  }

  ///////////////////////////////////////
  /// TJValue true
  TJValueTrue::TJValueTrue()
  {
  }

  const char* TJValueTrue::to_string() const
  {
    return "true";
  }

  ///////////////////////////////////////
  /// TJValue false
  TJValueFalse::TJValueFalse()
  {
  }

  const char* TJValueFalse::to_string() const
  {
    return "false";
  }

  ///////////////////////////////////////
  /// TJValue null
  TJValueNull::TJValueNull()
  {
  }

  const char* TJValueNull::to_string() const
  {
    return "null";
  }

  ///////////////////////////////////////
  /// TJValueObject
  TJValueObject::TJValueObject() :
    _members(nullptr)
  {
  }

  TJValueObject::TJValueObject(std::vector<TJMember*>* members) :
    _members(members)
  {
  }

  TJValueObject::~TJValueObject()
  {
    free_members();
  }

  const char* TJValueObject::to_string() const
  {
    return "TJValueObject";
  }

  void TJValueObject::free_members()
  {
    if (_members == nullptr)
    {
      return;
    }

    for each (auto var in *_members)
    {
      delete var;
    }
    delete _members;
    _members = nullptr;
  }

  const TJValue* TJValueObject::try_get_value(const char* name) const
  {
    if (nullptr == name)
    {
      return nullptr;
    }
    if (nullptr == _members)
    {
      return nullptr;
    }

    auto it = std::find_if(_members->begin(), _members->end(), [&](TJMember* value) {
      return stricmp(name, value->name()) == 0;
      });

    return (it == _members->end()) ? nullptr : (*it)->value();
  }

  const char* TJValueObject::try_get_string(const char* name) const
  {
    auto value = try_get_value(name);
    return (value == nullptr) ? nullptr : value->to_string();
  }

  ///////////////////////////////////////
  /// TJValue Number
  TJValueNumber::TJValueNumber(const bool is_negative) : 
    _is_negative(is_negative)
  {
  }

  const char* TJValueNumber::to_string() const
  {
    return "number";
  }

  ///////////////////////////////////////
  /// TJValue whole Number
  TJValueNumberInt::TJValueNumberInt(const unsigned long long& number, const bool is_negative) :
    TJValueNumber(is_negative),
    _number(number)
  {
  }

  const long long TJValueNumberInt::get_number() const
  {
    return _is_negative ? -1* _number : _number;
  }

  ///////////////////////////////////////
  /// TJValue float Number
  TJValueNumberFloat::TJValueNumberFloat(const unsigned long long& number, const unsigned long long& fraction, const unsigned int& fraction_exponent, bool is_negative) :
    TJValueNumber(is_negative),
    _number(number),
    _fraction(fraction),
    _fraction_exponent(fraction_exponent)
  {
  }

  const long double TJValueNumberFloat::get_number() const
  {
    if (_fraction == 0) {
      return static_cast<const long double>(_number);
    }

    // Convert b to its fractional form
    const long double& pow = std::powl(10, _fraction_exponent);
    const auto& whole_number = _number * pow + _fraction;

    // Combine the number and the fraction
    return (_is_negative ? -1 : 1) * (whole_number / pow);
  }

  ///////////////////////////////////////
  /// TJValue float Number
  TJValueNumberExponent::TJValueNumberExponent(const unsigned long long& number, const unsigned long long& fraction, const unsigned int& fraction_exponent, const unsigned int& exponent, bool is_negative) :
    TJValueNumber(is_negative),
    _exponent(exponent),
    _fraction(fraction),
    _fraction_exponent(fraction_exponent),
    _number(number),
    _string(nullptr)
  {
    // we need space for the whole number + fraction + exponent
    _string = new char[255];

    // if we have no fraction, then just return it.
    if (_fraction == 0) 
    {
      std::sprintf(_string, "%llde+%i", _number, _exponent);
    }
    else
    {
      // rebuild the buffer and make sure that we have all the zeros for the fractions.
      std::sprintf(_string, "%lld.%0*llde+%i", _number, _fraction_exponent, _fraction, _exponent);
    }
  }

  TJValueNumberExponent::~TJValueNumberExponent()
  {
    if (_string != nullptr)
    {
      delete[] _string;
    }
  }

  const char* TJValueNumberExponent::to_string() const
  {
    return _string;
  }
} // TinyJSON