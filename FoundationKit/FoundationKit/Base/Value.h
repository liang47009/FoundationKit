/****************************************************************************
  Copyright (c) 2015 libo All rights reserved.
 
  losemymind.libo@gmail.com

  Support Type:
      unsigned char
      unsigned short
      unsigned int
      unsigned long
      unsigned long long
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
#ifndef LOSEMYMIND_VALUE_H
#define LOSEMYMIND_VALUE_H

#pragma once

#include <cstdint>
#include <vector>
#include <list>
#include <map>
#include <unordered_map>
#include <string>
#include "FoundationKit/FoundationKitDefines.h"

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
        ULONG,
        ULONGLONG,
        BOOL,
        CHAR,
        SHORT,
        INT,
        LONG,
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
    explicit Value(unsigned char data);
    explicit Value(unsigned short data);
    explicit Value(unsigned int data);
    explicit Value(unsigned long data);
    explicit Value(unsigned long long data);
    explicit Value(bool data);
    explicit Value(char data);
    explicit Value(short data);
    explicit Value(int data);
    explicit Value(long data);
    explicit Value(long long data);
    explicit Value(float data);
    explicit Value(double data);
    explicit Value(const char* data);
    explicit Value(const std::string& data);

    // assignment operator
    Value& operator= (const Value& other);
    Value& operator= (Value&& other);
    Value& operator= (unsigned char data);
    Value& operator= (unsigned short data);
    Value& operator= (unsigned int data);
    Value& operator= (unsigned long data);
    Value& operator= (unsigned long long data);
    Value& operator= (bool data);
    Value& operator= (char data);
    Value& operator= (short data);
    Value& operator= (int data);
    Value& operator= (long data);
    Value& operator= (long long data);
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
        unsigned char      _ucharVal;
        unsigned short     _ushortVal;
        unsigned int       _uintVal;
        unsigned long      _ulongVal;
        unsigned long long _ulonglongVal;
        bool               _boolVal;
        char               _charVal;
        short              _shortVal;
        int                _intVal;
        long               _longVal;
        long long          _longlongVal;
        float              _floatVal;
        double             _doubleVal;
        char*              _pcharVal;
        char*              _stringVal; //std::string
    }_field;

    Type _type;
};

NS_FK_END
#endif // LOSEMYMIND_VALUE_H

#include "Value.inl"



