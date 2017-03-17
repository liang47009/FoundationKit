/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.
 
  losemymind.libo@gmail.com

  Support Type:
      uint8
      uint16
      uint32
      unsigned long
      uint64
      bool
      char
      short
      int
      long
      long long
      float
      double
      char*
      std::string 

****************************************************************************/

/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.
 
  losemymind.libo@gmail.com

****************************************************************************/
#ifndef FOUNDATIONKIT_VALUE_HPP
#define FOUNDATIONKIT_VALUE_HPP


#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <cstdint>
#include <vector>
#include <list>
#include <map>
#include <unordered_map>
#include <string>
#include "FoundationKit/FoundationMacros.hpp"
#include "FoundationKit/Base/Types.hpp"

NS_FK_BEGIN

class  Value
{
public:
    enum class Type
    {
        NONE = 0,
        UCHAR,
        USHORT,
        UINT,
        ULONGLONG,
        BOOL,
        CHAR,
        SHORT,
        INT,
        LONGLONG,
        FLOAT,
        DOUBLE,
        PCHAR,
        STRING
    };


    static const Value Null;

    Value();
    ~Value();
    Value(const Value& other);
    Value(Value&& other);
    explicit Value(uint8 data);
    explicit Value(uint16 data);
    explicit Value(uint32 data);
    explicit Value(uint64 data);
    explicit Value(bool data);
    explicit Value(char data);
    explicit Value(int16 data);
    explicit Value(int32 data);
    explicit Value(int64 data);
    explicit Value(float data);
    explicit Value(double data);
    explicit Value(const char* data);
    explicit Value(const std::string& data);

    // assignment operator
    Value& operator= (const Value& other);
    Value& operator= (Value&& other);
    Value& operator= (uint8 data);
    Value& operator= (uint16 data);
    Value& operator= (uint32 data);
    Value& operator= (uint64 data);
    Value& operator= (bool data);
    Value& operator= (char data);
    Value& operator= (int16 data);
    Value& operator= (int32 data);
    Value& operator= (int64 data);
    Value& operator= (float data);
    Value& operator= (double data);
    Value& operator= (const char* data);
    Value& operator= (const std::string& data);

    /** != operator overloading */
    bool operator!= (const Value& other);
    /** != operator overloading */
    bool operator!= (const Value& other) const;
    /** == operator overloading */
    bool operator== (const Value& other);
    /** == operator overloading */
    bool operator== (const Value& other) const;
    ///** >= operator overloading */
    //bool operator>= (const Value & other) const;
    ///** <= operator overloading */
    //bool operator<= (const Value & other) const;
    ///** < operator overloading */
    //bool operator < (const Value & other) const;
    ///** > operator overloading */
    //bool operator > (const Value & other) const;

    inline bool isNull()  const { return _type == Type::NONE; }
    inline Type getType() const { return _type; };
    void        copy(Value&  other);
    void        move(Value&& other);
    void        swap(Value&  other);
    void        clear();

//================= Define Template Method =================
    template< typename T >
    inline T as();

private:

    void reset(Type valType);

    Value(Type valType);
    union
    {
        uint8              _ucharVal;
        uint16             _ushortVal;
        uint32             _uintVal;
        uint64             _ulonglongVal;
        bool               _boolVal;
        int8               _charVal;
        int16              _shortVal;
        int32              _intVal;
        int64              _longlongVal;
        float              _floatVal;
        double             _doubleVal;
        char*              _pcharVal;
        char*              _stringVal; //std::string
    }_field;

    Type _type;
};

typedef std::vector<Value>   ValueList;

NS_FK_END
#endif // FOUNDATIONKIT_VALUE_HPP

#include "Value.inl"



