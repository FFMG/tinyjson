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

#ifndef TJ_USE_CHAR
#  define TJ_USE_CHAR 1
#endif

#if TJ_USE_CHAR == 1
#  define TJCHAR char
#  define TJCHARPREFIX(x) x
#elif TJ_USE_CHAR == 8
#  define TJCHAR char8_t
#  define TJCHARPREFIX(x) u8 ## x
#elif TJ_USE_CHAR == 16
#  define TJCHAR char16_t
#  define TJCHARPREFIX(x) u ## x
#elif TJ_USE_CHAR == 32
#  define TJCHAR char32_t
#  define TJCHARPREFIX(x) U ## x
#endif

#include <vector>

namespace TinyJSON
{
  // the various types of formating.
  enum class formating
  {
    none,
    indented
  };

  struct internal_dump_configuration;
  class TJHelper;
  class TJValueArray;
  class TJValueObject;

  // A simple JSON value, the base of all items in a json
  class TJValue
  {
    friend TJValueArray;
    friend TJValueObject;
  public:
    TJValue();
    virtual ~TJValue();

    /// <summary>
    /// Allow each derived class to create a copy of itself.
    /// </summary>
    /// <returns></returns>
    virtual TJValue* clone() const = 0;

    virtual bool is_object() const;
    virtual bool is_array() const;
    virtual bool is_string() const;
    virtual bool is_number() const;
    virtual bool is_true() const;
    virtual bool is_false() const;
    virtual bool is_null() const;

    const TJCHAR* dump(formating formating = formating::indented, const TJCHAR* indent = TJCHARPREFIX("  ")) const;
    const TJCHAR* dump_string() const;

  protected:
    virtual void internal_dump(internal_dump_configuration& configuration, const TJCHAR* current_indent) const = 0;

  private:
    TJValue(const TJValue&) = delete;
    TJValue(TJValue&&) = delete;
    TJValue& operator=(TJValue&&) = delete;
    TJValue& operator=(const TJValue&) = delete;

    mutable TJCHAR* _last_dump;
    void free_last_dump() const;
  };

  // The parser class
  class TinyJSON
  {
  public:
    virtual ~TinyJSON() = default;

    /// <summary>
    /// Parse a json string
    /// </summary>
    /// <param name="source">The source we are trying to parse.</param>
    /// <returns></returns>
    static TJValue* parse(const TJCHAR* source);

  private:
    TinyJSON() = delete;
    TinyJSON(TinyJSON&&) = delete;
    TinyJSON(const TinyJSON&) = delete;
    TinyJSON& operator=(const TinyJSON&) = delete;
    TinyJSON& operator=(TinyJSON&&) = delete;
  };

  // A TJMember is a key value pair, (name/value), that belong to an object.
  class TJMember
  {
    friend TJHelper;
  public:
    TJMember(const TJCHAR* string, const TJValue* value);
    virtual ~TJMember();

    const TJCHAR* name() const;
    const TJValue* value() const;

  protected:
    /// <summary>
    /// Move the value ownership to the member.
    /// The caller will _not_ delete!
    /// </summary>
    /// <param name="string"></param>
    /// <param name="value"></param>
    /// <returns></returns>
    static TJMember* move(TJCHAR*& string, TJValue*& value);

  private:
    TJCHAR* _string;
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
    int get_number_of_items() const;

    /// <summary>
    /// Try and get a string value, if it does not exist, then we return null.
    /// </summary>
    /// <param name="name"></param>
    /// <returns></returns>
    const TJCHAR* try_get_string(const TJCHAR* name) const;

    /// <summary>
    /// Try and get the value of this member if it exists.
    /// </summary>
    /// <param name="name"></param>
    /// <returns></returns>
    virtual const TJValue* try_get_value(const TJCHAR* name) const;

    TJValue* clone() const;

    TJMember* operator [](int idx) const;
    TJMember* at(int idx) const;

    bool is_object() const;

  protected:
    /// <summary>
    /// Move the value ownership to the members.
    /// The caller will _not_ delete!
    /// </summary>
    /// <param name="members"></param>
    /// <returns></returns>
    static TJValueObject* move(std::vector<TJMember*>*& members);

    void internal_dump(internal_dump_configuration& configuration, const TJCHAR* current_indent) const;

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
    int get_number_of_items() const;

    TJValue* clone() const;

    TJValue* operator [](int idx) const;
    TJValue* at(int idx) const;

    bool is_array() const;

  protected:
    /// <summary>
    /// Move the value ownership to the values.
    /// The caller will _not_ delete!
    /// </summary>
    /// <param name="values"></param>
    /// <returns></returns>
    static TJValueArray* move(std::vector<TJValue*>*& values);

    void internal_dump(internal_dump_configuration& configuration, const TJCHAR* current_indent) const;

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
    TJValueString(const TJCHAR* value);
    virtual ~TJValueString();

    TJValue* clone() const;

    bool is_string() const;
  protected:
    /// <summary>
    /// Move the value ownership of the string.
    /// The caller will _not_ delete!
    /// </summary>
    /// <param name="value"></param>
    /// <returns></returns>
    static TJValueString* move(TJCHAR*& value);

    void internal_dump(internal_dump_configuration& configuration, const TJCHAR* current_indent) const;

  private:
    TJCHAR* _value;
    void free_value();
  };

  // A bolean JSon
  class TJValueBoolean : public TJValue
  {
  public:
    TJValueBoolean(bool is_true);
    virtual ~TJValueBoolean() = default;

    bool is_true() const;
    bool is_false() const;

    TJValue* clone() const;

  protected:
    void internal_dump(internal_dump_configuration& configuration, const TJCHAR* current_indent) const;

  private:
    const bool _is_true;
  };

  // A null JSon
  class TJValueNull : public TJValue
  {
  public:
    TJValueNull();
    virtual ~TJValueNull() = default;

    TJValue* clone() const;

    bool is_null() const;

  protected:
    void internal_dump(internal_dump_configuration& configuration, const TJCHAR* current_indent) const;
  };

  // A number JSon, float or int
  class TJValueNumber : public TJValue
  {
  protected:
    TJValueNumber(const bool is_negative);
    virtual ~TJValueNumber() = default;

  public:
    bool is_number() const;

  protected:
    const bool _is_negative;
  };

  // A number JSon, float or int
  class TJValueNumberInt : public TJValueNumber
  {
  public:
    TJValueNumberInt(const long long& number);
    TJValueNumberInt(const unsigned long long& number, const bool is_negative);
    virtual ~TJValueNumberInt() = default;

    long long get_number() const;

    TJValue* clone() const;

  protected:
    void internal_dump(internal_dump_configuration& configuration, const TJCHAR* current_indent) const;

  private:
    const long long _number;
  };

  // A number JSon, float or int
  class TJValueNumberFloat : public TJValueNumber
  {
  public:
    TJValueNumberFloat(const unsigned long long& number, const unsigned long long& fraction, const unsigned int& fraction_exponent, bool is_negative);
    virtual ~TJValueNumberFloat();

    long double get_number() const;

    TJValue* clone() const;

  protected:
    void internal_dump(internal_dump_configuration& configuration, const TJCHAR* current_indent) const;

  private:
  private:
    void make_string_if_needed() const;
    mutable TJCHAR* _string;
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

    TJValue* clone() const;

  protected:
    void internal_dump(internal_dump_configuration& configuration, const TJCHAR* current_indent) const;

  private:
    void make_string_if_needed() const;
    mutable TJCHAR* _string;
    const unsigned long long _number;
    const unsigned long long _fraction;
    const unsigned int _fraction_exponent;
    const int _exponent;
  };
} // TinyJSON