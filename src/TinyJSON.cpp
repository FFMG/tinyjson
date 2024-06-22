// Licensed to Florent Guelfucci under one or more agreements.
// Florent Guelfucci licenses this file to you under the MIT license.
// See the LICENSE file in the project root for more information.
#include "TinyJSON.h"

#include <algorithm>
#include <cmath>
#include <cstring> 
#include <cstdio>
#include <fstream>
#include <iostream>
#include <string> 

static constexpr short TJ_MAX_NUMBER_OF_DIGGITS = 19;
static constexpr short TJ_DEFAULT_STRING_READ_SIZE = 10;

static constexpr TJCHAR TJ_NULL_TERMINATOR = '\0';

static constexpr TJCHAR TJ_UTF8_BOM0 = static_cast<TJCHAR>(0x0EF);
static constexpr TJCHAR TJ_UTF8_BOM1 = static_cast<TJCHAR>(0x0BB);
static constexpr TJCHAR TJ_UTF8_BOM2 = static_cast<TJCHAR>(0x0BF);

static constexpr TJCHAR TJ_ESCAPE_QUOTATION = static_cast<TJCHAR>(0x022);       // % x22 / ; "    quotation mark  U+0022
static constexpr TJCHAR TJ_ESCAPE_REVERSE_SOLIDUS = static_cast<TJCHAR>(0x05C); // % x5C / ; \    reverse solidus U+005C
static constexpr TJCHAR TJ_ESCAPE_SOLIDUS = static_cast<TJCHAR>(0x02F);         // % x2F / ; /    solidus         U+002F
static constexpr TJCHAR TJ_ESCAPE_BACKSPACE = static_cast<TJCHAR>(0x008);       // % x62 / ; b    backspace       U+0008
static constexpr TJCHAR TJ_ESCAPE_FORM_FEED = static_cast<TJCHAR>(0x00C);       // % x66 / ; f    form feed       U+000C
static constexpr TJCHAR TJ_ESCAPE_LINE_FEED = static_cast<TJCHAR>(0x00A);       // % x6E / ; n    line feed       U+000A
static constexpr TJCHAR TJ_ESCAPE_CARRIAGE_RETURN = static_cast<TJCHAR>(0x00D); // % x72 / ; r    carriage return U+000D
static constexpr TJCHAR TJ_ESCAPE_TAB = static_cast<TJCHAR>(0x009);             // % x74 / ; t    tab             U+0009
// static constexpr TJCHAR TJ_ESCAPE_HEXDIG = '\u1234';// % x75 4HEXDIG; uXXXX                U + XXXX

#ifdef _DEBUG
# if defined(_MSC_VER)
#   define TJASSERT(x) for(;;){ if(!x){ __debugbreak();}; break;}
#else
#   include  <assert.h>
#   define TJASSERT(x) for(;;){ assert(x); break;}
# endif
#else
# define TJASSERT(x) for(;;){break;}
#endif

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

#define TJ_CASE_HEX           case '0': \
                              case '1': \
                              case '2': \
                              case '3': \
                              case '4': \
                              case '5': \
                              case '6': \
                              case '7': \
                              case '8': \
                              case '9': \
                              case 'a': \
                              case 'b': \
                              case 'c': \
                              case 'd': \
                              case 'e': \
                              case 'f': \
                              case 'A': \
                              case 'B': \
                              case 'C': \
                              case 'D': \
                              case 'E': \
                              case 'F': 

#define TJ_CASE_SPACE         case ' ':  \
                              case TJ_ESCAPE_TAB: \
                              case TJ_ESCAPE_LINE_FEED: \
                              case TJ_ESCAPE_CARRIAGE_RETURN:

#define TJ_CASE_START_STRING  case TJ_ESCAPE_QUOTATION:

#define TJ_CASE_COMMA         case ',':

#define TJ_CASE_COLON         case ':':

#define TJ_CASE_BEGIN_OBJECT  case '{':

#define TJ_CASE_END_OBJECT    case '}':

#define TJ_CASE_BEGIN_ARRAY   case '[':

#define TJ_CASE_END_ARRAY     case ']':

#define TJ_CASE_MAYBE_ESCAPE  case '\\':

namespace TinyJSON
{
  struct internal_dump_configuration
  {
    TJCHAR* _buffer;
    const formating _formating;
    const TJCHAR* _indent;
    int _buffer_pos;
    int _buffer_max_length;

    const TJCHAR* _item_separator;
    const TJCHAR* _key_separator;
    const TJCHAR* _value_quote;
    const TJCHAR* _key_quote;
    const TJCHAR* _new_line;
    const bool _escape_special_characters;

    internal_dump_configuration(
      formating formating,
      const TJCHAR* indent,
      const TJCHAR* item_separator,
      const TJCHAR* key_separator,
      const TJCHAR* value_quote,
      const TJCHAR* key_quote,
      const TJCHAR* new_line,
      bool escape_special_characters
    ) :
      _formating(formating),
      _indent(indent),
      _item_separator(item_separator),
      _key_separator(key_separator),
      _value_quote(value_quote),
      _key_quote(key_quote),
      _new_line(new_line),
      _escape_special_characters(escape_special_characters)
    {
      _buffer = nullptr;
      _buffer_max_length = _buffer_pos = 0;
    }
  };

  ///////////////////////////////////////
  // Parse result.
  class ParseResult
  {
  public:
    ParseResult(const parse_options& parse_options) :
      _parse_exception_message(nullptr),
      _parse_options(parse_options),
      _depth(0)
    {
    }

    ParseResult(const ParseResult& parse_result) = delete;
    ParseResult& operator=(const ParseResult& parse_result) = delete;

    ~ParseResult()
    {
      free_parse_exception_message();
    }

    void push_depth()
    {
      ++_depth;
    }

    void pop_depth()
    {
      --_depth;
    }

    unsigned int current_depth() const
    {
      return _depth;
    }

    /// <summary>
    /// Assign a parse error message.
    /// </summary>
    /// <param name="parse_exception_message"></param>
    void assign_parse_exception_message(const char* parse_exception_message)
    {
      free_parse_exception_message();
      if (parse_exception_message != nullptr)
      {
        auto length = strlen(parse_exception_message);
        _parse_exception_message = new char[length + 1];
        std::strcpy(_parse_exception_message, parse_exception_message);
      }
    }

    void throw_if_parse_exception()
    {
      if (!_parse_options.throw_exception)
      {
        return;
      }
      if (!has_parse_exception_message())
      {
        return;
      }
      throw TJParseException(_parse_exception_message);
    }

    bool has_parse_exception_message() const
    {
      return _parse_exception_message != nullptr;
    }

    const parse_options& options() const
    {
      return _parse_options;
    }

  protected:
    void free_parse_exception_message() noexcept
    {
      if (_parse_exception_message != nullptr)
      {
        delete[] _parse_exception_message;
        _parse_exception_message = nullptr;
      }
    }

