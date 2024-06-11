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
  // the various types of formating.
  enum class formating
  {
    none,
    indented
  };

  class TJHelper;
  class TJValueArray;
  class TJValueBoolean;
  class TJValueNull;
  class TJValueNumberInt;

  // A simple JSON value, the base of all items in a json
  class TJValue
  {
    friend TJValueArray;
    friend TJValueBoolean;
    friend TJValueNull;
    friend TJValueNumberInt;
  public:
    TJValue();
    virtual ~TJValue();

    /// <summary>
    /// Try and get a string representation of the value.
    /// </summary>
    /// <returns></returns>
    virtual const char* to_string() const = 0;

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

    const char* dump(formating formating = formating::indented, const char* indent = "  ") const;

  protected:
    virtual void internal_dump(char*& buffer, formating formating, const char* current_indent, const char* indent, int& buffer_pos, int& buffer_max_length) const;

  private:
    TJValue(const TJValue&) = delete;
    TJValue(TJValue&&) = delete;
    TJValue& operator=(TJValue&&) = delete;
    TJValue& operator=(const TJValue&) = delete;

    mutable char* _last_dump;
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
    static TJValue* parse(const char* source);

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
    /// <summary>
    /// Move the value ownership to the member.
    /// The caller will _not_ delete!
    /// </summary>
    /// <param name="string"></param>
    /// <param name="value"></param>
    /// <returns></returns>
    static TJMember* move(char*& string, TJValue*& value);

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
    int get_number_of_items() const;

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

    /// <summary>
    /// Try and get a string representation of the value.
    /// </summary>
    /// <returns></returns>
    const char* to_string() const;

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

    virtual void internal_dump(char*& buffer, formating formating, const char* current_indent, const char* indent, int& buffer_pos, int& buffer_max_length) const;

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

    TJValue* clone() const;

    bool is_string() const;
  protected:
    /// <summary>
    /// Move the value ownership of the string.
    /// The caller will _not_ delete!
    /// </summary>
    /// <param name="value"></param>
    /// <returns></returns>
    static TJValueString* move(char*& value);

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

    TJValue* clone() const;

  protected:
    virtual void internal_dump(char*& buffer, formating formating, const char* current_indent, const char* indent, int& buffer_pos, int& buffer_max_length) const;

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

    TJValue* clone() const;

    bool is_null() const;

  protected:
    virtual void internal_dump(char*& buffer, formating formating, const char* current_indent, const char* indent, int& buffer_pos, int& buffer_max_length) const;
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
    TJValueNumberInt(const long long& number);
    TJValueNumberInt(const unsigned long long& number, const bool is_negative);
    virtual ~TJValueNumberInt() = default;

    long long get_number() const;

    TJValue* clone() const;

  protected:
    virtual void internal_dump(char*& buffer, formating formating, const char* current_indent, const char* indent, int& buffer_pos, int& buffer_max_length) const;

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

    TJValue* clone() const;

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

    TJValue* clone() const;
  private:
    void make_string();
    char* _string;
    const unsigned long long _number;
    const unsigned long long _fraction;
    const unsigned int _fraction_exponent;
    const int _exponent;
  };
} // TinyJSON