// Licensed to Florent Guelfucci under one or more agreements.
// Florent Guelfucci licenses this file to you under the MIT license.
// See the LICENSE file in the project root for more information.
#include "TinyJSON.h"

#include <algorithm>

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

  TinyJSON* TinyJSON::Parse(const char* src)
  {
    const char* p = src;

    return start(p);
  }

  TinyJSON* TinyJSON::start(const char*& p)
  {
    while (*p != '\0') 
    {
      char c = *p;
      switch (c)
      {
      TJ_CASE_SPACE
        break;

      TJ_CASE_BEGIN_OBJECT
      {
        auto tjobject = new TJObject();
        auto json = tjobject->finish_object(++p);
        if (nullptr == json)
        {
          //  something went wrong, the error was logged.
          delete tjobject;
        }
        return json;
      }

      default:
        break;
      }
      p++;
    }

    // something went wrong.
    return nullptr;
  }

  ///////////////////////////////////////
  /// TJObject
  TJObject::TJObject() : 
    _members(nullptr)
  {
  }

  TJObject::~TJObject()
  {
    free_elements();
  }

  void TJObject::free_elements()
  {
    if (_members != nullptr)
    {
      for each (auto var in *_members)
      {
        delete var;
      }
      delete _members;
      _members = nullptr;
    }
  }

  const TJValue* TJObject::try_get_value(const char* name) const
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

  const char* TJObject::try_get_string(const char* name) const
  {
    auto value = try_get_value(name);
    return (value == nullptr) ? nullptr : value->ToString();
  }

  TJObject* TJObject::finish_object(const char*& p)
  {
    TJObject* object_created = nullptr;
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
          if (nullptr != object_created)
          {
            // ERROR we tried to close the object more than once.
            //       the object is complete so it does not make sense to have this here.
            return nullptr;
          }

          //  we are now done with this object.
          object_created = this;
          
          // but is it what we expected?
          if (waiting_for_a_string)
          {
            // ERROR: unexpected end of object, there was a "," after
            //        the last string and we expected a string now, not a close "}"
            return nullptr;
          }
          p++;
          break;

        TJ_CASE_START_STRING
        {
          // we got our string, no longer waiting for one.
          waiting_for_a_string = false;

          // we are no longer after the string
          after_string = false;

          // read the actual string and value
          // that's the way it has to be.
          auto element = TJMember::try_read_string_and_value(p);
          if (element == nullptr)
          {
            // ERROR: There was an error reading the name and/or the value
            return nullptr;
          }
          if (nullptr == _members)
          {
            _members = new std::vector<TJMember*>();
          }
          _members->push_back(element);
          after_string = true;
        }
        break;

      TJ_CASE_COMMA
        if(!after_string)
        {
          // ERROR: found a comma out of order
          return nullptr;
        }
        // we are no longer after the string
        after_string = false;
        waiting_for_a_string = true;
        p++;
        break;

      default:
        // ERROR: unknown character
        return nullptr;
      }
    }

    // return what we might have found
    return object_created;
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
    _value->ToString();
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

  const char* TJValueString::ToString() const
  {
    return _value;
  }

  ///////////////////////////////////////
  /// TJValue true
  TJValueTrue::TJValueTrue()
  {
  }

  const char* TJValueTrue::ToString() const
  {
    return "true";
  }

  ///////////////////////////////////////
  /// TJValue false
  TJValueFalse::TJValueFalse()
  {
  }

  const char* TJValueFalse::ToString() const
  {
    return "false";
  }

  ///////////////////////////////////////
  /// TJValue null
  TJValueNull::TJValueNull()
  {
  }

  const char* TJValueNull::ToString() const
  {
    return "null";
  }
} // TinyJSON