// Licensed to Florent Guelfucci under one or more agreements.
// Florent Guelfucci licenses this file to you under the MIT license.
// See the LICENSE file in the project root for more information.
#include "TinyJSON.h"

#include <algorithm>
#include <cmath>
#include <cstring> 
#include <cstdio>

static constexpr short TJ_MAX_NUMBER_OF_DIGGITS = 19;
static constexpr short TJ_DEFAULT_STRING_READ_SIZE = 10;

#define TJ_CASE_NULL_TERMINATOR '\0'

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

#define TJ_CASE_BEGIN_ARRAY   case '[':

#define TJ_CASE_END_ARRAY     case ']':

#define TJ_CASE_MAYBE_ESCAPE  case '\\':

static constexpr char* TJ_DUMP_CONFIG_DEFAULT_VALUE_QUOTE = "\"";
static constexpr char* TJ_DUMP_CONFIG_DEFAULT_KEY_QUOTE = "\"";

namespace TinyJSON
{
  struct internal_dump_configuration
  {
    char* _buffer;
    const formating _formating;
    const char* _indent;
    int _buffer_pos;
    int _buffer_max_length;

    const char* _item_separator;
    const char* _key_separator;
    const char* _value_quote = TJ_DUMP_CONFIG_DEFAULT_VALUE_QUOTE;
    const char* _key_quote = TJ_DUMP_CONFIG_DEFAULT_KEY_QUOTE;

    internal_dump_configuration(
      formating formating, 
      const char* indent,
      const char* item_separator,
      const char* key_separator,
      const char* value_quote = TJ_DUMP_CONFIG_DEFAULT_VALUE_QUOTE,
      const char* key_quote = TJ_DUMP_CONFIG_DEFAULT_KEY_QUOTE
    ) :
      _formating(formating),
      _indent(indent),
      _item_separator(item_separator),
      _key_separator(key_separator),
      _value_quote(value_quote),
      _key_quote(key_quote)
    {
      _buffer = nullptr;
      _buffer_max_length = _buffer_pos = 0;
    }
  };

  ///////////////////////////////////////
  /// Protected Helper class
  class TJHelper
  {
    friend TinyJSON;
    friend TJValue;
    friend TJValueArray;
    friend TJValueBoolean;
    friend TJValueNumberExponent;
    friend TJValueNull;
    friend TJValueNumberFloat;
    friend TJValueNumberInt;
    friend TJValueObject;
    friend TJValueString;
  protected:
    static void free_members(std::vector<TJMember*>* members)
    {
      if (members == nullptr)
      {
        return;
      }

      for (auto var : *members)
      {
        delete var;
      }
      delete members;
      members = nullptr;
    }

    static void free_values(std::vector<TJValue*>* values)
    {
      if (values == nullptr)
      {
        return;
      }

      for (auto var : *values)
      {
        delete var;
      }
      delete values;
      values = nullptr;
    }

    static unsigned long long get_number_of_digits(const unsigned long long& number)
    {
      if (number == 0)
      {
        return 0ull;
      }
      unsigned long long truncated_number = number;
      unsigned long long count = 0;
      do
      {
        truncated_number /= 10;
        ++count;
      } while (truncated_number != 0);
      return count;
    }

    static char* resize_string(char*& source, int length, int resize_length)
    {
      char* new_string = new char[resize_length];
      memset(new_string, TJ_CASE_NULL_TERMINATOR, sizeof(char) * resize_length);
      if (source == nullptr)
      {
        return new_string;
      }

      auto actual_length = length < resize_length ? length : resize_length;
      for (auto i = 0; i < actual_length; ++i)
      {
        new_string[i] = source[i];
      }
      delete[] source;
      source = nullptr;
      return new_string;
    }

    static void add_char_to_string(const char char_to_add, char*& buffer, int& buffer_pos, int& buffer_max_length)
    {
      if (buffer_pos + 1 >= buffer_max_length)
      {
        buffer = resize_string(buffer, buffer_max_length, buffer_max_length + TJ_DEFAULT_STRING_READ_SIZE);
        buffer_max_length += TJ_DEFAULT_STRING_READ_SIZE;
      }
      buffer[buffer_pos] = char_to_add;
      ++buffer_pos;
    }

    static void add_string_to_string(const char* string_to_add, char*& buffer, int& buffer_pos, int& buffer_max_length)
    {
      if (nullptr == string_to_add)
      {
        return;
      }
      if (nullptr == buffer)
      {
        buffer = resize_string(buffer, buffer_max_length, buffer_max_length + TJ_DEFAULT_STRING_READ_SIZE);
      }
      while (*string_to_add != TJ_CASE_NULL_TERMINATOR)
      {
        add_char_to_string(*string_to_add, buffer, buffer_pos, buffer_max_length);
        string_to_add++;
      }
    }

