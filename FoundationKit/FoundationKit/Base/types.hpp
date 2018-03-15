/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.
 
  losemymind.libo@gmail.com

****************************************************************************/
#ifndef FOUNDATIONKIT_TYPES_HPP
#define FOUNDATIONKIT_TYPES_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <string>
#include <vector>
#include <cstdint>
#include "FoundationKit/GenericPlatformMacros.hpp"

NS_FK_BEGIN

using std::int16_t;
using std::int_least16_t;
using std::uint16_t;
using std::uint_least16_t;
using std::int32_t;
using std::int_least32_t;
using std::uint32_t;
using std::uint_least32_t;
using std::int64_t;
using std::int_least64_t;
using std::uint64_t;
using std::uint_least64_t;
using std::uintmax_t;

//---------------------------------------------------------------------
// Utility for automatically setting up the pointer-sized integer type
// http://zh.cppreference.com/w/cpp/language/types
//---------------------------------------------------------------------

// Unsigned base types.
typedef unsigned char 		uint8;		// 8-bit  unsigned.
typedef unsigned short int	uint16;		// 16-bit unsigned.
typedef unsigned int		uint32;		// 32-bit unsigned.
typedef unsigned long long	uint64;		// 64-bit unsigned.

// Signed base types.
typedef	signed char			int8;		// 8-bit  signed.
typedef signed short int	int16;		// 16-bit signed.
typedef signed int	 		int32;		// 32-bit signed.
typedef signed long long	int64;		// 64-bit signed.

// Floating point types
typedef float		        float32;    // 32-bit signed.
typedef double		        float64;    // 64-bit signed.
//typedef long double       float128;   // 80-bit signed.

// Character types.
typedef char				ANSICHAR;	// An ANSI character       -                  8-bit fixed-width representation of 7-bit characters.
typedef wchar_t				WIDECHAR;	// A wide character        - In-memory only.  ?-bit fixed-width representation of the platform's natural wide character set.  Could be different sizes on different platforms.
typedef uint8				CHAR8;		// An 8-bit character type - In-memory only.  8-bit representation.  Should really be char8_t but making this the generic option is easier for compilers which don't fully support C++11 yet (i.e. MSVC).
typedef uint16				CHAR16;		// A 16-bit character type - In-memory only.  16-bit representation.  Should really be char16_t but making this the generic option is easier for compilers which don't fully support C++11 yet (i.e. MSVC).
typedef uint32				CHAR32;		// A 32-bit character type - In-memory only.  32-bit representation.  Should really be char32_t but making this the generic option is easier for compilers which don't fully support C++11 yet (i.e. MSVC).
typedef WIDECHAR			TCHAR;		// A switchable character  - In-memory only.  Either ANSICHAR or WIDECHAR, depending on a licensee's requirements.

template<typename T32BITS, typename T64BITS, int PointerSize>
struct SelectIntPointerType
{
    // nothing here are is it an error if the partial specializations fail
};

template<typename T32BITS, typename T64BITS>
struct SelectIntPointerType<T32BITS, T64BITS, 8>
{
    typedef T64BITS type; // select the 64 bit type
};

template<typename T32BITS, typename T64BITS>
struct SelectIntPointerType<T32BITS, T64BITS, 4>
{
    typedef T32BITS type; // select the 32 bit type
};

typedef SelectIntPointerType<uint32, uint64, sizeof(void*)>::type UPTRINT;	// unsigned int the same size as a pointer
typedef SelectIntPointerType<int32, int64, sizeof(void*)>::type PTRINT;   // signed int the same size as a pointer

typedef std::basic_string<uint8, std::char_traits<uint8>, std::allocator<uint8> > u8string;