    char* _parse_exception_message;
    const parse_options& _parse_options;
    unsigned int _depth;
  };

  ///////////////////////////////////////
  /// Parsing Exception
  TJParseException::TJParseException(const char* message) :
    _message(nullptr)
  {
    assign_message(message);
  }

  TJParseException::TJParseException(const TJParseException& exception)
    : _message(nullptr)
  {
    *this = exception;
  }

  TJParseException::~TJParseException()
  {
    free_message();
  }

  TJParseException& TJParseException::operator=(const TJParseException& exception)
  {
    if (this != &exception)
    {
      assign_message(exception._message);
    }
    return *this;
  }

  const char* TJParseException::what() const noexcept
  {
    return _message == nullptr ? "Unknown" : _message;
  }

  void TJParseException::assign_message(const char* message)
  {
    free_message();
    if (message != nullptr)
    {
      auto length = strlen(message);
      _message = new char[length + 1];
      std::strcpy(_message, message);
    }
  }

  void TJParseException::free_message() noexcept
  {
    if (_message != nullptr)
    {
      delete[] _message;
      _message = nullptr;
    }
  }

  ///////////////////////////////////////
  /// Protected Helper class
  class TJHelper
  {
    friend TinyJSON;
    friend TJMember;
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
    // Function to multiply an unsigned integer by 10 using bit-shifting
    static unsigned long long fast_multiply_by_10(unsigned long long number) 
    {
      return (number << 3) + (number << 1);
    }

    static unsigned long long fast_power_of_10(unsigned int exponent)
    {
      if (exponent == 0)
      {
        return 1;
      }

      unsigned long long base = 10;
      for (unsigned int i = 1; i < exponent; ++i)
      {
        base = fast_multiply_by_10(base);
      }
      return base;
    }

    /// <summary>
    /// Raise a number to the power of 16
    /// </summary>
    /// <param name="number"></param>
    /// <param name="exponent"></param>
    /// <returns></returns>
    static unsigned long long fast_power_of_16(unsigned int exponent)
    {
      if (exponent == 0)
      {
        return 1;
      }

      unsigned long long base = 16;
      for (unsigned int i = 1; i < exponent; ++i)
      {
        base = base << 4;
      }
      return base;
    }

    /// <summary>
    /// Get the length of a string.
    /// </summary>
    /// <param name="source"></param>
    /// <returns></returns>
    static unsigned int string_length(const TJCHAR* source)
    {
      if (nullptr == source)
      {
        return 0;
      }

      for (auto i = 0;; ++i)
      {
        if (source[i] == TJ_NULL_TERMINATOR)
        {
          return i;
        }
      }
      return 0;
    }

    /// <summary>
    /// Quickly convert an +ve integer to a string then add -ve if we want.
    /// we also add leading zeros if needed.
    /// </summary>
    /// <param name="number"></param>
    /// <param name="fraction_exponent"></param>
    /// <param name="is_negative"></param>
    /// <param name="length"></param>
    /// <returns></returns>
    static TJCHAR* fast_number_to_string(unsigned long long number, unsigned int fraction_exponent, bool is_negative, unsigned int& length, bool force_positive_sign = false)
    {
      TJCHAR reverse_buffer[255];
      unsigned reverse_position = 0;
      length = 0;

      if (number == 0)
      {
        reverse_buffer[reverse_position++] = '0';
      }
      else
      {
        while (number > 0)
        {
          auto mod = number % 10;
          switch (mod)
          {
          case 0:
            reverse_buffer[reverse_position++] = '0';
            break;

          case 1:
            reverse_buffer[reverse_position++] = '1';
            break;

          case 2:
            reverse_buffer[reverse_position++] = '2';
            break;
          case 3:
            reverse_buffer[reverse_position++] = '3';
            break;
          case 4:
            reverse_buffer[reverse_position++] = '4';
            break;
          case 5:
            reverse_buffer[reverse_position++] = '5';
            break;
          case 6:
            reverse_buffer[reverse_position++] = '6';
            break;
          case 7:
            reverse_buffer[reverse_position++] = '7';
            break;
          case 8:
            reverse_buffer[reverse_position++] = '8';
            break;
          case 9:
            reverse_buffer[reverse_position++] = '9';
            break;

          default:
            break;
          }
          number /= 10;
        }
      }

      if (fraction_exponent > reverse_position)
      {
        const unsigned int& zeros = fraction_exponent - reverse_position;
        for (unsigned int j = 0; j < zeros; ++j)
        {
          reverse_buffer[reverse_position++] = '0';
        }
      }
      if (is_negative)
      {
        reverse_buffer[reverse_position++] = '-';
      }
      else if (force_positive_sign)
      {
        reverse_buffer[reverse_position++] = '+';
      }

      TJCHAR* buffer = new TJCHAR[reverse_position+1];
      buffer[reverse_position] = TJ_NULL_TERMINATOR;
      for (unsigned int i = 0; i < reverse_position; ++i)
      {
        buffer[reverse_position -1 - i] = reverse_buffer[i];
      }

      length = reverse_position;
      return buffer;
    }

    static TJCHAR* fast_number_to_string(unsigned long long number, unsigned int fraction_exponent, bool is_negative, bool force_positive_sign = false)
    {
      unsigned int ignore = 0;
      return fast_number_to_string(number, fraction_exponent, is_negative, ignore, force_positive_sign);
    }

    /// <summary>
    /// 'join' a whole number together with a fraction
    /// If the number is -12.0045 then
    ///   - the number is 12
    ///   - the fraction is 45
    ///   - the exponent is 4
    ///   - and it is negative.
    /// </summary>
    /// <param name="number">The number we are creating</param>
    /// <param name="fraction">The whole number part of the fraction</param>
    /// <param name="fraction_exponent">The length of the fraction, needed for leading zeros.</param>
    /// <param name="is_negative">If the number is negative or not.</param>
    /// <returns></returns>
    static TJCHAR* fast_number_and_fraction_to_string(unsigned long long number, unsigned long long fraction, unsigned int fraction_exponent, bool is_negative)
    {
      // format the number and fraction separately
      unsigned int length_of_number, length_of_fraction;

      // the number has negative sign in front.
      auto string_number = fast_number_to_string(number, 0, is_negative, length_of_number);

      // the fraction does not have a negative sign
      auto string_fraction = fast_number_to_string(fraction, fraction_exponent, false, length_of_fraction);

      // calculate the total length, we add +1 for the '.' and +1 for the null terminator
      int total_length  = length_of_number + length_of_fraction + 1 + 1;
      int final_string_pos = 0;

      // recreate the final string
      TJCHAR* final_string = new TJCHAR[total_length];
      add_string_to_string(string_number, final_string, final_string_pos, total_length);
      add_char_to_string('.', final_string, final_string_pos, total_length);
      add_string_to_string(string_fraction, final_string, final_string_pos, total_length);

      // cleanup the number and fraction.
      delete[] string_number;
      delete[] string_fraction;
      return final_string;
    }