    static void try_add_char_to_string_after_escape(const char*& source, char*& result, int& result_pos, int& result_max_length)
    {
      const auto& next_char = *(source + 1);
      if (next_char == TJ_CASE_NULL_TERMINATOR)
      {
        return;
      }

      // as per RFC8259
      switch (next_char)
      {
      case '"': // "    quotation mark  U+0022
      case '/': // \    reverse solidus U+005C
      case '\\':// /    solidus         U+002F 
        // skip the escpape and keep the character
        source++;
        break;

      case'f':  // f    form feed       U+000C
        source++;
        add_char_to_string('\f', result, result_pos, result_max_length);
        return;

      case 'b': // b    backspace       U+0008
        source++;
        add_char_to_string('\b', result, result_pos, result_max_length);
        return;

      case 'n': // n    line feed       U+000A
        source++;
        add_char_to_string('\n', result, result_pos, result_max_length);
        return;

      case 'r': // r    carriage return U+000D
        source++;
        add_char_to_string('\r', result, result_pos, result_max_length);
        return;

      case 't': // t    tab             U+0009
        source++;
        add_char_to_string('\t', result, result_pos, result_max_length);
        return;

      default:
        break;
      }

      // if we escaped or not it does not matter.
      // just add the character now.
      add_char_to_string(*source, result, result_pos, result_max_length);
    }

    static char* try_read_string(const char*& p)
    {
      int result_pos = 0;
      int result_max_length = 0;
      char* result = nullptr;
      const char* firstQuote = nullptr;
      while (*p != TJ_CASE_NULL_TERMINATOR)
      {
        switch (*p)
        {
          TJ_CASE_SPACE
            add_char_to_string(*p, result, result_pos, result_max_length);
            p++;
          break;

          TJ_CASE_MAYBE_ESCAPE
            try_add_char_to_string_after_escape(p, result, result_pos, result_max_length);
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
              p++;

              // Allocate memory for the result string
              // Null-terminate the string
              add_char_to_string(TJ_CASE_NULL_TERMINATOR, result, result_pos, result_max_length);
              return result;
            }
          break;

        default:
          // if we are still in the string, then we are good.
          if (nullptr == firstQuote)
          {
            delete[] result;
            // ERROR: unknown character
            return nullptr;
          }

          add_char_to_string(*p, result, result_pos, result_max_length);
          p++;
          break;
        }
      }

      // // ERROR: we could not close the object.
      delete[] result;
      return nullptr;
    }

    static bool try_skip_colon(const char*& p)
    {
      while (*p != TJ_CASE_NULL_TERMINATOR)
      {
        switch (*p)
        {
          TJ_CASE_SPACE
            p++;
          break;

          TJ_CASE_COLON
            p++;
          return true;

        default:
          // ERROR: could not find the expected colon
          return false;
        }
      }

      // ERROR: This should never be reached, unless the string does not contain a '\0'
      return false;
    }

    static TJValue* try_continue_read_true(const char*& p)
    {
      if (*(p) != 'r')
      {
        return nullptr;
      }
      if (*(p + 1) != 'u')
      {
        return nullptr;
      }
      if (*(p + 2) != 'e')
      {
        return nullptr;
      }

      p += 3;

      // all good.
      return new TJValueBoolean(true);
    }

    static TJValue* try_continue_read_false(const char*& p)
    {
      if (*(p) != 'a')
      {
        return nullptr;
      }
      if (*(p + 1) != 'l')
      {
        return nullptr;
      }
      if (*(p + 2) != 's')
      {
        return nullptr;
      }
      if (*(p + 3) != 'e')
      {
        return nullptr;
      }

      p += 4;

      // all good.
      return new TJValueBoolean(false);
    }

    static TJValue* try_continue_read_null(const char*& p)
    {
      if (*(p) != 'u')
      {
        return nullptr;
      }
      if (*(p + 1) != 'l')
      {
        return nullptr;
      }
      if (*(p + 2) != 'l')
      {
        return nullptr;
      }

      p += 3;

      // all good.
      return new TJValueNull();
    }

    static TJValue* try_create_number_from_parts_no_exponent(const bool& is_negative, const unsigned long long& unsigned_whole_number, const unsigned long long& unsigned_fraction, const unsigned int& fraction_exponent)
    {
      if (unsigned_fraction == 0)
      {
        // zero is a positive number
        return new TJValueNumberInt(unsigned_whole_number, unsigned_whole_number == 0 ? false : is_negative);
      }
      return new TJValueNumberFloat(unsigned_whole_number, unsigned_fraction, fraction_exponent, is_negative);
    }

    static unsigned long long shift_number_left(const unsigned long long source, const unsigned long long exponent)
    {
      if (exponent == 0)
      {
        return source;
      }
      const auto muliplier = std::pow(10, exponent);
      return source * muliplier;
    }

    static unsigned long long shift_number_right(const unsigned long long source, const unsigned long long exponent, unsigned long long& shifted_source)
    {
      const auto divider = std::pow(10, exponent);
      auto new_source = static_cast<unsigned long long>(source / divider);
      shifted_source = source - new_source * divider;
      return new_source;
    }

    static unsigned long long shift_fraction_left(const unsigned long long& fraction, const unsigned long long& fraction_exponent, const unsigned long long& exponent, unsigned long long& shifted_fraction, unsigned long long& shitfed_unsigned_fraction_exponent)
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

