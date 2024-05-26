#pragma once
// Licensed to Florent Guelfucci under one or more agreements.
// Florent Guelfucci licenses this file to you under the MIT license.
// See the LICENSE file in the project root for more information.

#include <vector>

namespace TinyJSON
{
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
    static TJValue* Parse(const char* src);

  protected:
    static TJValue* start(const char*& p);
  };

  // A TJMember is a key value pair, (name/value), that belong to an object.
  class TJMember
  {
  friend TinyJSON;
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
    static TJValue* try_read_number(const char*& p);
    static TJValue* try_create_number_from_parts(const bool& is_negative, const unsigned long long& unsigned_whole_number, const unsigned long long& unsigned_fraction, const long long& exponent);
    static int get_number_of_digits(const unsigned long long& number);
    static char* try_read_whole_number(const char*& p);
    static char* try_read_whole_number_as_fraction(const char*& p);

    /// <summary>
    /// Try and read a complete object {}
    /// </summary>
    /// <param name="p"></param>
    /// <returns></returns>
    static TJValue* try_read_object(const char*& p);

    /// <summary>
    /// Try and read an object after we have read the openning bracket
    /// This is to prevent having to read the same char more than once.
    /// </summary>
    /// <param name="p"></param>
    /// <returns></returns>
    static TJValue* try_continue_read_object(const char*& p);

    static void free_members(std::vector<TJMember*>* members);

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
    virtual const char* to_string() const = 0;

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
    const char* to_string() const;

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
    const char* to_string() const;
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
    const char* to_string() const;
  };

  // A null JSon
  class TJValueNull : public TJValue
  {
  public:
    TJValueNull();
    virtual ~TJValueNull() = default;

    /// <summary>
    /// Try and get a string representation of the value.
    /// </summary>
    /// <returns></returns>
    const char* to_string() const;
  };

  // A null JSon
  class TJValueObject : public TJValue
  {
  friend TJMember;
  public:
    TJValueObject();
    virtual ~TJValueObject();

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

    /// <summary>
    /// Try and get a string representation of the value.
    /// </summary>
    /// <returns></returns>
    const char* to_string() const;

  protected:
    // protected contructor called internally so we can 
    // pass ownership of the member to this TJValue.
    TJValueObject(std::vector<TJMember*>* members);

  private:
    // All the key value pairs in this object.
    std::vector<TJMember*>* _members;

    void free_members();
  };

  // A number JSon, float or int
  class TJValueNumber : public TJValue
  {
  protected:
    TJValueNumber(const bool is_negative);
    virtual ~TJValueNumber() = default;

  public:
    /// <summary>
    /// Try and get a string representation of the value.
    /// </summary>
    /// <returns></returns>
    const char* to_string() const;

  protected:
    const bool _is_negative;
  };

  // A number JSon, float or int
  class TJValueNumberInt : public TJValueNumber
  {
  public:
    TJValueNumberInt(const unsigned long long& number, const bool is_negative);
    virtual ~TJValueNumberInt() = default;

    const long long get_number() const;

  private:
    const long long _number;
  };

  // A number JSon, float or int
  class TJValueNumberFloat : public TJValueNumber
  {
  public:
    TJValueNumberFloat(const unsigned long long& number, const unsigned long long fraction, bool is_negative);
    virtual ~TJValueNumberFloat() = default;

    const long double get_number() const;

  private:
    const unsigned long long _number;
    const unsigned long long _fraction;
  };
} // TinyJSON