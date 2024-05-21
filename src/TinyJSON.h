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
    virtual const char* TryGet(const char* name) const = 0;

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
    const char* TryGet(const char* name) const;

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

    const char* Name() const
    {
      return _string;
    }

    /// <summary>
    /// Try and get a string representation of the value.
    /// </summary>
    /// <returns></returns>
    const char* ToString() const;

  protected:
    // create while passing the ownership of the memory.
    TJMember(char* string, TJValue* value);

    static TJMember* try_read_string_and_value(const char*& p);
    static char* try_read_string(const char*& p);

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
} // TinyJSON