    static TJValue* try_create_number_from_parts_positive_exponent_no_whole_number(const bool& is_negative, const unsigned long long& unsigned_fraction, const unsigned int& fraction_exponent, const unsigned long long& exponent)
    {
      if (exponent >= fraction_exponent)
      {
        unsigned long long shifted_unsigned_fraction = 0;
        unsigned long long shitfed_unsigned_fraction_exponent = 0;
        const auto& fraction_length = get_number_of_digits(unsigned_fraction);
        const auto& leading_zeros = fraction_exponent - fraction_length;
        // we just want the first number so we are passing a 1x exponent only
        // but we need to add the number of leading zeros to make sure that we shift properly.
        const auto& shifted_unsigned_whole_number = shift_fraction_left(unsigned_fraction, fraction_exponent, leading_zeros + 1, shifted_unsigned_fraction, shitfed_unsigned_fraction_exponent);

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
        return new TJValueNumberFloat(0ull, unsigned_fraction, shifted_fraction_exponent, is_negative);
      }

      // TODO: Cases where exponent is > than TJ_MAX_NUMBER_OF_DIGGITS
      return nullptr;
    }

    static TJValue* try_create_number_from_parts_positive_exponent(const bool& is_negative, const unsigned long long& unsigned_whole_number, const unsigned long long& unsigned_fraction, const unsigned int& fraction_exponent, const unsigned long long& exponent)
    {
      const auto number_of_digit_whole = get_number_of_digits(unsigned_whole_number);

      // case 1:
      //   The total number is less than TJ_MAX_NUMBER_OF_DIGGITS
      //   so we can get rid of the exponent altogether.
      if (fraction_exponent <= exponent && number_of_digit_whole + exponent <= TJ_MAX_NUMBER_OF_DIGGITS)
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

      if (fraction_exponent > exponent && (number_of_digit_whole + exponent) <= TJ_MAX_NUMBER_OF_DIGGITS)
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
      shifted_unsigned_fraction = (shifted_unsigned_fraction * std::pow(10, shifted_fraction_exponent)) + unsigned_fraction;

      // and the exponent also shitt byt the number we moved.
      const unsigned long long shifted_exponent = exponent + shifted_unsigned_whole_number_exponent;

      return new TJValueNumberExponent(
        shifted_unsigned_whole_number,
        shifted_unsigned_fraction,
        (shifted_unsigned_whole_number_exponent + fraction_exponent),
        shifted_exponent,
        is_negative);
    }