struct uint24_t
{
    uint32_t val;
    uint24_t() {}
    inline operator uint32_t() { return val; }
    inline operator uint32_t() const { return val; }
    inline uint24_t(const uint24_t& a) { val = a.val; }
    inline uint24_t operator++() { ++val; val &= 0x00FFFFFF; return *this; }
    inline uint24_t operator--() { --val; val &= 0x00FFFFFF; return *this; }
    inline uint24_t operator++(int) { uint24_t temp(val); ++val; val &= 0x00FFFFFF; return temp; }
    inline uint24_t operator--(int) { uint24_t temp(val); --val; val &= 0x00FFFFFF; return temp; }
    inline uint24_t operator&(const uint24_t& a) { return uint24_t(val&a.val); }
    inline uint24_t& operator=(const uint24_t& a) { val = a.val; return *this; }
    inline uint24_t& operator+=(const uint24_t& a) { val += a.val; val &= 0x00FFFFFF; return *this; }
    inline uint24_t& operator-=(const uint24_t& a) { val -= a.val; val &= 0x00FFFFFF; return *this; }
    inline bool operator==(const uint24_t& right) const { return val == right.val; }
    inline bool operator!=(const uint24_t& right) const { return val != right.val; }
    inline bool operator > (const uint24_t& right) const { return val > right.val; }
    inline bool operator < (const uint24_t& right) const { return val < right.val; }
    inline const uint24_t operator+(const uint24_t &other) const { return uint24_t(val + other.val); }
    inline const uint24_t operator-(const uint24_t &other) const { return uint24_t(val - other.val); }
    inline const uint24_t operator/(const uint24_t &other) const { return uint24_t(val / other.val); }
    inline const uint24_t operator*(const uint24_t &other) const { return uint24_t(val*other.val); }
    inline uint24_t(const uint32_t& a) { val = a; val &= 0x00FFFFFF; }
    inline uint24_t operator&(const uint32_t& a) { return uint24_t(val&a); }
    inline uint24_t& operator=(const uint32_t& a) { val = a; val &= 0x00FFFFFF; return *this; }
    inline uint24_t& operator+=(const uint32_t& a) { val += a; val &= 0x00FFFFFF; return *this; }
    inline uint24_t& operator-=(const uint32_t& a) { val -= a; val &= 0x00FFFFFF; return *this; }
    inline bool operator==(const uint32_t& right) const { return val == (right & 0x00FFFFFF); }
    inline bool operator!=(const uint32_t& right) const { return val != (right & 0x00FFFFFF); }
    inline bool operator > (const uint32_t& right) const { return val > (right & 0x00FFFFFF); }
    inline bool operator < (const uint32_t& right) const { return val < (right & 0x00FFFFFF); }
    inline const uint24_t operator+(const uint32_t &other) const { return uint24_t(val + other); }
    inline const uint24_t operator-(const uint32_t &other) const { return uint24_t(val - other); }
    inline const uint24_t operator/(const uint32_t &other) const { return uint24_t(val / other); }
    inline const uint24_t operator*(const uint32_t &other) const { return uint24_t(val*other); }
};

namespace TypeTest
{
    static_assert(sizeof(uint8)      == 1, "uint8 type size test failed.");
    static_assert(sizeof(uint16)     == 2, "uint16 type size test failed.");
    static_assert(sizeof(uint32)     == 4, "uint32 type size test failed.");
    static_assert(sizeof(uint64)     == 8, "uint64 type size test failed.");
    static_assert(sizeof(int8)       == 1, "int8 type size test failed.");
    static_assert(sizeof(int16)      == 2, "int16 type size test failed.");
    static_assert(sizeof(int32)      == 4, "int32 type size test failed.");
    static_assert(sizeof(int64)      == 8, "int64 type size test failed.");
    static_assert(sizeof(float32)    == 4, "float32 type size test failed.");
    static_assert(sizeof(float64)    == 8, "float64 type size test failed.");
    //static_assert(sizeof(float128) == 8, "float128 type size test failed.");//ÓÐ´ýÉÌÈ¶
    static_assert(sizeof(PTRINT)     == sizeof(void *), "PTRINT type size test failed.");
    static_assert(sizeof(UPTRINT)    == sizeof(void *), "UPTRINT type size test failed.");
    static_assert(int32(uint8(-1))   == 0xFF, "uint8 type sign test failed.");
    static_assert(int32(uint16(-1))  == 0xFFFF, "uint16 type sign test failed.");
    static_assert(int64(uint32(-1))  == int64(0xFFFFFFFF), "uint32 type sign test failed.");
    static_assert(uint64(-1) > uint64(0), "uint64 type sign test failed.");
    static_assert(int32(int8(-1)) == -1, "int8 type sign test failed.");
    static_assert(int32(int16(-1)) == -1, "int16 type sign test failed.");
    static_assert(int64(int32(-1)) == int64(-1), "int32 type sign test failed.");
    static_assert(int64(-1) < int64(0), "int64 type sign test failed.");
    static_assert(int32(ANSICHAR(-1)) == -1, "ANSICHAR type sign test failed.");
    static_assert(sizeof(WIDECHAR) == 2 || sizeof(WIDECHAR) == 4, "WIDECHAR type size test failed.");
    static_assert(PTRINT(-1) < PTRINT(0), "PTRINT type sign test failed.");
    static_assert(UPTRINT(-1) > UPTRINT(0), "UPTRINT type sign test failed.");
}

NS_FK_END

#endif // FOUNDATIONKIT_TYPES_HPP
