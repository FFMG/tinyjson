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

  unsigned long long TJMember::fast_string_to_long_long(const char* p)
  {
    // the numbers are unsigned and should only contain digits.
    // so we do not have signs or letters to worry about.
    register unsigned long long result = 0;
    while (*p != '\0')
    {
      char c = *p;
      switch (c)
      {
      TJ_CASE_SPACE
        p++;
        break;

      // it might sound silly to do it that way but it is faster
      // than doing something like result += c - '0'
      case '0':
        result *= 10;
        p++;
        break;

      case '1':
        result *= 10;
        result += 1;
        p++;
        break;

      case '2':
        result *= 10;
        result += 2;
        p++;
        break;

      case '3':
        result *= 10;
        result += 3;
        p++;
        break;

      case '4':
        result *= 10;
        result += 4;
        p++;
        break;

      case '5':
        result *= 10;
        result += 5;
        p++;
        break;

      case '6':
        result *= 10;
        result += 6;
        p++;
        break;

      case '7':
        result *= 10;
        result += 7;
        p++;
        break;

      case '8':
        result *= 10;
        result += 8;
        p++;
        break;

      case '9':
        result *= 10;
        result += 9;
        p++;
        break;

      default:
        return result;
      }
    }
    return result;
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
    const auto& unsigned_whole_number = fast_string_to_long_long(possible_number);
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
      unsigned_fraction = fast_string_to_long_long(possible_fraction_number);
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

      const auto& unsigned_exponent = fast_string_to_long_long(possible_exponent);
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

  TJValue* TJMember::try_create_number_from_parts_no_exponent(const bool& is_negative, const unsigned long long& unsigned_whole_number, const unsigned long long& unsigned_fraction, const unsigned int& fraction_exponent)
  {
    if (unsigned_fraction == 0)
    {
      // zero is a positive number
      return new TJValueNumberInt(unsigned_whole_number, unsigned_whole_number == 0 ? false : is_negative);
    }
    return new TJValueNumberFloat(unsigned_whole_number, unsigned_fraction, fraction_exponent, is_negative);
  }

  unsigned long long TJMember::shift_number_left(const unsigned long long source, const unsigned long long exponent)
  {
    if (exponent == 0)
    {
      return source;
    }
    const auto muliplier = std::pow(10, exponent);
    return source * muliplier;
  }

  unsigned long long TJMember::shift_number_right(const unsigned long long source, const unsigned long long exponent, unsigned long long& shifted_source)
  {
    const auto divider = std::pow(10, exponent);
    auto new_source = static_cast<unsigned long long>(source / divider);
    shifted_source = source - new_source * divider;
    return new_source;
  }

  unsigned long long TJMember::shift_fraction_left(const unsigned long long& fraction, const unsigned long long& fraction_exponent, const unsigned long long& exponent, unsigned long long& shifted_fraction, unsigned long long& shitfed_unsigned_fraction_exponent)
  {
    if (exponent > fraction_exponent)
    {
      // we are moving more to the left than we have fractions
      // so we just need to move the extra fraction
      shifted_fraction = 0;
      shitfed_unsigned_fraction_exponent = 0;
      return shift_number_left(fraction, exponent - fraction_exponent);
    }

    if (exponent == fraction_exponent)
    {
      // no shifting needed the number is already what we need.
      shifted_fraction = 0;
      shitfed_unsigned_fraction_exponent = 0;
      return fraction;
    }

    shitfed_unsigned_fraction_exponent = fraction_exponent - exponent;

    // we know that the fraction_exponent is bigger than the exponent.
    // so we are not shifting the whole way but we have to be careful as the
    // len of the fraction might actually be less than the fraction because of leading 0s
    // for example 0.0012 and 0.12 have a len of 2 but a fraction_exponent of 2 and 4
    // the lenght can never be more than the fraction exponent.
    const auto& fraction_length = get_number_of_digits(fraction);

    if (fraction_length == fraction_exponent)
    {
      auto divider = std::pow(10, shitfed_unsigned_fraction_exponent);
      const auto& shifted_unsigned_fraction = static_cast<unsigned long long>(fraction / divider);
      shifted_fraction = fraction - static_cast<unsigned long long>(shifted_unsigned_fraction * divider);
      return shifted_unsigned_fraction;
    }

    if (fraction_exponent - fraction_length <= 0)
    {
      // the number is 0.0012 and we want to shift 2 or less
      // so the new faction is 0.12 and the return number is zero
      shifted_fraction = fraction;
      return 0ll;
    }

    // the number is 0.0123 and we want to shift 3
    //   the retrun number is 12, (0123 shifted leftx3)
    //   the return fraction is 3, (0123 shifted leftx3 - 12)
    //   the return fraction_exponent is 1, (0123 shifted leftx3 - 12)
    // The number of leading zeros, (that we have to ignore), is the fraction_exponent - fraction_length
    const auto& leading_zeros = fraction_exponent - fraction_length;
    if (leading_zeros >= exponent)
    {
      // we have more leading zeros than the number of exponents we are trying to shift.
      // so the fraction remains the same and the shitfed_unsigned_fraction_exponent has already been updated.
      shifted_fraction = fraction;
      return 0ll;
    }

    auto divider = std::pow(10, shitfed_unsigned_fraction_exponent);
    const auto& shifted_unsigned_fraction = static_cast<unsigned long long>(fraction / divider);
    shifted_fraction = fraction - static_cast<unsigned long long>(shifted_unsigned_fraction * divider);
    return shifted_unsigned_fraction;
  }

  TJValue* TJMember::try_create_number_from_parts_positive_exponent_no_whole_number(const bool& is_negative, const unsigned long long& unsigned_fraction, const unsigned int& fraction_exponent, const unsigned long long& exponent)
  {
    if (exponent >= fraction_exponent)
    {
      unsigned long long shifted_unsigned_fraction = 0;
      unsigned long long shitfed_unsigned_fraction_exponent = 0;
      const auto& fraction_length = get_number_of_digits(unsigned_fraction);
      const auto& leading_zeros = fraction_exponent - fraction_length;
      // we just want the first number so we are passing a 1x exponent only
      // but we need to add the number of leading zeros to make sure that we shift properly.
      const auto& shifted_unsigned_whole_number = shift_fraction_left(unsigned_fraction, fraction_exponent, leading_zeros+1, shifted_unsigned_fraction, shitfed_unsigned_fraction_exponent);

      const auto& shifted_fraction_exponent = exponent - fraction_exponent;
      if (shifted_fraction_exponent <= TJ_MAX_NUMBER_OF_DIGGITS)
      {
        if (shifted_unsigned_fraction == 0)
        {
          return new TJValueNumberInt(shift_number_left(shifted_unsigned_whole_number, shifted_fraction_exponent), is_negative);
        }

        return new TJValueNumberFloat(
          shift_number_left(shifted_unsigned_whole_number, shifted_fraction_exponent),
          shifted_unsigned_fraction,
          shifted_fraction_exponent,
          is_negative);
      }

      // TODO: Cases where exponent is > than TJ_MAX_NUMBER_OF_DIGGITS
      return nullptr;
    }


    // the number is something like 0.00001e+3 the fraction_exponent is 4 and the exponent is 3
    // so we can just move the fraction to the left the whole number will remain zero
    const auto& shifted_fraction_exponent = fraction_exponent - exponent;
    if (shifted_fraction_exponent <= TJ_MAX_NUMBER_OF_DIGGITS)
    {
      // the number cannot be an int as it would mean that both
      // the whole number and the fraction are zer0
      return new TJValueNumberFloat( 0ull, unsigned_fraction, shifted_fraction_exponent, is_negative);
    }

    // TODO: Cases where exponent is > than TJ_MAX_NUMBER_OF_DIGGITS
    return nullptr;
  }

  TJValue* TJMember::try_create_number_from_parts_negative_exponent_no_whole_number(const bool& is_negative, const unsigned long long& unsigned_fraction, const unsigned int& fraction_exponent, const unsigned long long& exponent)
  {
    //
    // remember that this is a negative exponent ...
    //

    // if we have a fraction and no whole number then we can move the number to the right
    unsigned long long shifted_unsigned_fraction_exponent = 0;
    unsigned long long shifted_unsigned_fraction = 0;
    const auto& fraction_length = get_number_of_digits(unsigned_fraction);
    const auto& leading_zeros = fraction_exponent - fraction_length;
    // we just want the first number so we are passing a 1x exponent only
    // but we need to add the number of leading zeros to make sure that we shift properly.
    const auto& shifted_unsigned_whole_number = shift_fraction_left(unsigned_fraction, fraction_exponent, leading_zeros+1, shifted_unsigned_fraction, shifted_unsigned_fraction_exponent);

    const auto& actual_shifted_fraction_exponent = exponent + (fraction_exponent - shifted_unsigned_fraction_exponent);

    if (actual_shifted_fraction_exponent <= TJ_MAX_NUMBER_OF_DIGGITS)
    {
      if (shifted_unsigned_fraction == 0)
      {
        return new TJValueNumberInt(shift_number_left(shifted_unsigned_whole_number, shifted_unsigned_fraction_exponent), is_negative);
      }
      return new TJValueNumberFloat(
        shift_number_left(shifted_unsigned_whole_number, shifted_unsigned_fraction_exponent),
        shifted_unsigned_fraction,
        shifted_unsigned_fraction_exponent,
        is_negative);
    }

    // TODO: Cases where exponent is > than TJ_MAX_NUMBER_OF_DIGGITS
    return new TJValueNumberExponent(
      shifted_unsigned_whole_number,
      shifted_unsigned_fraction,
      shifted_unsigned_fraction_exponent,
      -1* actual_shifted_fraction_exponent,
      is_negative);
  }

  TJValue* TJMember::try_create_number_from_parts_positive_exponent(const bool& is_negative, const unsigned long long& unsigned_whole_number, const unsigned long long& unsigned_fraction, const unsigned int& fraction_exponent, const unsigned long long& exponent)
  {
    auto number_of_digit_whole = get_number_of_digits(unsigned_whole_number);
    auto number_of_digit_fraction = get_number_of_digits(unsigned_fraction)+fraction_exponent -1;

    // case 1:
    //   The total number is less than TJ_MAX_NUMBER_OF_DIGGITS
    //   so we can get rid of the exponent altogether.
    if (fraction_exponent <= exponent && number_of_digit_whole+exponent <= TJ_MAX_NUMBER_OF_DIGGITS)
    {
      // we know that the fraction will disapear because it is smaller than the total fraction
      // we want to first move the whole number by the number of fractions
      auto shifted_unsigned_whole_number = shift_number_left(unsigned_whole_number, fraction_exponent);
      // then add the fraction
      shifted_unsigned_whole_number += unsigned_fraction;
      // then shift it again with the rest of the exponent
      shifted_unsigned_whole_number = shift_number_left(shifted_unsigned_whole_number, exponent - fraction_exponent);

      return new TJValueNumberInt(shifted_unsigned_whole_number, is_negative);
    }

    if (fraction_exponent > exponent && number_of_digit_whole + exponent <= TJ_MAX_NUMBER_OF_DIGGITS <= TJ_MAX_NUMBER_OF_DIGGITS)
    {
      // we now know that the fraction will not completely shift.
      // so we must move the whole number by the number of expoent
      auto shifted_unsigned_whole_number = shift_number_left(unsigned_whole_number, exponent);
      
      // we then want to shift the fraction by the number of exponent and add that to the list.
      unsigned long long shifted_unsigned_fraction_exponent = 0;
      unsigned long long shifted_unsigned_fraction = 0;
      shifted_unsigned_whole_number += shift_fraction_left(unsigned_fraction, fraction_exponent, exponent, shifted_unsigned_fraction, shifted_unsigned_fraction_exponent);

      // as we sifted the fraction by the number of exponent
      // then the size of the fraction is smaller by the exponent.
      return new TJValueNumberFloat(shifted_unsigned_whole_number, shifted_unsigned_fraction, shifted_unsigned_fraction_exponent, is_negative);
    }

    // case 2:
    //  The total number if more than TJ_MAX_NUMBER_OF_DIGGITS
    //  So we have to move the whole number to be a single digit.
    //  and the fraction to be shifted accordingly
    //  and the exponent moved.

    // case 2a:
    //  The whole number if zero ... in that case we have to shift the fraction to the first whole number.
    //  But we might not make it and in that case the whole number will remain to zero
    //  But the fraction will shift one way or the other.
    if (unsigned_whole_number == 0)
    {
      return try_create_number_from_parts_positive_exponent_no_whole_number(is_negative, unsigned_fraction, fraction_exponent, exponent);
    }

    // case 2b:
    //   The whole number is more than zero _and_ the fraction is also non zero.
    //   The total is, (currently), greater than TJ_MAX_NUMBER_OF_DIGGITS
    const unsigned long long shifted_unsigned_whole_number_exponent = number_of_digit_whole - 1;
    unsigned long long shifted_unsigned_fraction = 0;
    const auto& shifted_unsigned_whole_number = shift_number_right(unsigned_whole_number, shifted_unsigned_whole_number_exponent, shifted_unsigned_fraction);

    // we then need to add shifted_unsigned_fraction in front of unsigned_fraction
    auto shifted_fraction_exponent = shifted_unsigned_whole_number_exponent + (fraction_exponent - shifted_unsigned_whole_number_exponent);
    shifted_unsigned_fraction = (shifted_unsigned_fraction *std::pow(10, shifted_fraction_exponent)) + unsigned_fraction;

    // and the exponent also shitt byt the number we moved.
    const unsigned long long shifted_exponent = exponent + shifted_unsigned_whole_number_exponent;

    return new TJValueNumberExponent(
      shifted_unsigned_whole_number, 
      shifted_unsigned_fraction, 
      (shifted_unsigned_whole_number_exponent + fraction_exponent), 
      shifted_exponent, 
      is_negative);
  }

  TJValue* TJMember::try_create_number_from_parts(const bool& is_negative, const unsigned long long& unsigned_whole_number, const unsigned long long& unsigned_fraction, const unsigned int& fraction_exponent, const long long& exponent)
  {
    // no exponent number is int or float
    if (exponent == 0)
    {
      return try_create_number_from_parts_no_exponent(is_negative, unsigned_whole_number, unsigned_fraction, fraction_exponent);
    }

    // positive exponent.
    if (exponent > 0)
    {
      return try_create_number_from_parts_positive_exponent(is_negative, unsigned_whole_number, unsigned_fraction, fraction_exponent, exponent);
    }

    // the exponent is negative, so we need to either shift the whole number and the fraction
    // but we have to be careful how it is shifted so we do not overflow one way or another.
    const auto& positive_exponent = -1 * exponent;
    return try_create_number_from_parts_negative_exponent(is_negative, unsigned_whole_number, unsigned_fraction, fraction_exponent, positive_exponent);
  }

  TJValue* TJMember::try_create_number_from_parts_negative_exponent(const bool& is_negative, const unsigned long long& unsigned_whole_number, const unsigned long long& unsigned_fraction, const unsigned int& fraction_exponent, const unsigned long long& exponent)
  {
    // if the number is something like 123.456 with e=2
    // then the number will become 12345.6 e=0
    // so we need the number of digits.
    auto number_of_digit_whole = get_number_of_digits(unsigned_whole_number);
    auto number_of_digit_fraction = get_number_of_digits(unsigned_fraction);

    // case 1:
    //   The total number is less than TJ_MAX_NUMBER_OF_DIGGITS
    //   so we can get rid of the exponent altogether.
    if (number_of_digit_whole + number_of_digit_fraction + exponent <= TJ_MAX_NUMBER_OF_DIGGITS)
    {
      // we will shift the whole number to the left by the number of exponent
      // then we wil shift the number of fraction to the lest by the number if exponent.
      // we will then add the two together.
      unsigned long long shifted_unsigned_fraction = 0;
      const auto& shifted_unsigned_whole_number = shift_number_right(unsigned_whole_number, exponent, shifted_unsigned_fraction);

      if (shifted_unsigned_fraction == 0)
      {
        return new TJValueNumberInt(shifted_unsigned_whole_number, is_negative);
      }

      const auto& shifted_fraction_exponent = fraction_exponent + exponent;
      return new TJValueNumberFloat(shifted_unsigned_whole_number, shifted_unsigned_fraction, shifted_fraction_exponent, is_negative);
    }

    // case 2:
    //  The total number if more than TJ_MAX_NUMBER_OF_DIGGITS
    //  So we have to move the whole number to be a single digit.
    //  and the fraction to be shifted accordingly
    //  and the exponent moved.

    // case 2a:
    //  The whole number if zero ... in that case we have to shift the fraction to the first whole number.
    //  But we might not make it and in that case the whole number will remain to zero
    //  But the fraction will shift one way or the other.
    if (unsigned_whole_number == 0)
    {
      return try_create_number_from_parts_negative_exponent_no_whole_number(is_negative, unsigned_fraction, fraction_exponent, exponent);
    }

    // case 2b:
    //   The whole number is more than zero _and_ the fraction is also non zero.
    //   The total is, (currently), greater than TJ_MAX_NUMBER_OF_DIGGITS
    const unsigned long long shifted_unsigned_whole_number_exponent = number_of_digit_whole - 1;
    unsigned long long shifted_unsigned_fraction = 0;
    const auto& shifted_unsigned_whole_number = shift_number_right(unsigned_whole_number, shifted_unsigned_whole_number_exponent, shifted_unsigned_fraction);

    // we then need to add shifted_unsigned_fraction in front of unsigned_fraction
    auto shifted_fraction_exponent = shifted_unsigned_whole_number_exponent + (fraction_exponent - shifted_unsigned_whole_number_exponent);
    shifted_unsigned_fraction = (shifted_unsigned_fraction * std::pow(10, shifted_fraction_exponent)) + unsigned_fraction;

    // and the exponent also shitt by the number we moved.
    // as it is a negative exponent we need to move to the left.
    const unsigned long long shifted_exponent = exponent - shifted_unsigned_whole_number_exponent;

    return new TJValueNumberExponent(
      shifted_unsigned_whole_number,
      shifted_unsigned_fraction,
      (shifted_unsigned_whole_number_exponent + fraction_exponent),
      -1 * shifted_exponent,
      is_negative);
  }

  int TJMember::get_number_of_digits(const unsigned long long& number )
  {
    if (number == 0)
    {
      return 0;
    }
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
  TJValueNumberExponent::TJValueNumberExponent(const unsigned long long& number, const unsigned long long& fraction, const unsigned int& fraction_exponent, const int& exponent, bool is_negative) :
    TJValueNumber(is_negative),
    _exponent(exponent),
    _fraction(fraction),
    _fraction_exponent(fraction_exponent),
    _number(number),
    _string(nullptr)
  {
  }

  TJValueNumberExponent::~TJValueNumberExponent()
  {
    if (_string != nullptr)
    {
      delete[] _string;
    }
  }

  void TJValueNumberExponent::make_string()
  {
    if (nullptr != _string)
    {
      return;
    }

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
      if (_exponent < 0)
      {
        std::sprintf(_string, "%lld.%0*llde%i", _number, _fraction_exponent, _fraction, _exponent);
      }
      else
      {
        std::sprintf(_string, "%lld.%0*llde+%i", _number, _fraction_exponent, _fraction, _exponent);
      }
    }
  }

  const char* TJValueNumberExponent::to_string() const
  {
    if (nullptr == _string)
    {
      // we only create the string value when the caller asks for it.
      // this is to make sure that we do not create it on parsing.
      const_cast<TJValueNumberExponent*>(this)->make_string();
    }
    return _string;
  }
} // TinyJSON