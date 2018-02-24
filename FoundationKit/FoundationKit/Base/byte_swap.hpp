/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.
 
  losemymind.libo@gmail.com

****************************************************************************/
#ifndef FOUNDATIONKIT_BYTESWAP_HPP
#define FOUNDATIONKIT_BYTESWAP_HPP

#pragma once
#include "FoundationKit/Base/types.hpp"

// The following guarantees declaration of the byte swap functions
#ifdef _MSC_VER
# include <cstdlib>
#elif defined(__APPLE__)
// Mac OS X / Darwin features
#include <libkern/OSByteOrder.h>
#elif defined(__GLIBC__) || defined(__linux__)
#include <byteswap.h>
#define HAVE_BYTESWAP 1
#endif

NS_FK_BEGIN

//Other impl see:https://sourceforge.net/p/predef/wiki/Endianness/
#define BYTE_ENDIAN_ORDER (0xFFFFFFFF & 1)
#define LITTLE_ENDIAN_BYTE_ORDER 0x00000001    // little-endian byte-order
#define BIG_ENDIAN_BYTE_ORDER 0x01000000       // big-endian (network) byte-order
#define NETWORK_ENDIAN_BYTE_ORDER 0x01000000   // big-endian (network) byte-order
#define PDP_ENDIAN_BYTE_ORDER 0x00010000
#define UNKNOWN_ENDIAN_BYTE_ORDER  0xFFFFFFFF

enum endianness
{
    big_endian,
    little_endian,
#if (BYTE_ENDIAN_ORDER == BIG_ENDIAN_BYTE_ORDER)
    native_endian = big_endian,
#else
    native_endian = little_endian,
#endif
};

/// when define returns true it means that our architecture uses big endian
#define HOST_IS_BIG_ENDIAN (bool)(*(unsigned short *)"\0\xff" < 0x100) 



#if defined(_MSC_VER)

static inline uint16_t bswap_16(uint16_t val)
{
    return _byteswap_ushort(val);
}

static inline uint32_t bswap_32(uint32_t val)
{
    return _byteswap_ulong(val);
}

static inline uint64_t bswap_64(uint64_t val)
{
    return _byteswap_uint64(val);
}

#elif defined(__APPLE__)
static inline uint16_t bswap_16(uint16_t val)
{
    return OSSwapInt16(val);
}

static inline uint32_t bswap_32(uint32_t val)
{
    return OSSwapInt32(val);
}

static inline uint64_t bswap_64(uint64_t val)
{
    return OSSwapInt64(val);
}

#elif !defined(HAVE_BYTESWAP)

/**
 * bswap_16 - reverse bytes in a uint16_t value.
 * @val: value whose bytes to swap.
 *
 * Example:
 *	// Output contains "1024 is 4 as two bytes reversed"
 *	printf("1024 is %u as two bytes reversed\n", bswap_16(1024));
 */
static inline uint16_t bswap_16(uint16_t val)
{
    return ((val & (uint16_t)0x00ffU) << 8)
        | ((val & (uint16_t)0xff00U) >> 8);
}

/**
 * bswap_32 - reverse bytes in a uint32_t value.
 * @val: value whose bytes to swap.
 *
 * Example:
 *	// Output contains "1024 is 262144 as four bytes reversed"
 *	printf("1024 is %u as four bytes reversed\n", bswap_32(1024));
 */
static inline uint32_t bswap_32(uint32_t val)
{
    return ((val & (uint32_t)0x000000ffUL) << 24)
        | ((val & (uint32_t)0x0000ff00UL) << 8)
        | ((val & (uint32_t)0x00ff0000UL) >> 8)
        | ((val & (uint32_t)0xff000000UL) >> 24);
}


/**
 * bswap_64 - reverse bytes in a uint64_t value.
 * @val: value whose bytes to swap.
 *
 * Example:
 *	// Output contains "1024 is 1125899906842624 as eight bytes reversed"
 *	printf("1024 is %llu as eight bytes reversed\n",
 *		(unsigned long long)bswap_64(1024));
 */
static inline uint64_t bswap_64(uint64_t val)
{
    return ((val & (uint64_t)0x00000000000000ffULL) << 56)
        | ((val & (uint64_t)0x000000000000ff00ULL) << 40)
        | ((val & (uint64_t)0x0000000000ff0000ULL) << 24)
        | ((val & (uint64_t)0x00000000ff000000ULL) << 8)
        | ((val & (uint64_t)0x000000ff00000000ULL) >> 8)
        | ((val & (uint64_t)0x0000ff0000000000ULL) >> 24)
        | ((val & (uint64_t)0x00ff000000000000ULL) >> 40)
        | ((val & (uint64_t)0xff00000000000000ULL) >> 56);
}

#endif // #elif !defined(HAVE_BYTESWAP)


static inline void bswap_128(const void *src, void *dst)
{
    const uint8_t *s = (const uint8_t *)src;
    uint8_t *d = (uint8_t *)dst;
    int i;
    for (i = 0; i < 16; i++)
        d[15 - i] = s[i];
}

static inline void bswap_buf(uint8_t* inbytes, uint8_t* outbytes, uint32_t length)
{
    for (uint32_t i = 0; i < length; i++)
        outbytes[i] = inbytes[length - i - 1];
}

static inline int8_t   endian_reverse(int8_t x) noexcept
{
    return x;
}

static inline int16_t  endian_reverse(int16_t x) noexcept
{
    return bswap_16(static_cast<uint16_t>(x));
}

static inline int32_t  endian_reverse(int32_t x) noexcept
{
    return bswap_32(static_cast<uint32_t>(x));
}

static inline int64_t  endian_reverse(int64_t x) noexcept
{
    return bswap_64(static_cast<uint64_t>(x));
}

static inline uint8_t  endian_reverse(uint8_t x) noexcept
{
    return x;
}

static inline uint16_t endian_reverse(uint16_t x) noexcept
{
    return bswap_16(x);
}

static inline uint32_t endian_reverse(uint32_t x) noexcept
{
    return bswap_32(x);
}

static inline uint64_t endian_reverse(uint64_t x) noexcept
{
    return bswap_64(x);
}



template <class EndianReversible >
inline EndianReversible  big_to_native(EndianReversible  x) noexcept
{
#if (BYTE_ENDIAN_ORDER == BIG_ENDIAN_BYTE_ORDER)
    return x;
#   else
    return endian_reverse(x);
#   endif
}

template <class EndianReversible >
inline EndianReversible  native_to_big(EndianReversible  x) noexcept
{
#if (BYTE_ENDIAN_ORDER == BIG_ENDIAN_BYTE_ORDER)
    return x;
#   else
    return endian_reverse(x);
#   endif
}

template <class EndianReversible >
inline EndianReversible  little_to_native(EndianReversible  x) noexcept
{
#if (BYTE_ENDIAN_ORDER == LITTLE_ENDIAN_BYTE_ORDER)
    return x;
#   else
    return endian_reverse(x);
#   endif
}

template <class EndianReversible >
inline EndianReversible  native_to_little(EndianReversible  x) noexcept
{
#if (BYTE_ENDIAN_ORDER == LITTLE_ENDIAN_BYTE_ORDER)
    return x;
#   else
    return endian_reverse(x);
#   endif
}

NS_FK_END



#endif // FOUNDATIONKIT_BYTESWAP_HPP
