/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.
 
  losemymind.libo@gmail.com

****************************************************************************/
#ifndef FOUNDATIONKIT_CRC_HPP
#define FOUNDATIONKIT_CRC_HPP

#include "FoundationKit/GenericPlatformMacros.hpp"
#include "FoundationKit/Base/types.hpp"

NS_FK_BEGIN

// https://en.wikipedia.org/wiki/Cyclic_redundancy_check
// http://www.backplane.com/matt/crc64.html

template <typename _Ty, const _Ty Polynom>
struct crc_table
{
    _Ty Data[256];
    crc_table()
    {
        for (int i = 0, t = 0; i < 256; t = 8, i++)
        {
            Data[i] = i;
            while (t--) Data[i] = Data[i] >> 1 ^ (Data[i] & 1 ? Polynom : 0);
        }
    }
};

template <typename _Ty, const _Ty Polynom, const _Ty Initial, const _Ty Final>
class basic_crc
{
    static const crc_table<_Ty, Polynom> table;
public:
    static _Ty crc_buffer(const char* data, size_t length)
    {
        _Ty crc_value = Initial;
        if (data != nullptr)
        {
            for (size_t i = 0; i < length; i++)
            {
                crc_value = table.Data[(crc_value ^ data[i]) & 0xFF] ^ (crc_value >> 8);
            }
        }
        return crc_value ^ Final;
    }

    static _Ty crc_file(const std::string& filename)
    {
        FILE* fp = fopen(filename.c_str(), "rb");
        if (!fp) return 0;
        _Ty crc_value = Initial;
        uint64 total_readed = 0;
        size_t readed = 0;
        static const size_t BUFFER_SIZE = 1024 * 1024 * 5; //10M
        char* chunk_buf = new char[BUFFER_SIZE];
        while (!feof(fp))
        {
            readed = fread(chunk_buf, 1, BUFFER_SIZE, fp);
            if (readed == 0) break;
            total_readed += readed;
            for (size_t i = 0; i < readed; i++)
            {
                crc_value = table.Data[(crc_value ^ chunk_buf[i]) & 0xFF] ^ (crc_value >> 8);
            }
        }
        fclose(fp);
        delete chunk_buf;
        return crc_value ^ Final;
    }
};

template <typename _Ty, const _Ty Polynom, const _Ty Initial, const _Ty Final>
const crc_table<_Ty, Polynom> basic_crc<_Ty, Polynom, Initial, Final>::table;
/*******************************************************************************
Cyclic Redundancy Check (CRC)

A CRC is a numerical value related to a block of data. This value provides
information that allows an application to determine if the block of data has
been modified. A CRC is similar to a Checksum, but is considered a stronger
error-checking mechanism.

CRC is based on binary polynomial calculation.

Standard CRC-32 generator polynomial:
Name               : CRC-32 Standard
Standards          : ISO 3309, ITU-T V.42, ANSI X3.66, FIPS PUB 71
References         : ZIP, RAR, Ethernet, AUTODIN II, FDDI
Initializing value : FFFFFFFF
Finalizing value   : FFFFFFFF
Polynomial value   : 0x04C11DB7	0xEDB88320	0xDB710641	0x82608EDB
Polynom            : x^32 + x^26 + x^23 + x^22 + x^16 + x^12 + x^11 +
                     x^10 + x^8 + x^7 + x^5 + x^4 + x^2 + x + 1

Standard CRC-64 generator polynomial:
Name               : CRC-64 Standard
Standards          : ECMA
References         : -
Initializing value : FFFFFFFFFFFFFFFF
Finalizing value   : FFFFFFFFFFFFFFFF
Polynomial value   : 0x42F0E1EBA9EA3693	0xC96C5795D7870F42	0x92D8AF2BAF0E1E85	0xA17870F5D4F51B49
Polynom            : x^64 + x^62 + x^57 + x^55 + x^54 + x^63 + x^52 +
                     x^47 + x^46 + x^45 + x^40 + x^39 + x^38 + x^37 +
                     x^35 + x^33 + x^32 + x^32 + x^29 + x^27 + x^24 + 
                     x^23 + x^22 + x^21 + x^19 + x^17 + x^13 + x^12 + 
                     x^10 + x^9  + x^7  + x^4  + x + 1

*******************************************************************************/
typedef basic_crc<uint32, 0xEDB88320, 0xFFFFFFFFU, 0xFFFFFFFFU> crc32;
typedef basic_crc<uint64, 0x42F0E1EBA9EA3693, 0xFFFFFFFFFFFFFFFFULL, 0xFFFFFFFFFFFFFFFFULL> crc64;

NS_FK_END

#endif // END OF FOUNDATIONKIT_CRC_HPP