    /// <summary>
    /// 'join' a whole number together with a fraction and add the exponent as well if needed.
    /// If the number is -12.0045e+12 then
    ///   - the number is 12
    ///   - the fraction is 45
    ///   - the fraction_exponent is 4
    ///   - the exponent is 12
    ///   - and the number is negative.
    /// </summary>
    /// <param name="number"></param>
    /// <param name="fraction"></param>
    /// <param name="fraction_exponent"></param>
    /// <param name="exponent"></param>
    /// <param name="is_negative"></param>
    /// <returns></returns>
    static TJCHAR* fast_number_fraction_and_exponent_to_string(unsigned long long number, unsigned long long fraction, unsigned int fraction_exponent, int exponent, bool is_negative)
    {
      // format the number and fraction separately
      unsigned int length_of_number = 0, length_of_fraction = 0, length_of_exponent = 0;

      // the number has negative sign in front.
      auto string_number = fast_number_to_string(number, 0, is_negative, length_of_number);

      // the fraction does not have a negative sign
      TJCHAR* string_fraction = nullptr;
      if (fraction > 0)
      {
        string_fraction = fast_number_to_string(fraction, fraction_exponent, false, length_of_fraction);
      }

      // the fraction does not have a negative sign
      TJCHAR* string_exponent = nullptr;
      if (exponent < 0)
      {
        string_exponent = fast_number_to_string(-1*exponent, 0, true, length_of_exponent);
      }
      else
      {
        string_exponent = fast_number_to_string(exponent, 0, false, length_of_exponent, true);
      }

      // calculate the total length, 
      //   - +1 for the '.' (if needed)
      //   - +1 for the null terminator
      //   - +1 for the 'e' (if needed)
      int total_length = length_of_number + length_of_fraction + length_of_exponent +1 + 1 + 1;
      int final_string_pos = 0;

      // recreate the final string
      TJCHAR* final_string = new TJCHAR[total_length];
      add_string_to_string(string_number, final_string, final_string_pos, total_length);
      if (nullptr != string_fraction)
      {
        add_char_to_string('.', final_string, final_string_pos, total_length);
        add_string_to_string(string_fraction, final_string, final_string_pos, total_length);
      }
      if (nullptr != string_exponent)
      {
        add_char_to_string('e', final_string, final_string_pos, total_length);
        add_string_to_string(string_exponent, final_string, final_string_pos, total_length);
      }

      // cleanup the number and fraction.
      delete[] string_number;
      delete[] string_fraction;
      delete[] string_exponent;
      return final_string;
    }

    /// <summary>
    /// Try and convert a hex string to a number.
    /// If the number returned is -1 then we can assume an error.
    /// This method does not allow, (or expect), 0x at the begining
    /// and does not case about a leading 0
    /// If you pas 123 then we will assume that it is hex and return 291
    /// If you pass 0123 then we will still return 291
    /// </summary>
    /// <param name="source"></param>
    /// <returns></returns>
    static long long fast_hex_to_decimal(const TJCHAR* source)
    {
      long long decimal = 0ll;
      auto length = string_length(source);
      auto power = 0;
      for(int i = length-1; i >= 0; --i)
      {
        unsigned int number = 0;
        switch (source[i])
        {
        case '0':
          number = 0;
          break;

        case '1':
          number = 1;
          break;

        case '2':
          number = 2;
          break;

        case '3':
          number = 3;
          break;

        case '4':
          number = 4;
          break;

        case '5':
          number = 5;
          break;

        case '6':
          number = 6;
          break;

        case '7':
          number = 7;
          break;

        case '8':
          number = 8;
          break;

        case '9':
          number = 9;
          break;

        case 'A':
        case 'a':
          number = 10;
          break;

        case 'B':
        case 'b':
          number = 11;
          break;

        case 'C':
        case 'c':
          number = 12;
          break;

        case 'D':
        case 'd':
          number = 13;
          break;

        case 'E':
        case 'e':
          number = 14;
          break;

        case 'F':
        case 'f':
          number = 15;
          break;

        default:
          // this number is not an ex
          return -1;
        }
        if(number > 0 )
        {
          decimal = decimal + (number * fast_power_of_16(power++));
        }
      }
      return decimal;
    }

    /// <summary>
    /// Copy the source string to the destination string
    /// </summary>
    /// <param name="source"></param>
    /// <param name="destination"></param>
    /// <param name="max_length"></param>
    /// <returns></returns>
    static void copy_string(const TJCHAR* source, TJCHAR* destination, unsigned int max_length)
    {
      if (nullptr == source)
      {
        if (destination != nullptr && max_length >= 1)
        {
          destination[0] = TJ_NULL_TERMINATOR;
        }
        return;
      }
      for (unsigned int i = 0;; ++i)
      {
        if (source[i] == TJ_NULL_TERMINATOR)
        {
          destination[i] = TJ_NULL_TERMINATOR;
          return;
        }
        if (i > max_length)
        {
          return;
        }
        if (i == max_length)
        {
          // the caller must leave space for the null terminator
          // so we assume that the actual buffer size if max_length+1
          destination[i] = TJ_NULL_TERMINATOR;
          return;
        }
        destination[i] = source[i];
      }
    }

    /// <summary>
    /// Compare if 2 strings are the same
    /// </summary>
    /// <param name="lhs"></param>
    /// <param name="rhs"></param>
    /// <returns></returns>
    static bool are_same(const TJCHAR* lhs, const TJCHAR* rhs)
    {
      if (nullptr == lhs && nullptr == rhs)
      {
        return true;
      }
      else if (nullptr == lhs || nullptr == rhs)
      {
        return false;
      }

      for (auto i = 0;; ++i)
      {
        if (lhs[i] == TJ_NULL_TERMINATOR && rhs[i] == TJ_NULL_TERMINATOR)
        {
          return true;
        }
        else if (lhs[i] == TJ_NULL_TERMINATOR || rhs[i] == TJ_NULL_TERMINATOR)
        {
          return false;
        }
        if (lhs[i] != rhs[i])
        {
          return false;
        }
      }
      return true;
    }

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

