#pragma once
// Licensed to Florent Guelfucci under one or more agreements.
// Florent Guelfucci licenses this file to you under the MIT license.
// See the LICENSE file in the project root for more information.

#include <vector>

namespace TinyJSON
{
  class TJObject;
  class TJMember;
  class TJValue;
  class TJValueString;
  class TinyJSON
  {
  public:
    TinyJSON();
    virtual ~TinyJSON() = default;

    /// <summary>
    /// Parse a json string
    /// </summary>
    /// <param name="src">The source we are trying to parse.</param>
    /// <returns></returns>
    static TinyJSON* Parse(const char* src);

    /// <summary>
    /// Try and get a string value, if it does not exist, then we return null.
    /// </summary>
    /// <param name="name"></param>
    /// <returns></returns>
    virtual const char* try_get_string(const char* name) const = 0;

    /// <summary>
    /// Try and get the value of this member if it exists.
    /// </summary>
    /// <param name="name"></param>
    /// <returns></returns>
    virtual const TJValue* try_get_value(const char* name) const = 0;

  protected:
    static TinyJSON* start(const char*& p);
  };

  class TJObject : public TinyJSON
  {
  friend TinyJSON;
  public:
    TJObject();
    virtual ~TJObject();

    /// <summary>
    /// Try and get a string value, if it does not exist, then we return null.
    /// </summary>
    /// <param name="name"></param>
    /// <returns></returns>
    const char* try_get_string(const char* name) const;

    /// <summary>
    /// Try and get the value of this member if it exists.
    /// </summary>
    /// <param name="name"></param>
    /// <returns></returns>
    virtual const TJValue* try_get_value(const char* name) const;

  protected:
    TJObject* finish_object(const char*& p);

    // All the key value pairs in the object.
    std::vector<TJMember*>* _members;

  private:
    void free_elements();
  };

  // A TJMember is a key value pair, (name/value), that belong to an object.
  class TJMember
  {
  friend TJObject;
  public:
    TJMember(const char* string, const TJValue* value);
    virtual ~TJMember();

    const char* name() const
    {
      return _string;
    }

    const TJValue* value() const
    {
      return _value;
    }

    /// <summary>
    /// Try and get a string representation of the value.
    /// </summary>
    /// <returns></returns>
    const char* to_string() const;

  protected:
    // create while passing the ownership of the memory.
    TJMember(char* string, TJValue* value);

    static TJMember* try_read_string_and_value(const char*& p);
    static char* try_read_string(const char*& p);
    static bool try_read_true(const char*& p);
    static bool try_read_false(const char*& p);
    static bool try_read_null(const char*& p);

  private:
    char* _string;
    TJValue* _value;
    void free_string();
    void free_value();
  };

  // A simple JSON value
  class TJValue
  {
  public:
    virtual ~TJValue() = default;

    /// <summary>
    /// Try and get a string representation of the value.
    /// </summary>
    /// <returns></returns>
    virtual const char* ToString() const = 0;

  protected:
    TJValue();
  };

  // A string JSon
  class TJValueString : public TJValue
  {
  friend TJMember;
  public:
    TJValueString(const char* value);
    virtual ~TJValueString();

    /// <summary>
    /// Try and get a string representation of the value.
    /// </summary>
    /// <returns></returns>
    const char* ToString() const;

  protected:
    // create while passing the ownership of the memory.
    TJValueString(char* value);

  private:
    char* _value;
    void free_value();
  };

  // A true bolean JSon
  class TJValueTrue : public TJValue
  {
  public:
    TJValueTrue();
    virtual ~TJValueTrue() = default;

    /// <summary>
    /// Try and get a string representation of the value.
    /// </summary>
    /// <returns></returns>
    const char* ToString() const;
  };

  // A true bolean JSon
  class TJValueFalse : public TJValue
  {
  public:
    TJValueFalse();
    virtual ~TJValueFalse() = default;

    /// <summary>
    /// Try and get a string representation of the value.
    /// </summary>
    /// <returns></returns>
    const char* ToString() const;
  };

  // A true bolean JSon
  class TJValueNull : public TJValue
  {
  public:
    TJValueNull();
    virtual ~TJValueNull() = default;

    /// <summary>
    /// Try and get a string representation of the value.
    /// </summary>
    /// <returns></returns>
    const char* ToString() const;
  };
} // TinyJSON