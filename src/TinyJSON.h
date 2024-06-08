#pragma once
// Licensed to Florent Guelfucci under one or more agreements.
// Florent Guelfucci licenses this file to you under the MIT license.
// See the LICENSE file in the project root for more information.

// https://semver.org/
// Semantic Versioning 2.0.0
//   MAJOR version when you make incompatible API changes
//   MINOR version when you add functionality in a backward compatible manner
//   PATCH version when you make backward compatible bug fixes
static const short TJ_VERSION_MAJOR = 0;
static const short TJ_VERSION_MINOR = 0;
static const short TJ_VERSION_PATCH = 1;
static const char TJ_VERSION_STRING[] = "0.0.1";

#include <vector>

namespace TinyJSON
{
  class TJValue;
  class TJHelper;

  // A simple JSON value, the base of all items in a json
  class TJValue
  {
  public:
    virtual ~TJValue() = default;

    /// <summary>
    /// Try and get a string representation of the value.
    /// </summary>
    /// <returns></returns>
    virtual const char* to_string() const = 0;

    virtual bool is_object() const;
    virtual bool is_array() const;
    virtual bool is_string() const;
    virtual bool is_number() const;
    virtual bool is_true() const;
    virtual bool is_false() const;
    virtual bool is_null() const;

  protected:
    TJValue();
  };

  // The parser class
  class TinyJSON
  {
  public:
    virtual ~TinyJSON() = default;

    /// <summary>
    /// Parse a json string
    /// </summary>
    /// <param name="src">The source we are trying to parse.</param>
    /// <returns></returns>
    static TJValue* parse(const char* src);

  protected:
    TinyJSON() = default;

    static TJValue* start(const char*& p);
  };

  // A TJMember is a key value pair, (name/value), that belong to an object.
  class TJMember
  {
    friend TJHelper;
  public:
    TJMember(const char* string, const TJValue* value);
    virtual ~TJMember();

    const char* name() const;
    const TJValue* value() const;

    /// <summary>
    /// Try and get a string representation of the value.
    /// </summary>
    /// <returns></returns>
    const char* to_string() const;

  protected:
    // create while passing the ownership of the memory.
    TJMember(char* string, TJValue* value);

  private:
    char* _string;
    TJValue* _value;
    void free_string();
    void free_value();
  };

  // A Json object that contain an array of key/value pairs.
  class TJValueObject : public TJValue
  {
    friend TJHelper;
  public:
    TJValueObject();
    virtual ~TJValueObject();

    /// <summary>
    /// Get the number of items in this array
    /// </summary>
    /// <returns></returns>
    int number_of_items() const;

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

    TJMember* operator [](int idx) const;
    TJMember* at(int idx) const;

    bool is_object() const;

  protected:
    // protected contructor called internally so we can 
    // pass ownership of the member to this TJValue.
    TJValueObject(std::vector<TJMember*>* members);

  private:
    // All the key value pairs in this object.
    std::vector<TJMember*>* _members;

    void free_members();
  };

  // A Json object that contain an array of key/value pairs.
  class TJValueArray : public TJValue
  {
    friend TJHelper;
  public:
    TJValueArray();
    virtual ~TJValueArray();

    /// <summary>
    /// Get the number of items in this array
    /// </summary>
    /// <returns></returns>
    int number_of_items() const;

    /// <summary>
    /// Try and get a string representation of the value.
    /// </summary>
    /// <returns></returns>
    const char* to_string() const;

    TJValue* operator [](int idx) const;
    TJValue* at(int idx) const;

    bool is_array() const;

  protected:
    // protected contructor called internally so we can 
    // pass ownership of the member to this TJValue.
    TJValueArray(std::vector<TJValue*>* values);

  private:
    // All the key value pairs in this object.
    std::vector<TJValue*>* _values;

    void free_values();
  };

  // A string JSon
  class TJValueString : public TJValue
  {
    friend TJHelper;
  public:
    TJValueString(const char* value);
    virtual ~TJValueString();

    /// <summary>
    /// Try and get a string representation of the value.
    /// </summary>
    /// <returns></returns>
    const char* to_string() const;

    bool is_string() const;
  protected:
    // create while passing the ownership of the memory.
    TJValueString(char* value);

  private:
    char* _value;
    void free_value();
  };

  // A bolean JSon
  class TJValueBoolean : public TJValue
  {
  public:
    TJValueBoolean(bool is_true);
    virtual ~TJValueBoolean() = default;

    /// <summary>
    /// Try and get a string representation of the value.
    /// </summary>
    /// <returns></returns>
    const char* to_string() const;

    bool is_true() const;
    bool is_false() const;

  private:
    const bool _is_true;
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

    bool is_null() const;
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

    bool is_number() const;

  protected:
    const bool _is_negative;
  };

  // A number JSon, float or int
  class TJValueNumberInt : public TJValueNumber
  {
  public:
    TJValueNumberInt(const unsigned long long& number, const bool is_negative);
    virtual ~TJValueNumberInt() = default;

    long long get_number() const;

  private:
    const long long _number;
  };

  // A number JSon, float or int
  class TJValueNumberFloat : public TJValueNumber
  {
  public:
    TJValueNumberFloat(const unsigned long long& number, const unsigned long long& fraction, const unsigned int& fraction_exponent, bool is_negative);
    virtual ~TJValueNumberFloat() = default;

    long double get_number() const;

  private:
    const unsigned long long _number;
    const unsigned long long _fraction;
    const unsigned int _fraction_exponent;
  };

  // A number JSon, float or int
  class TJValueNumberExponent : public TJValueNumber
  {
  public:
    TJValueNumberExponent(const unsigned long long& number, const unsigned long long& fraction, const unsigned int& fraction_exponent, const int& exponent,bool is_negative);
    virtual ~TJValueNumberExponent();

    const char* to_string() const;
  private:
    void make_string();
    char* _string;
    const unsigned long long _number;
    const unsigned long long _fraction;
    const unsigned int _fraction_exponent;
    const int _exponent;
  };
} // TinyJSON