    static TJCHAR* resize_string(TJCHAR*& source, int length, int resize_length)
    {
      TJCHAR* new_string = new TJCHAR[resize_length];
      memset(new_string, TJ_NULL_TERMINATOR, sizeof(TJCHAR) * resize_length);
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

    static void add_char_to_string(const TJCHAR char_to_add, TJCHAR*& buffer, int& buffer_pos, int& buffer_max_length) noexcept
    {
      if (buffer_pos + 1 >= buffer_max_length)
      {
        buffer = resize_string(buffer, buffer_max_length, buffer_max_length + TJ_DEFAULT_STRING_READ_SIZE);
        buffer_max_length += TJ_DEFAULT_STRING_READ_SIZE;
      }
      buffer[buffer_pos] = char_to_add;
      buffer[buffer_pos+1] = TJ_NULL_TERMINATOR;
      ++buffer_pos;
    }

    static void add_string_to_string(const TJCHAR* string_to_add, TJCHAR*& buffer, int& buffer_pos, int& buffer_max_length) noexcept
    {
      if (nullptr == string_to_add)
      {
        return;
      }
      if (nullptr == buffer)
      {
        buffer = resize_string(buffer, buffer_max_length, buffer_max_length + TJ_DEFAULT_STRING_READ_SIZE);
      }
      while (*string_to_add != TJ_NULL_TERMINATOR)
      {
        add_char_to_string(*string_to_add, buffer, buffer_pos, buffer_max_length);
        string_to_add++;
      }
    }

    static bool try_add_char_to_string_after_escape(const TJCHAR*& source, TJCHAR*& result, int& result_pos, int& result_max_length)
    {
      const auto& next_char = *(source + 1);
      if (next_char == TJ_NULL_TERMINATOR)
      {
        return false;
      }

      // as per RFC8259
      switch (next_char)
      {
      case TJ_ESCAPE_QUOTATION:       // "    quotation mark  U+0022
      case TJ_ESCAPE_SOLIDUS:         // /    solidus         U+002F 
      case TJ_ESCAPE_REVERSE_SOLIDUS: // \    reverse solidus U+005C
        // skip the escpape and keep the character
        source++;
        add_char_to_string(next_char, result, result_pos, result_max_length);
        return true;

      case'f':  // f    form feed       U+000C
        source++;
        add_char_to_string(TJ_ESCAPE_FORM_FEED, result, result_pos, result_max_length);
        return true;

      case 'b': // b    backspace       U+0008
        source++;
        add_char_to_string(TJ_ESCAPE_BACKSPACE, result, result_pos, result_max_length);
        return true;

      case 'n': // n    line feed       U+000A
        source++;
        add_char_to_string(TJ_ESCAPE_LINE_FEED, result, result_pos, result_max_length);
        return true;

      case 'r': // r    carriage return U+000D
        source++;
        add_char_to_string(TJ_ESCAPE_CARRIAGE_RETURN, result, result_pos, result_max_length);
        return true;

      case 't': // t    tab             U+0009
        source++;
        add_char_to_string(TJ_ESCAPE_TAB, result, result_pos, result_max_length);
        return true;

      case 'u': // /uxxxx escape
        {
          // this is the worse case scenario .. we now have to try read the next 4 characters
          // U+0000 through U+FFFF
          TJCHAR* hex = nullptr;
          int buffer_pos = 0;
          int buffer_max_length = 0;
          for (auto i = 0; i < 4; ++i)
          {
            const auto& possible_hex_char = *(source + i + 2);  //  we add two for '/u'
            switch (possible_hex_char)
            {
            TJ_CASE_HEX
              add_char_to_string(possible_hex_char, hex, buffer_pos, buffer_max_length);
              break;

            default:
            case TJ_NULL_TERMINATOR:
                //  not sure what this is, but it is not valid.
                delete[] hex;
                return false;
            }
          }

          auto decimal = fast_hex_to_decimal(hex);
          delete[] hex;
          if (decimal < 0)
          {
            return false; //  not sure what this is.
          }
#if TJ_USE_CHAR == 1
          if (decimal <= 0x7F) 
          {
            // 1-byte UTF-8 (ASCII)
            add_char_to_string(static_cast<char>(decimal), result, result_pos, result_max_length);
          }
          else if (decimal <= 0x7FF) 
          {
            // 2-byte UTF-8
            add_char_to_string(static_cast<char>(0xC0 | (decimal >> 6)), result, result_pos, result_max_length);
            add_char_to_string(static_cast<char>(0x80 | (decimal & 0x3F)), result, result_pos, result_max_length);
          }
          else if (decimal <= 0xFFFF)
          {
            // 3-byte UTF-8
            add_char_to_string(static_cast<char>(0xE0 | (decimal >> 12)), result, result_pos, result_max_length);
            add_char_to_string(static_cast<char>(0x80 | ((decimal >> 6) & 0x3F)), result, result_pos, result_max_length);
            add_char_to_string(static_cast<char>(0x80 | (decimal & 0x3F)), result, result_pos, result_max_length);
          }
#else
          add_char_to_string(static_cast<TJCHAR>(decimal), result, result_pos, result_max_length);
#endif
          source += 6;  //  the full \uXXXX = 6 char
          return true;
        }
        return false;
      }

      //  this is not an escaped character, just a single reverse solidus
      return false;
    }

    static TJCHAR* try_continue_read_string(const TJCHAR*& p, ParseResult& parse_result)
    {
      int result_pos = 0;
      int result_max_length = 0;
      TJCHAR* result = nullptr;
      while (*p != TJ_NULL_TERMINATOR)
      {
        switch (*p)
        {
        TJ_CASE_SPACE
          //  only read if we started.
          switch (*p)
          {
          case TJ_ESCAPE_LINE_FEED:       // % x6E / ; n    line feed       U + 000A
          case TJ_ESCAPE_CARRIAGE_RETURN: // % x72 / ; r    carriage return U + 000D
          case  TJ_ESCAPE_TAB:            // % x74 / ; t    tab             U + 0009
            // ERROR: invalid character inside the string.
            delete[] result;
            parse_result.assign_parse_exception_message("Invalid character inside the string.");
            return nullptr;
          }
          add_char_to_string(*p, result, result_pos, result_max_length);
          p++;
          break;

        TJ_CASE_MAYBE_ESCAPE
          if (!try_add_char_to_string_after_escape(p, result, result_pos, result_max_length))
          {
            delete[] result;
            // ERROR: invalid/unknown character after single reverse solidus.
            parse_result.assign_parse_exception_message("Invalid/unknown character after single reverse solidus.");
            return nullptr;
          }
          p++;
          break;

        case TJ_ESCAPE_SOLIDUS:         // % x2F / ; /    solidus         U + 002F
          // solidus can be escaped ... and not escaped...
          // this is a case where it is not escaped.
          add_char_to_string(*p, result, result_pos, result_max_length);
          p++;
          break;

        case TJ_ESCAPE_BACKSPACE:       // % x62 / ; b    backspace       U + 0008
        case TJ_ESCAPE_FORM_FEED:       // % x66 / ; f    form feed       U + 000C
          delete[] result;
          // ERROR: invalid character inside the string.
          parse_result.assign_parse_exception_message("Invalid character inside the string..");
          return nullptr;

        TJ_CASE_START_STRING
          p++;

          // Allocate memory for the result string
          // Null-terminate the string
          add_char_to_string(TJ_NULL_TERMINATOR, result, result_pos, result_max_length);
          return result;

        default:
          // we are still in the string, then we are good.
          add_char_to_string(*p, result, result_pos, result_max_length);
          p++;
          break;
        }
      }

      // // ERROR: we could not close the object.
      delete[] result;
      parse_result.assign_parse_exception_message("We could not close the string.");
      return nullptr;
    }

    static bool try_skip_colon(const TJCHAR*& p)
    {
      while (*p != TJ_NULL_TERMINATOR)
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

    static TJValue* try_continue_read_true(const TJCHAR*& p)
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

    static TJValue* try_continue_read_false(const TJCHAR*& p)
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

    static TJValue* try_continue_read_null(const TJCHAR*& p)
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
      const auto muliplier = fast_power_of_10(exponent);
      return source * muliplier;
    }

    static unsigned long long shift_number_right(const unsigned long long source, const unsigned long long exponent, unsigned long long& shifted_source)
    {
      const auto divider = fast_power_of_10(exponent);
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
      // the length can never be more than the fraction exponent.
      const auto& fraction_length = get_number_of_digits(fraction);

      if (fraction_length == fraction_exponent)
      {
        const auto& divider = fast_power_of_10(shitfed_unsigned_fraction_exponent);
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

      auto divider = fast_power_of_10(shitfed_unsigned_fraction_exponent);
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
      shifted_unsigned_fraction = (shifted_unsigned_fraction * fast_power_of_10(shifted_fraction_exponent)) + unsigned_fraction;

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
      shifted_unsigned_fraction = (shifted_unsigned_fraction * fast_power_of_10(shifted_fraction_exponent)) + unsigned_fraction;

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
    static TJCHAR* try_read_whole_number(const TJCHAR*& p, ParseResult& parse_result)
    {
      const TJCHAR* start = nullptr;
      int found_spaces = 0;
      while (*p != TJ_NULL_TERMINATOR)
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
              parse_result.assign_parse_exception_message("Number has a space between it.");
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

    static TJCHAR* read_string(const TJCHAR* start, const TJCHAR* end, int spaces)
    {
      if (nullptr == start)
      {
        // ERROR: unknown character
        return nullptr;
      }
      // Calculate the length of the text inside the quotes
      const auto& length = end - start - spaces;
      // Allocate memory for the result string
      TJCHAR* result = new TJCHAR[length + 1];
      TJHelper::copy_string(start, result, length);
      result[length] = TJ_NULL_TERMINATOR; // Null-terminate the string
      return result;
    }

    static TJCHAR* try_read_whole_number_as_fraction(const TJCHAR*& p, ParseResult& parse_result)
    {
      // try read the number
      auto whole_number = try_read_whole_number(p, parse_result);
      if (nullptr == whole_number)
      {
        parse_result.assign_parse_exception_message("Fraction does not have a number after the token '.'");
        return nullptr;
      }
      // trip the trailling zeros are they are not needed in a fraction
      // but we need to make sure that we have at least one
      // so 42.000000 becomes 42.0
      auto len = TJHelper::string_length(whole_number);
      while (len > 1 && whole_number[len - 1] == '0')
      {
        whole_number[len - 1] = TJ_NULL_TERMINATOR;
        --len;
      }
      return whole_number;
    }

    static unsigned long long fast_string_to_long_long(const TJCHAR* p)
    {
      // the numbers are unsigned and should only contain digits.
      // so we do not have signs or letters to worry about.
      long long result = 0;
      while (*p != TJ_NULL_TERMINATOR)
      {
        TJCHAR c = *p;
        switch (c)
        {
          // it might sound silly to do it that way but it is faster
          // than doing something like result += c - '0'
        case '0':
          result = TJHelper::fast_multiply_by_10(result);
          p++;
          break;

        case '1':
          result = TJHelper::fast_multiply_by_10(result);
          result += 1;
          p++;
          break;

        case '2':
          result = TJHelper::fast_multiply_by_10(result);
          result += 2;
          p++;
          break;

        case '3':
          result = TJHelper::fast_multiply_by_10(result);
          result += 3;
          p++;
          break;

        case '4':
          result = TJHelper::fast_multiply_by_10(result);
          result += 4;
          p++;
          break;

        case '5':
          result = TJHelper::fast_multiply_by_10(result);
          result += 5;
          p++;
          break;

        case '6':
          result = TJHelper::fast_multiply_by_10(result);
          result += 6;
          p++;
          break;

        case '7':
          result = TJHelper::fast_multiply_by_10(result);
          result += 7;
          p++;
          break;

        case '8':
          result = TJHelper::fast_multiply_by_10(result);
          result += 8;
          p++;
          break;

        case '9':
          result = TJHelper::fast_multiply_by_10(result);
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

    static bool has_possible_double_zero(const TJCHAR* p)
    {
      if (p[0] == TJ_NULL_TERMINATOR || p[1] == TJ_NULL_TERMINATOR || p[0] != '0')
      {
        return false;
      }
      // if the number is 0121 then it is wrong
      // but if we have 0.121 then it is valid
      return p[0] == '0' && p[1] != '.';
    }

    /// <summary>
    /// Try and read a number given a string.
    /// </summary>
    /// <param name="p"></param>
    /// <param name="parse_result"></param>
    /// <returns></returns>
    static TJValue* try_read_number(const TJCHAR*& p, ParseResult& parse_result)
    {
      bool is_negative = false;
      if (*p == '-')
      {
        is_negative = true;
        p++;
      }

      // then try and read the digit(s).
      auto possible_number = try_read_whole_number(p, parse_result);
      if (nullptr == possible_number)
      {
        // ERROR: Could not locate the number.
        return nullptr;
      }

      if (has_possible_double_zero(possible_number))
      {
        // ERROR: Numbers cannot have leading zeros
        parse_result.assign_parse_exception_message("Numbers cannot have leading zeros.");
        delete[] possible_number;
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
        const auto& possible_fraction_number = try_read_whole_number_as_fraction(p, parse_result);
        if (nullptr == possible_fraction_number)
        {
          // ERROR: we cannot have a number like '-12.' or '42.
          return nullptr;
        }

        // so 001 become exponent = 3
        fraction_exponent = string_length(possible_fraction_number);
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
        const auto& possible_exponent = try_read_whole_number(p, parse_result);
        if (nullptr == possible_exponent)
        {
          // ERROR: we cannot have a number like '-12e' or '42e
          parse_result.assign_parse_exception_message("Number has exponent 'e' or 'E' but does not have a number.");
          return nullptr;
        }

        const auto& unsigned_exponent = fast_string_to_long_long(possible_exponent);
        delete[] possible_exponent;

        // as per the spec it is allowed to have 1e00
        // https://github.com/FFMG/tinyjson/issues/14
        if (0 == unsigned_exponent)
        {
          exponent = 0;
          is_negative_exponent = false;
        }
        else
        {
          exponent = is_negative_exponent ? unsigned_exponent * -1 : unsigned_exponent;
        }
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
            return TJHelper::are_same( elem->name(), member->name());
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
    static TJValue* try_continue_read_object(const TJCHAR*& p, ParseResult& parse_result)
    {
      if (parse_result.current_depth() >= parse_result.options().max_depth)
      {
        // Error: We reached the max depth
        parse_result.assign_parse_exception_message("Reached the max parse depth (object).");
        return nullptr;
      }
      //  assume no members in that object.
      bool found_comma = false;
      std::vector<TJMember*>* members = nullptr;
      bool after_string = false;
      bool waiting_for_a_string = false;
      while (*p != TJ_NULL_TERMINATOR)
      {
        TJCHAR c = *p;
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
            parse_result.assign_parse_exception_message("Unexpected end of object, there was a ', ' after the last string.");
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
            parse_result.assign_parse_exception_message("Expected a comma after the last element.");
            return nullptr;
          }

          // read the actual string and value
          // that's the way it has to be.
          auto member = try_read_string_and_value(p, parse_result);
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
            parse_result.assign_parse_exception_message("Found a comma out of order.");
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
          parse_result.assign_parse_exception_message("Unknown character.");
          return nullptr;
        }
      }

      // ERROR end of the string was found and we didn't find what we needed.
      free_members(members);
      parse_result.assign_parse_exception_message("End of the string was found and we didn't find what we needed.");
      return nullptr;
    }

    /// <summary>
    /// Try and read an array after we have read the opening bracket.
    /// This is to prevent having to read the same char more than once.
    /// </summary>
    /// <param name="p"></param>
    /// <returns></returns>
    static TJValue* try_continue_read_array(const TJCHAR*& p, ParseResult& parse_result)
    {
      if (parse_result.current_depth() >= parse_result.options().max_depth)
      {
        // Error: We reached the max depth
        parse_result.assign_parse_exception_message("Reached the max parse depth (array).");
        return nullptr;
      }

      //  assume no values in that array
      std::vector<TJValue*>* values = nullptr;
      bool waiting_for_a_value = true;
      bool found_comma = false;
      while (*p != TJ_NULL_TERMINATOR)
      {
        TJCHAR c = *p;
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
            parse_result.assign_parse_exception_message("Unexpected end of array, there was a ', ' after the last string.");
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
            parse_result.assign_parse_exception_message("Found a comma out of order, (2 commas).");
            return nullptr;
          }
          // we are now waiting for a value
          waiting_for_a_value = true;
          found_comma = true;
          p++;
          break;

        default:
          const auto& value = try_read_Value(p, parse_result);
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
            parse_result.assign_parse_exception_message("We found a value but we expected a comma.");
            return nullptr;
          }
          values->push_back(value);
          waiting_for_a_value = false;
          found_comma = false;
          break;
        }
      }

      // ERROR: end of the string was found and we didn't find what we needed.
      free_values(values);
      parse_result.assign_parse_exception_message("End of the string was found and we didn't find what we needed.");
      return nullptr;
    }

    static TJValue* try_read_Value(const TJCHAR*& p, ParseResult& parse_result)
    {
      while (*p != TJ_NULL_TERMINATOR)
      {
        TJCHAR c = *p;
        switch (c)
        {
        TJ_CASE_SPACE
          p++;
          break;

        TJ_CASE_START_STRING
        {
          auto string_value = try_continue_read_string(++p, parse_result);
          if (nullptr == string_value)
          {
            //  ERROR: could not read the string properly.
            return nullptr;
          }

          // whave read the string
          // no need to try and move further forward.
          return TJValueString::move(string_value);
        }

        case 't':
          {
            auto true_value = try_continue_read_true(++p);
            if (nullptr == true_value)
            {
              //  ERROR could not read the word 'true'
              parse_result.assign_parse_exception_message("Could not read the word 'true'.");
              return nullptr;
            }
            return true_value;
          }

        case 'f':
        {
          auto false_value = try_continue_read_false(++p);
          if (nullptr == false_value)
          {
            //  ERROR: could not read the word 'false'
            parse_result.assign_parse_exception_message("Could not read the word 'false'.");
            return nullptr;
          }
          return false_value;
        }

        case 'n':
        {
          auto null_value = try_continue_read_null(++p);
          if (nullptr == null_value)
          {
            //  ERROR: could not read the word 'null'
            parse_result.assign_parse_exception_message("Could not read the word 'null'.");
            return nullptr;
          }
          return null_value;
        }

        TJ_CASE_DIGIT
        TJ_CASE_SIGN
        {
          auto number = try_read_number(p, parse_result);
          if (nullptr == number)
          {
            //  ERROR: could not read number
            return nullptr;
          }
          return number;
        }

        TJ_CASE_BEGIN_ARRAY
        {
          // an array within an array
          parse_result.push_depth();
          auto tjvalue_array = try_continue_read_array(++p, parse_result);
          if (tjvalue_array == nullptr)
          {
            // Error: something went wrong reading an array, the error was logged.
            return nullptr;
          }
          parse_result.pop_depth();
          return tjvalue_array;
        }

        TJ_CASE_BEGIN_OBJECT
        {
          // an object within the object
          parse_result.push_depth();
          auto tjvalue_object = try_continue_read_object(++p, parse_result);
          if (tjvalue_object == nullptr)
          {
            // Error: something went wrong reading an object, the error was logged.
            return nullptr;
          }
          parse_result.pop_depth();
          return tjvalue_object;
        }

        default:
          // ERROR: Unexpected Token while trying to read value.
          parse_result.assign_parse_exception_message("Unexpected Token while trying to read value.");
          return nullptr;
        }
      }

      // ERROR: Unable to read a string and/or value
      return nullptr;
    }

    static TJMember* try_read_string_and_value(const TJCHAR*& p, ParseResult& parse_result)
    {
      // first we look for the string, all the elements are supposed to have one.
      auto string_value = try_continue_read_string(++p, parse_result);
      if (string_value == nullptr)
      {
        //  ERROR: could not read the string
        return nullptr;
      }

      // then we look for the colon
      // only white spaces and the colon are allowed here.
      if (!try_skip_colon(p))
      {
        delete[] string_value;
        //  ERROR: could not locate the expected colon after the key value
        parse_result.assign_parse_exception_message("Could not locate the expected colon after the key value.");
        return nullptr;
      }

      auto value = try_read_Value(p, parse_result);
      if (nullptr == value)
      {
        delete[] string_value;
        //  ERROR: Could not read the value, the error was logged.
        return nullptr;
      }
      return TJMember::move(string_value, value);
    }

    /// <summary>
    /// Return true of the content of the given source has a utf-8 bom
    /// </summary>
    /// <param name="source"></param>
    /// <returns></returns>
    static bool has_utf8_bom(const TJCHAR* source)
    {
      if (source == nullptr)
      {
        return false;
      }

      const auto& c0 = *source;
      if (c0 != TJ_UTF8_BOM0)
      {
        return false;
      }
      const auto& c1 = *(source +1);
      if (c1 != TJ_UTF8_BOM1)
      {
        return false;
      }
      const auto& c2 = *(source +2);
      if (c2 != TJ_UTF8_BOM2)
      {
        return false;
      }

      // if we are here, then it is utf-8
      return true;
    }
  };  // Helper class

  ///////////////////////////////////////
  /// TinyJSON
  /// <summary>
  /// Parse a json file
  /// </summary>
  /// <param name="source">The source file we are trying to parse.</param>
  /// <param name="parse_options">The option we want to use when parsing this.</param>
  /// <returns></returns>
  TJValue* TinyJSON::parse_file(const TJCHAR* file_path, const parse_options& parse_options)
  {
    // sanity check
    if (nullptr == file_path)
    {
      return nullptr;
    }

    std::ifstream file(file_path, std::ios::binary | std::ios::ate);
    if (!file.is_open())
    {
      // ERROR: Could not open the file
      return nullptr;
    }

    std::streamsize file_size = file.tellg();
    file.seekg(0, std::ios::beg);

    TJCHAR* buffer = new TJCHAR[file_size+1];
    if (!file.read(buffer, file_size))
    {
      delete[] buffer;
      return nullptr;
    }
    buffer[file_size] = TJ_NULL_TERMINATOR;

    // we can explicitely close the file to free the resources.
    file.close();

    try
    {
      // parse the file.
      auto value = internal_parse(buffer, parse_options);

      // get rid of the buffer
      delete[] buffer;

      // return whatever we managed to read out of the file.
      return value;
    }
    catch (...)
    {
      // get rid of the buffer
      delete[] buffer;

      // rethrow.
      throw;
    }
  }

  /// <summary>
  /// Parse a json string
  /// </summary>
  /// <param name="source">The source we are trying to parse.</param>
  /// <param name="options">The option we want to use when parsing this.</param>
  /// <returns></parse_options>
  TJValue* TinyJSON::parse(const TJCHAR* source, const parse_options& parse_options)
  {
    return internal_parse(source, parse_options);
  }

  /// <summary>
  /// Internal parsing of a json source
  /// We will use the option to throw, (or not).
  /// </summary>
  /// <param name="source"></param>
  /// <param name="parse_options"></param>
  /// <returns></returns>
  TJValue* TinyJSON::internal_parse(const TJCHAR* source, const parse_options& parse_options)
  {
    // sanity check
    if (nullptr == source)
    {
      if (parse_options.throw_exception)
      {
        throw TJParseException("The given source is null.");
      }
      return nullptr;
    }

    // if we have a utf-8 content then we just skip those.
    if (TJHelper::has_utf8_bom(source))
    {
      source += 3;
    }

    ParseResult parse_result(parse_options);

    // we can only have one value and nothing else
    TJValue* value_found = nullptr;
    while (*source != TJ_NULL_TERMINATOR)
    {
      switch (*source)
      {
      TJ_CASE_SPACE
        source++;
        break;

      default:
        if (nullptr != value_found)
        {
          // Error: Unexpected multiple JSON values in root.
          parse_result.assign_parse_exception_message("Unexpected multiple JSON values in root.");
          delete value_found;
          TJASSERT(parse_result.has_parse_exception_message());
          parse_result.throw_if_parse_exception();
          return nullptr;
        }

        // try and look for the value
        value_found = TJHelper::try_read_Value(source, parse_result);
        if (nullptr == value_found)
        {
          // there was an issue trying to parse.
          TJASSERT(parse_result.has_parse_exception_message());
          parse_result.throw_if_parse_exception();
          return nullptr;
        }
        break;
      }
    }

    if (parse_options.specification == parse_options::rfc4627 && !parse_result.has_parse_exception_message())
    {
      if (value_found == nullptr || (!value_found->is_array() && !value_found->is_object()))
      {
        // error: RFC 4627: A JSON text must be either an object or an array.
        parse_result.assign_parse_exception_message("RFC 4627: A JSON text must be either an object or an array.");
        delete value_found;

        // throw if the options want us to, otherwise return null.
        parse_result.throw_if_parse_exception();
        return nullptr;
      }
    }

    // return if we found anything.
    // if we found nothing ... then it is not an error, just an empty string
    parse_result.throw_if_parse_exception();
    return value_found != nullptr ? value_found : new TJValueString(TJCHARPREFIX(""));
  }

  ///////////////////////////////////////
  /// TJMember
  TJMember::TJMember(const TJCHAR* string, const TJValue* value):
    _string(nullptr),
    _value(nullptr)
  {
    if (string != nullptr)
    {
      const auto& length = TJHelper::string_length(string);
      _string = new TJCHAR[length + 1];
      TJHelper::copy_string(string, _string, length);
    }
    if (value != nullptr)
    {
      _value = value->clone();
    }
  }

  TJMember* TJMember::move(TJCHAR*& string, TJValue*& value)
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

  const TJCHAR* TJMember::name() const
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

  const TJCHAR* TJValue::dump(formating formating, const TJCHAR* indent) const
  {
    free_last_dump();
    switch (formating)
    {
    case formating::none:
      {
        internal_dump_configuration configuration(formating, nullptr, 
          TJCHARPREFIX(","), 
          TJCHARPREFIX(":"), 
          TJCHARPREFIX("\""), 
          TJCHARPREFIX("\""), nullptr, true);
        internal_dump(configuration, nullptr);
        _last_dump = configuration._buffer;
      }
      break;
    case formating::indented:
      {
        internal_dump_configuration configuration(formating, indent, 
          TJCHARPREFIX(","), 
          TJCHARPREFIX(": "), 
          TJCHARPREFIX("\""), 
          TJCHARPREFIX("\""), 
          TJCHARPREFIX("\n"), true);
        internal_dump(configuration, nullptr);
        _last_dump = configuration._buffer;
      }
      break;
    }
    return _last_dump;
  }

  const TJCHAR* TJValue::dump_string() const
  {
    free_last_dump();
    internal_dump_configuration configuration(formating::none, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, false);
    internal_dump(configuration, nullptr);
    _last_dump = configuration._buffer;
    return _last_dump;
  }

  ///////////////////////////////////////
  /// TJValue string
  TJValueString::TJValueString(const TJCHAR* value) :
    _value(nullptr)
  {
    if (value != nullptr)
    {
      const auto& length = TJHelper::string_length(value);
      _value = new TJCHAR[length+1];
      TJHelper::copy_string(value, _value, length);
    }
  }

  TJValueString::~TJValueString()
  {
    free_value();
  }

  TJValueString* TJValueString::move(TJCHAR*& value)
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

  void TJValueString::internal_dump(internal_dump_configuration& configuration, const TJCHAR* current_indent) const
  {
    //  unused
    (void)current_indent;

    // add the quote, (if we have one)
    TJHelper::add_string_to_string(configuration._value_quote, configuration._buffer, configuration._buffer_pos, configuration._buffer_max_length);
    if (nullptr == _value)
    {
      TJHelper::add_string_to_string(TJCHARPREFIX(""), configuration._buffer, configuration._buffer_pos, configuration._buffer_max_length);
    }
    else if (configuration._escape_special_characters)
    {
      const TJCHAR* p = _value;
      while (*p != TJ_NULL_TERMINATOR)
      {
        switch (*p)
        {
        case TJ_ESCAPE_QUOTATION: // % x22 / ; "    quotation mark  U+0022
          TJHelper::add_string_to_string(TJCHARPREFIX("\\\""), configuration._buffer, configuration._buffer_pos, configuration._buffer_max_length);
          break;

        case TJ_ESCAPE_REVERSE_SOLIDUS: // % x5C / ; \    reverse solidus U + 005C
          TJHelper::add_string_to_string(TJCHARPREFIX("\\\\"), configuration._buffer, configuration._buffer_pos, configuration._buffer_max_length);
          break;

        case TJ_ESCAPE_SOLIDUS: // % x2F / ; / solidus         U + 002F
          TJHelper::add_string_to_string(TJCHARPREFIX("\\/"), configuration._buffer, configuration._buffer_pos, configuration._buffer_max_length);
          break;

        case TJ_ESCAPE_BACKSPACE: // % x62 / ; b    backspace       U + 0008
          TJHelper::add_string_to_string(TJCHARPREFIX("\\b"), configuration._buffer, configuration._buffer_pos, configuration._buffer_max_length);
          break;

        case TJ_ESCAPE_FORM_FEED: // % x66 / ; f    form feed       U + 000C
          TJHelper::add_string_to_string(TJCHARPREFIX("\\f"), configuration._buffer, configuration._buffer_pos, configuration._buffer_max_length);
          break;

        case TJ_ESCAPE_LINE_FEED:  // % x6E / ; n    line feed       U + 000A
          TJHelper::add_string_to_string(TJCHARPREFIX("\\n"), configuration._buffer, configuration._buffer_pos, configuration._buffer_max_length);
          break;

        case TJ_ESCAPE_CARRIAGE_RETURN:  // % x72 / ; r    carriage return U + 000D
          TJHelper::add_string_to_string(TJCHARPREFIX("\\r"), configuration._buffer, configuration._buffer_pos, configuration._buffer_max_length);
          break;

        case TJ_ESCAPE_TAB:  // % x74 / ; t    tab             U + 0009
          TJHelper::add_string_to_string(TJCHARPREFIX("\\t"), configuration._buffer, configuration._buffer_pos, configuration._buffer_max_length);
          break;

        default:
          TJHelper::add_char_to_string(*p, configuration._buffer, configuration._buffer_pos, configuration._buffer_max_length);
          break;
        }
        ++p;
      }
    }
    else
    {
      TJHelper::add_string_to_string(_value, configuration._buffer, configuration._buffer_pos, configuration._buffer_max_length);
    }

    // then close the quote, (if we have one)
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

  void TJValueBoolean::internal_dump(internal_dump_configuration& configuration, const TJCHAR* current_indent) const
  {
    //  unused
    (void)current_indent;

    // then the word we are after
    TJHelper::add_string_to_string(_is_true ? TJCHARPREFIX("true"): TJCHARPREFIX("false"), configuration._buffer, configuration._buffer_pos, configuration._buffer_max_length);
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

  void TJValueNull::internal_dump(internal_dump_configuration& configuration, const TJCHAR* current_indent) const
  {
    //  unused
    (void)current_indent;

    // then the word we are after
    TJHelper::add_string_to_string(TJCHARPREFIX("null"), configuration._buffer, configuration._buffer_pos, configuration._buffer_max_length);
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

  void TJValueObject::internal_dump(internal_dump_configuration& configuration, const TJCHAR* current_indent) const
  {
    // open it
    TJHelper::add_char_to_string('{', configuration._buffer, configuration._buffer_pos, configuration._buffer_max_length);

    auto number_of_items = get_number_of_items();
    if (number_of_items > 0)
    {
      // only return if we have data.
      if (configuration._formating == formating::indented)
      {
        TJHelper::add_char_to_string(TJ_ESCAPE_LINE_FEED, configuration._buffer, configuration._buffer_pos, configuration._buffer_max_length);
      }

      int inner_buffer_pos = 0;
      int inner_buffer_max_length = 0;
      TJCHAR* inner_current_indent = nullptr;

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
          TJHelper::add_char_to_string(TJ_ESCAPE_LINE_FEED, configuration._buffer, configuration._buffer_pos, configuration._buffer_max_length);
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

  const TJValue* TJValueObject::try_get_value(const TJCHAR* name) const
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
      return TJHelper::are_same(name, value->name());
    });

    return (it == _members->end()) ? nullptr : (*it)->value();
  }

  const TJCHAR* TJValueObject::try_get_string(const TJCHAR* name) const
  {
    auto value = try_get_value(name);
    if (nullptr == value)
    {
      return nullptr;
    }

    delete [] value->_last_dump;
    internal_dump_configuration configuration(formating::none, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, false);
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

  void TJValueArray::internal_dump(internal_dump_configuration& configuration, const TJCHAR* current_indent) const
  {
    // open it
    TJHelper::add_char_to_string('[', configuration._buffer, configuration._buffer_pos, configuration._buffer_max_length);

    auto number_of_items = get_number_of_items();
    if (number_of_items > 0)
    {
      // only return if we have data.
      TJHelper::add_string_to_string(configuration._new_line, configuration._buffer, configuration._buffer_pos, configuration._buffer_max_length);

      int inner_buffer_pos = 0;
      int inner_buffer_max_length = 0;
      TJCHAR* inner_current_indent = nullptr;
      
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
        TJHelper::add_string_to_string(configuration._new_line, configuration._buffer, configuration._buffer_pos, configuration._buffer_max_length);
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

  void TJValueNumberInt::internal_dump(internal_dump_configuration& configuration, const TJCHAR* current_indent) const
  {
    //  unused
    (void)current_indent;

    // if we have no fraction, then just return it.
    auto string = TJHelper::fast_number_to_string(_number, 0, _is_negative );

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
    TJValueNumber(is_negative),
    _string(nullptr),
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

    _string = TJHelper::fast_number_and_fraction_to_string(_number, _fraction, _fraction_exponent, _is_negative);
  }

  TJValue* TJValueNumberFloat::clone() const
  {
    return new TJValueNumberFloat(_number, _fraction, _fraction_exponent, _is_negative);
  }

  void TJValueNumberFloat::internal_dump(internal_dump_configuration& configuration, const TJCHAR* current_indent) const
  {
    // unused
    (void)current_indent;

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
    auto pow = static_cast<long double>(TJHelper::fast_power_of_10(_fraction_exponent));
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

  void TJValueNumberExponent::internal_dump(internal_dump_configuration& configuration, const TJCHAR* current_indent) const
  {
    // unused
    (void)current_indent;

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
    _string = TJHelper::fast_number_fraction_and_exponent_to_string(_number, _fraction, _fraction_exponent, _exponent, _is_negative);
  }
} // TinyJSON