    static TJValue* try_create_number_from_parts_negative_exponent(const bool& is_negative, const unsigned long long& unsigned_whole_number, const unsigned long long& unsigned_fraction, const unsigned int& fraction_exponent, const unsigned long long& exponent)
    {
      // if the number is something like 123.456 with e=2
      // then the number will become 12345.6 e=0
      // so we need the number of digits.
      const auto& number_of_digit_whole = get_number_of_digits(unsigned_whole_number);
      const auto& number_of_digit_fraction = get_number_of_digits(unsigned_fraction);

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

    static TJValue* try_create_number_from_parts_negative_exponent_no_whole_number(const bool& is_negative, const unsigned long long& unsigned_fraction, const unsigned int& fraction_exponent, const unsigned long long& exponent)
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
      const auto& shifted_unsigned_whole_number = shift_fraction_left(unsigned_fraction, fraction_exponent, leading_zeros + 1, shifted_unsigned_fraction, shifted_unsigned_fraction_exponent);

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
        -1 * actual_shifted_fraction_exponent,
        is_negative);
    }

    static TJValue* try_create_number_from_parts(const bool& is_negative, const unsigned long long& unsigned_whole_number, const unsigned long long& unsigned_fraction, const unsigned int& fraction_exponent, const long long& exponent)
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

    /// <summary>
    /// The function looks for a whole number and stops as soon as we find decimal and/or exponent.
    /// </summary>
    /// <param name="p">The current string pointer.</param>
    /// <returns></returns>
    static char* try_read_whole_number(const char*& p)
    {
      const char* start = nullptr;
      int found_spaces = 0;
      while (*p != TJ_CASE_NULL_TERMINATOR)
      {
        switch (*p)
        {
          TJ_CASE_SPACE
            if (nullptr != start)
            {
              ++found_spaces;
            }
            p++;
            break;

          TJ_CASE_DIGIT
            if (nullptr == start)
            {
              start = p; // this is the start
            }
            if (found_spaces > 0)
            {
              // ERROR: Number has a space between it.
              return nullptr;
            }
            p++;
            break;

        default:
          return read_string( start, p, found_spaces);
        }
      }

      return read_string(start, p, found_spaces);
    }

    static char* read_string(const char* start, const char* end, int spaces)
    {
      if (nullptr == start)
      {
        // ERROR: unknown character
        return nullptr;
      }
      // Calculate the length of the text inside the quotes
      const auto& length = end - start - spaces;
      // Allocate memory for the result string
      char* result = new char[length + 1];
      std::strncpy(result, start, length);
      result[length] = TJ_CASE_NULL_TERMINATOR; // Null-terminate the string
      return result;
    }

    static char* try_read_whole_number_as_fraction(const char*& p)
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
        whole_number[len - 1] = TJ_CASE_NULL_TERMINATOR;
        --len;
      }
      return whole_number;
    }

    static unsigned long long fast_string_to_long_long(const char* p)
    {
      // the numbers are unsigned and should only contain digits.
      // so we do not have signs or letters to worry about.
      long long result = 0;
      while (*p != TJ_CASE_NULL_TERMINATOR)
      {
        char c = *p;
        switch (c)
        {
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
          // ERROR: only numbers are expected.
          return 0ull;
        }
      }
      return result;
    }

    static TJValue* try_read_number(const char*& p)
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
        // ERROR: Could not locate the number.
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
        delete[] possible_fraction_number;
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

    /// <summary>
    /// We are moving the owership of the TJMember to the vector.
    /// If the vector is not created we will create it.
    /// Duplicate values will be overwiten here.
    /// </summary>
    /// <param name="member"></param>
    /// <param name="members"></param>
    static void move_member_to_members(TJMember* member, std::vector<TJMember*>*& members)
    {
      if (nullptr == members)
      {
        members = new std::vector<TJMember*>();
      }
      else
      {
        auto current = std::find_if(members->begin(), members->end(), [&](TJMember*& elem) 
          { 
            return strcmp( elem->name(), member->name()) == 0; 
          });
        if (current != members->end())
        {
          delete* current;
          *current = member;
          return;
        }
      }
      members->push_back(member);
    }

    /// <summary>
    /// Try and read an object after we have read the openning bracket
    /// This is to prevent having to read the same char more than once.
    /// </summary>
    /// <param name="p"></param>
    /// <returns></returns>
    static TJValue* try_continue_read_object(const char*& p)
    {
      //  assume no members in that object.
      bool found_comma = false;
      std::vector<TJMember*>* members = nullptr;
      bool after_string = false;
      bool waiting_for_a_string = false;
      while (*p != TJ_CASE_NULL_TERMINATOR)
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
              return nullptr;
            }
          p++;

          // we are done, we found it.
          // we give the ownership of the members over.
          return TJValueObject::move(members);

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
          auto member = try_read_string_and_value(p);
          if (member == nullptr)
          {
            // ERROR: There was an error reading the name and/or the value
            free_members(members);
            return nullptr;
          }

          found_comma = false;
          move_member_to_members(member, members);
          
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

    /// <summary>
    /// Try and read an array after we have read the opening bracket.
    /// This is to prevent having to read the same char more than once.
    /// </summary>
    /// <param name="p"></param>
    /// <returns></returns>
    static TJValue* try_continue_read_array(const char*& p)
    {
      //  assume no values in that array
      std::vector<TJValue*>* values = nullptr;
      bool waiting_for_a_value = true;
      bool found_comma = false;
      while (*p != TJ_CASE_NULL_TERMINATOR)
      {
        char c = *p;
        switch (c)
        {
          TJ_CASE_SPACE
            p++;
          break;

          TJ_CASE_END_ARRAY
            if (found_comma && waiting_for_a_value)
            {
              // ERROR: unexpected end of array, there was a "," after
              //        the last value and we expected a value now, not a close "]"
              free_values(values);
              return nullptr;
            }
          p++;

          // we are done, we found it.
          // we give the ownership of the members over.
          return TJValueArray::move(values);

          TJ_CASE_COMMA
            if (waiting_for_a_value)
            {
              // ERROR: found a comma out of order, (2 commas)
              free_values(values);
              return nullptr;
            }
          // we are now waiting for a value
          waiting_for_a_value = true;
          found_comma = true;
          p++;
          break;

        default:
          const auto& value = try_read_Value(p);
          if (value == nullptr)
          {
            // ERROR: unknown character
            free_values(values);
            return nullptr;
          }
          if (nullptr == values)
          {
            values = new std::vector<TJValue*>();
          }
          else if (found_comma == false && values->size() > 0)
          {
            // ERROR: We found a value but we expected a comma.
            delete value;
            free_values(values);
            return nullptr;
          }
          values->push_back(value);
          waiting_for_a_value = false;
          found_comma = false;
          break;
        }
      }

      // ERROR end of the string was found and we didn't find what we needed.
      free_values(values);
      return nullptr;
    }

    static TJValue* try_read_Value(const char*& p)
    {
      while (*p != TJ_CASE_NULL_TERMINATOR)
      {
        char c = *p;
        switch (c)
        {
          TJ_CASE_SPACE
            p++;
          break;

          TJ_CASE_START_STRING
          {
            auto value = try_read_string(p);
            if (nullptr == value)
            {
              //  ERROR could not read the string properly.
              return nullptr;
            }

            // whave read the string
            // no need to try and move further forward.
            return TJValueString::move(value);
          }

        case 't':
          {
            auto true_value = try_continue_read_true(++p);
            if (nullptr == true_value)
            {
              //  ERROR could not read the word 'true'
              return nullptr;
            }
            return true_value;
          }

        case 'f':
        {
          auto false_value = try_continue_read_false(++p);
          if (nullptr == false_value)
          {
            //  ERROR could not read the word 'true'
            return nullptr;
          }
          return false_value;
        }

        case 'n':
        {
          auto null_value = try_continue_read_null(++p);
          if (nullptr == null_value)
          {
            //  ERROR could not read the word 'true'
            return nullptr;
          }
          return null_value;
        }

        TJ_CASE_DIGIT
          TJ_CASE_SIGN
        {
          auto number = try_read_number(p);
          if (nullptr == number)
          {
            //  ERROR could not read the word 'true'
            return nullptr;
          }
          return number;
        }

          TJ_CASE_BEGIN_ARRAY
        {
          // an array within an array
          auto tjvalue_array = try_continue_read_array(++p);
          if (tjvalue_array == nullptr)
          {
            // Error:  something went wrong, the error was logged.
            return nullptr;
          }
          return tjvalue_array;
        }

          TJ_CASE_BEGIN_OBJECT
        {
          // an object within the object
          auto tjvalue_object = try_continue_read_object(++p);
          if (tjvalue_object == nullptr)
          {
            // Error:  something went wrong, the error was logged.
            return nullptr;
          }
          return tjvalue_object;
        }

        default:
          // ERROR: expected colon after the string
          return nullptr;
        }
      }

      // ERROR: Unable to read a string and/or value
      return nullptr;
    }

    static TJMember* try_read_string_and_value(const char*& p)
    {
      // first we look for the string, all the elements are supposed to have one.
      auto string = try_read_string(p);
      if (string == nullptr)
      {
        //  ERROR: could not read the string.
        return nullptr;
      }

      // then we look for the colon
      // only white spaces and the colon are allowed here.
      if (!try_skip_colon(p))
      {
        delete[] string;
        //  ERROR: Could not locate the expected colon
        return nullptr;
      }

      auto value = try_read_Value(p);
      if (nullptr == value)
      {
        delete[] string;
        //  ERROR: Could not read the value, the error was logged.
        return nullptr;
      }
      return TJMember::move(string, value);
    }
  };  // Helper class

  ///////////////////////////////////////
  /// TinyJSON
  TJValue* TinyJSON::parse(const char* source)
  {
    // we can only have one value and nothing else
    TJValue* value_found = nullptr;
    while (*source != TJ_CASE_NULL_TERMINATOR)
    {
      switch (*source)
      {
      TJ_CASE_SPACE
        source++;
        break;

      default:
        if (nullptr != value_found)
        {
          // we cannot find anything else, we already found our value.
          delete value_found;
          value_found = nullptr;
          return nullptr;
        }

        // try and look for the value
        value_found = TJHelper::try_read_Value(source);
        if (nullptr == value_found)
        {
          // there was an issue trying to parse.
          return nullptr;
        }
        break;
      }
    }

    // return if we found anything.
    // if we found nothing ... then it is not an error, 
    // just an empty string
    return value_found != nullptr ? value_found : new TJValueString("");
  }

  ///////////////////////////////////////
  /// TJMember
  TJMember::TJMember(const char* string, const TJValue* value):
    _string(nullptr),
    _value(nullptr)
  {
    if (string != nullptr)
    {
      _string = new char[std::strlen(string) + 1];
      std::strcpy(_string, string);
    }
    if (value != nullptr)
    {
      _value = value->clone();
    }
  }

  TJMember* TJMember::move(char*& string, TJValue*& value)
  {
    auto member = new TJMember(nullptr, nullptr);
    member->_string = string;
    member->_value = value;

    value = nullptr;
    string = nullptr;
    return member;
  }

  TJMember::~TJMember()
  {
    free_string();
    free_value();
  }

  const char* TJMember::name() const
  {
    return _string;
  }

  const TJValue* TJMember::value() const
  {
    return _value;
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

  ///////////////////////////////////////
  /// TJValue
  TJValue::TJValue() : 
    _last_dump(nullptr)
  {
  }

  TJValue::~TJValue()
  {
    free_last_dump();
  }

  void TJValue::free_last_dump()  const
  {
    if (nullptr != _last_dump)
    {
      delete[] _last_dump;
      _last_dump = nullptr;
    }
  }

  bool TJValue::is_object() const
  {
    return false;
  }

  bool TJValue::is_array() const
  {
    return false;
  }

  bool TJValue::is_string() const
  {
    return false;
  }

  bool TJValue::is_number() const
  {
    return false;
  }

  bool TJValue::is_true() const
  {
    return false;
  }

  bool TJValue::is_false() const
  {
    return false;
  }

  bool TJValue::is_null() const
  {
    return false;
  }

  const char* TJValue::dump(formating formating, const char* indent) const
  {
    free_last_dump();
    switch (formating)
    {
    case formating::none:
      {
        internal_dump_configuration configuration(formating, nullptr, ",", ":");
        internal_dump(configuration, nullptr);
        _last_dump = configuration._buffer;
      }
      break;
    case formating::indented:
      {
        internal_dump_configuration configuration(formating, indent, ",", ": ");
        internal_dump(configuration, nullptr);
        _last_dump = configuration._buffer;
      }
      break;
    }
    return _last_dump;
  }

  const char* TJValue::dump_string() const
  {
    free_last_dump();
    internal_dump_configuration configuration(formating::none, nullptr, nullptr, nullptr, nullptr);
    internal_dump(configuration, nullptr);
    _last_dump = configuration._buffer;
    return _last_dump;
  }

  ///////////////////////////////////////
  /// TJValue string
  TJValueString::TJValueString(const char* value) :
    _value(nullptr)
  {
    if (value != nullptr)
    {
      size_t length = std::strlen(value) + 1;
      _value = new char[length];
      std::strcpy(_value, value);
    }
  }

  TJValueString::~TJValueString()
  {
    free_value();
  }

  TJValueString* TJValueString::move(char*& value)
  {
    auto string = new TJValueString(nullptr);
    string->_value = value;
    value = nullptr;
    return string;
  }

  TJValue* TJValueString::clone() const
  {
    return new TJValueString(_value);
  }

  void TJValueString::internal_dump(internal_dump_configuration& configuration, const char* current_indent) const
  {
    // then the word we are after with the quotes around.
    TJHelper::add_string_to_string(configuration._value_quote, configuration._buffer, configuration._buffer_pos, configuration._buffer_max_length);
    TJHelper::add_string_to_string(_value == nullptr ? "" : _value, configuration._buffer, configuration._buffer_pos, configuration._buffer_max_length);
    TJHelper::add_string_to_string(configuration._value_quote, configuration._buffer, configuration._buffer_pos, configuration._buffer_max_length);
  }

  bool TJValueString::is_string() const
  {
    return true;
  }

  void TJValueString::free_value()
  {
    if (nullptr != _value)
    {
      delete[] _value;
    }
    _value = nullptr;
  }

  ///////////////////////////////////////
  /// TJValue true
  TJValueBoolean::TJValueBoolean(bool is_true) :
    _is_true(is_true)
  {
  }

  TJValue* TJValueBoolean::clone() const
  {
    return new TJValueBoolean(_is_true);
  }

  void TJValueBoolean::internal_dump(internal_dump_configuration& configuration, const char* current_indent) const
  {
    // then the word we are after
    TJHelper::add_string_to_string(_is_true ? "true":"false", configuration._buffer, configuration._buffer_pos, configuration._buffer_max_length);
  }

  bool TJValueBoolean::is_true() const
  {
    return _is_true;
  }

  bool TJValueBoolean::is_false() const
  {
    return !_is_true;
  }

  ///////////////////////////////////////
  /// TJValue null
  TJValueNull::TJValueNull()
  {
  }

  TJValue* TJValueNull::clone() const
  {
    return new TJValueNull();
  }

  void TJValueNull::internal_dump(internal_dump_configuration& configuration, const char* current_indent) const
  {
    // then the word we are after
    TJHelper::add_string_to_string("null", configuration._buffer, configuration._buffer_pos, configuration._buffer_max_length);
  }

  bool TJValueNull::is_null() const
  {
    return true;
  }

  ///////////////////////////////////////
  /// TJValueObject
  TJValueObject::TJValueObject() :
    _members(nullptr)
  {
  }

  TJValueObject::~TJValueObject()
  {
    free_members();
  }

  TJValueObject* TJValueObject::move(std::vector<TJMember*>*& members)
  {
    auto object = new TJValueObject();
    object->_members = members;
    members = nullptr;
    return object;
  }

  TJValue* TJValueObject::clone() const
  {
    auto object = new TJValueObject();
    if (_members != nullptr)
    {
      auto members = new std::vector<TJMember*>();
      for (auto& member : *_members)
      {
        members->push_back(new TJMember(member->name(), member->value()->clone()));
      }
      object->_members = members;
    }
    return object;
  }

  void TJValueObject::internal_dump(internal_dump_configuration& configuration, const char* current_indent) const
  {
    // open it
    TJHelper::add_char_to_string('{', configuration._buffer, configuration._buffer_pos, configuration._buffer_max_length);

    auto number_of_items = get_number_of_items();
    if (number_of_items > 0)
    {
      // only return if we have data.
      if (configuration._formating == formating::indented)
      {
        TJHelper::add_char_to_string('\n', configuration._buffer, configuration._buffer_pos, configuration._buffer_max_length);
      }

      int inner_buffer_pos = 0;
      int inner_buffer_max_length = 0;
      char* inner_current_indent = nullptr;

      TJHelper::add_string_to_string(current_indent, inner_current_indent, inner_buffer_pos, inner_buffer_max_length);
      TJHelper::add_string_to_string(configuration._indent, inner_current_indent, inner_buffer_pos, inner_buffer_max_length);

      for (const auto& member : *_members)
      {
        TJHelper::add_string_to_string(inner_current_indent, configuration._buffer, configuration._buffer_pos, configuration._buffer_max_length);

        TJHelper::add_string_to_string(configuration._key_quote, configuration._buffer, configuration._buffer_pos, configuration._buffer_max_length);
        TJHelper::add_string_to_string(member->name(), configuration._buffer, configuration._buffer_pos, configuration._buffer_max_length);
        TJHelper::add_string_to_string(configuration._key_quote, configuration._buffer, configuration._buffer_pos, configuration._buffer_max_length);

        TJHelper::add_string_to_string(configuration._key_separator, configuration._buffer, configuration._buffer_pos, configuration._buffer_max_length);

        member->value()->internal_dump(configuration, inner_current_indent);

        // don't add on the last item
        if (--number_of_items > 0)
        {
          TJHelper::add_string_to_string(configuration._item_separator, configuration._buffer, configuration._buffer_pos, configuration._buffer_max_length);
        }
        if (configuration._formating == formating::indented)
        {
          TJHelper::add_char_to_string('\n', configuration._buffer, configuration._buffer_pos, configuration._buffer_max_length);
        }
      }
      delete[] inner_current_indent;
    }
    // close it.
    TJHelper::add_string_to_string(current_indent, configuration._buffer, configuration._buffer_pos, configuration._buffer_max_length);
    TJHelper::add_char_to_string('}', configuration._buffer, configuration._buffer_pos, configuration._buffer_max_length);
  }

  bool TJValueObject::is_object() const
  {
    return true;
  }

  int TJValueObject::get_number_of_items() const
  {
    return _members == nullptr ? 0 : _members->size();
  }

  TJMember* TJValueObject::operator [](int idx) const
  {
    return at(idx);
  }

  TJMember* TJValueObject::at(int idx) const
  {
    if(idx >= get_number_of_items() || idx < 0)
    {
      return nullptr;
    }
    return (*_members)[idx];
  }

  void TJValueObject::free_members()
  {
    if (_members == nullptr)
    {
      return;
    }

    for (auto var : *_members)
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
    if (nullptr == value)
    {
      return nullptr;
    }

    delete [] value->_last_dump;
    internal_dump_configuration configuration(formating::none, nullptr, nullptr, nullptr, nullptr, nullptr);
    value->internal_dump(configuration, nullptr);
    value->_last_dump = configuration._buffer;

    return value->_last_dump;
  }

  ///////////////////////////////////////
  /// TJValueArray
  TJValueArray::TJValueArray() :
    _values(nullptr)
  {
  }

  TJValueArray::~TJValueArray()
  {
    free_values();
  }

  TJValueArray* TJValueArray::move(std::vector<TJValue*>*& values)
  {
    auto value = new TJValueArray();
    value->_values = values;
    values = nullptr;
    return value;
  }

  void TJValueArray::internal_dump(internal_dump_configuration& configuration, const char* current_indent) const
  {
    // open it
    TJHelper::add_char_to_string('[', configuration._buffer, configuration._buffer_pos, configuration._buffer_max_length);

    auto number_of_items = get_number_of_items();
    if (number_of_items > 0)
    {
      // only return if we have data.
      if (configuration._formating == formating::indented)
      {
        TJHelper::add_char_to_string('\n', configuration._buffer, configuration._buffer_pos, configuration._buffer_max_length);
      }

      int inner_buffer_pos = 0;
      int inner_buffer_max_length = 0;
      char* inner_current_indent = nullptr;
      
      TJHelper::add_string_to_string(current_indent, inner_current_indent, inner_buffer_pos, inner_buffer_max_length);
      TJHelper::add_string_to_string(configuration._indent, inner_current_indent, inner_buffer_pos, inner_buffer_max_length);

      for (const auto& value : *_values)
      {
        TJHelper::add_string_to_string(inner_current_indent, configuration._buffer, configuration._buffer_pos, configuration._buffer_max_length);
        value->internal_dump(configuration, inner_current_indent);

        // don't add on the last item
        if (--number_of_items > 0)
        {
          TJHelper::add_string_to_string(configuration._item_separator, configuration._buffer, configuration._buffer_pos, configuration._buffer_max_length);
        }
        if (configuration._formating == formating::indented)
        {
          TJHelper::add_char_to_string('\n', configuration._buffer, configuration._buffer_pos, configuration._buffer_max_length);
        }
      }
      delete[] inner_current_indent;
    }
    // close it.
    TJHelper::add_string_to_string(current_indent, configuration._buffer, configuration._buffer_pos, configuration._buffer_max_length);
    TJHelper::add_char_to_string(']', configuration._buffer, configuration._buffer_pos, configuration._buffer_max_length);
  }

  TJValue* TJValueArray::clone() const
  {
    auto array = new TJValueArray();
    if (_values != nullptr)
    {
      auto values = new std::vector<TJValue*>();
      for (auto& value : *_values)
      {
        values->push_back(value->clone());
      }
      array->_values = values;
    }
    return array;
  }

  bool TJValueArray::is_array() const
  {
    return true;
  }

  int TJValueArray::get_number_of_items() const
  {
    return _values == nullptr ? 0 : _values->size();
  }

  TJValue* TJValueArray::operator [](int idx) const
  {
    return at(idx);
  }

  TJValue* TJValueArray::at(int idx) const
  {
    if (idx >= get_number_of_items() || idx < 0)
    {
      return nullptr;
    }
    return (*_values)[idx];
  }

  void TJValueArray::free_values()
  {
    if (_values == nullptr)
    {
      return;
    }

    for (auto var : *_values)
    {
      delete var;
    }
    delete _values;
    _values = nullptr;
  }

  ///////////////////////////////////////
  /// TJValue Number
  TJValueNumber::TJValueNumber(const bool is_negative) : 
    _is_negative(is_negative)
  {
  }

  bool TJValueNumber::is_number() const
  {
    return true;
  }

  ///////////////////////////////////////
  /// TJValue whole Number
  TJValueNumberInt::TJValueNumberInt(const unsigned long long& number, const bool is_negative) :
    TJValueNumber(is_negative),
    _number(number)
  {
  }

  TJValueNumberInt::TJValueNumberInt(const long long& number) :
    TJValueNumber(number<0),
    _number(number < 0 ? -1*number : number)
  {
  }

  TJValue* TJValueNumberInt::clone() const
  {
    return new TJValueNumberInt(_number, _is_negative);
  }

  void TJValueNumberInt::internal_dump(internal_dump_configuration& configuration, const char* current_indent) const
  {
    auto string = new char[255];

    // if we have no fraction, then just return it.
    std::sprintf(string, _is_negative ? "-%lld" : "%lld", _number);

    // then the number
    TJHelper::add_string_to_string(string, configuration._buffer, configuration._buffer_pos, configuration._buffer_max_length);

    delete[] string;
  }

  long long TJValueNumberInt::get_number() const
  {
    return _is_negative ? -1* _number : _number;
  }

  ///////////////////////////////////////
  /// TJValue float Number
  TJValueNumberFloat::TJValueNumberFloat(const unsigned long long& number, const unsigned long long& fraction, const unsigned int& fraction_exponent, bool is_negative) :
    _string(nullptr),
    TJValueNumber(is_negative),
    _number(number),
    _fraction(fraction),
    _fraction_exponent(fraction_exponent)
  {
  }

  TJValueNumberFloat::~TJValueNumberFloat()
  {
    if (nullptr != _string)
    {
      delete[] _string;
    }
  }

  void TJValueNumberFloat::make_string_if_needed() const
  {
    if (nullptr != _string)
    {
      return;
    }

    _string = new char[1024];
    auto format_string = new char[1024];

    // if we have no fraction, then just return it.
    auto zeros = static_cast<unsigned long long>(_fraction_exponent);
    std::sprintf(format_string, "%s%%llu.%%0%ullu", _is_negative ? "-" : "", _fraction_exponent);
    std::sprintf(_string, format_string, _number, _fraction);

    delete[] format_string;
  }

  TJValue* TJValueNumberFloat::clone() const
  {
    return new TJValueNumberFloat(_number, _fraction, _fraction_exponent, _is_negative);
  }

  void TJValueNumberFloat::internal_dump(internal_dump_configuration& configuration, const char* current_indent) const
  {
    // make sthe string is needed
    make_string_if_needed();

    // then the number
    TJHelper::add_string_to_string(_string, configuration._buffer, configuration._buffer_pos, configuration._buffer_max_length);
  }

  long double TJValueNumberFloat::get_number() const
  {
    if (_fraction == 0) {
      return static_cast<long double>(_number);
    }

    // Convert b to its fractional form
    const auto& pow = std::powl(10, _fraction_exponent);
    const auto& whole_number = _number * pow + _fraction;

    // Combine the number and the fraction
    return (_is_negative ? -1 : 1) * (whole_number / pow);
  }

  ///////////////////////////////////////
  /// TJValue float Number
  TJValueNumberExponent::TJValueNumberExponent(const unsigned long long& number, const unsigned long long& fraction, const unsigned int& fraction_exponent, const int& exponent, bool is_negative) :
    TJValueNumber(is_negative),
    _string(nullptr),
    _number(number),
    _fraction(fraction),
    _fraction_exponent(fraction_exponent),
    _exponent(exponent)    
  {
  }

  TJValueNumberExponent::~TJValueNumberExponent()
  {
    if (_string != nullptr)
    {
      delete[] _string;
    }
  }

  TJValue* TJValueNumberExponent::clone() const
  {
    return new TJValueNumberExponent(_number, _fraction, _fraction_exponent, _exponent, _is_negative);
  }

  void TJValueNumberExponent::internal_dump(internal_dump_configuration& configuration, const char* current_indent) const
  {
    // we only create the string value when the caller asks for it.
    // this is to make sure that we do not create it on parsing.
    make_string_if_needed();

    // then the number
    TJHelper::add_string_to_string(_string, configuration._buffer, configuration._buffer_pos, configuration._buffer_max_length);
  }

  void TJValueNumberExponent::make_string_if_needed() const
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
      std::sprintf(_string, _is_negative ? "-%llue+%i" : "%llue+%i", _number, _exponent);
    }
    else
    {
      // rebuild the buffer and make sure that we have all the zeros for the fractions.
      if (_exponent < 0)
      {
        std::sprintf(_string, _is_negative ? "-%llu.%0*llue%i" : "%llu.%0*llue%i", _number, _fraction_exponent, _fraction, _exponent);
      }
      else
      {
        std::sprintf(_string, _is_negative ? "-%llu.%0*llue+%i" : "%llu.%0*llue+%i", _number, _fraction_exponent, _fraction, _exponent);
      }
    }
  }
} // TinyJSON