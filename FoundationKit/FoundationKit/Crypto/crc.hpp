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

static const uint32 CRC32TABLE[] =
{
    UINT32_C(0x00000000),UINT32_C(0x77073096),UINT32_C(0xEE0E612C),UINT32_C(0x990951BA),
    UINT32_C(0x076DC419),UINT32_C(0x706AF48F),UINT32_C(0xE963A535),UINT32_C(0x9E6495A3),
    UINT32_C(0x0EDB8832),UINT32_C(0x79DCB8A4),UINT32_C(0xE0D5E91E),UINT32_C(0x97D2D988),
    UINT32_C(0x09B64C2B),UINT32_C(0x7EB17CBD),UINT32_C(0xE7B82D07),UINT32_C(0x90BF1D91),
    UINT32_C(0x1DB71064),UINT32_C(0x6AB020F2),UINT32_C(0xF3B97148),UINT32_C(0x84BE41DE),
    UINT32_C(0x1ADAD47D),UINT32_C(0x6DDDE4EB),UINT32_C(0xF4D4B551),UINT32_C(0x83D385C7),
    UINT32_C(0x136C9856),UINT32_C(0x646BA8C0),UINT32_C(0xFD62F97A),UINT32_C(0x8A65C9EC),
    UINT32_C(0x14015C4F),UINT32_C(0x63066CD9),UINT32_C(0xFA0F3D63),UINT32_C(0x8D080DF5),
    UINT32_C(0x3B6E20C8),UINT32_C(0x4C69105E),UINT32_C(0xD56041E4),UINT32_C(0xA2677172),
    UINT32_C(0x3C03E4D1),UINT32_C(0x4B04D447),UINT32_C(0xD20D85FD),UINT32_C(0xA50AB56B),
    UINT32_C(0x35B5A8FA),UINT32_C(0x42B2986C),UINT32_C(0xDBBBC9D6),UINT32_C(0xACBCF940),
    UINT32_C(0x32D86CE3),UINT32_C(0x45DF5C75),UINT32_C(0xDCD60DCF),UINT32_C(0xABD13D59),
    UINT32_C(0x26D930AC),UINT32_C(0x51DE003A),UINT32_C(0xC8D75180),UINT32_C(0xBFD06116),
    UINT32_C(0x21B4F4B5),UINT32_C(0x56B3C423),UINT32_C(0xCFBA9599),UINT32_C(0xB8BDA50F),
    UINT32_C(0x2802B89E),UINT32_C(0x5F058808),UINT32_C(0xC60CD9B2),UINT32_C(0xB10BE924),
    UINT32_C(0x2F6F7C87),UINT32_C(0x58684C11),UINT32_C(0xC1611DAB),UINT32_C(0xB6662D3D),
    UINT32_C(0x76DC4190),UINT32_C(0x01DB7106),UINT32_C(0x98D220BC),UINT32_C(0xEFD5102A),
    UINT32_C(0x71B18589),UINT32_C(0x06B6B51F),UINT32_C(0x9FBFE4A5),UINT32_C(0xE8B8D433),
    UINT32_C(0x7807C9A2),UINT32_C(0x0F00F934),UINT32_C(0x9609A88E),UINT32_C(0xE10E9818),
    UINT32_C(0x7F6A0DBB),UINT32_C(0x086D3D2D),UINT32_C(0x91646C97),UINT32_C(0xE6635C01),
    UINT32_C(0x6B6B51F4),UINT32_C(0x1C6C6162),UINT32_C(0x856530D8),UINT32_C(0xF262004E),
    UINT32_C(0x6C0695ED),UINT32_C(0x1B01A57B),UINT32_C(0x8208F4C1),UINT32_C(0xF50FC457),
    UINT32_C(0x65B0D9C6),UINT32_C(0x12B7E950),UINT32_C(0x8BBEB8EA),UINT32_C(0xFCB9887C),
    UINT32_C(0x62DD1DDF),UINT32_C(0x15DA2D49),UINT32_C(0x8CD37CF3),UINT32_C(0xFBD44C65),
    UINT32_C(0x4DB26158),UINT32_C(0x3AB551CE),UINT32_C(0xA3BC0074),UINT32_C(0xD4BB30E2),
    UINT32_C(0x4ADFA541),UINT32_C(0x3DD895D7),UINT32_C(0xA4D1C46D),UINT32_C(0xD3D6F4FB),
    UINT32_C(0x4369E96A),UINT32_C(0x346ED9FC),UINT32_C(0xAD678846),UINT32_C(0xDA60B8D0),
    UINT32_C(0x44042D73),UINT32_C(0x33031DE5),UINT32_C(0xAA0A4C5F),UINT32_C(0xDD0D7CC9),
    UINT32_C(0x5005713C),UINT32_C(0x270241AA),UINT32_C(0xBE0B1010),UINT32_C(0xC90C2086),
    UINT32_C(0x5768B525),UINT32_C(0x206F85B3),UINT32_C(0xB966D409),UINT32_C(0xCE61E49F),
    UINT32_C(0x5EDEF90E),UINT32_C(0x29D9C998),UINT32_C(0xB0D09822),UINT32_C(0xC7D7A8B4),
    UINT32_C(0x59B33D17),UINT32_C(0x2EB40D81),UINT32_C(0xB7BD5C3B),UINT32_C(0xC0BA6CAD),
    UINT32_C(0xEDB88320),UINT32_C(0x9ABFB3B6),UINT32_C(0x03B6E20C),UINT32_C(0x74B1D29A),
    UINT32_C(0xEAD54739),UINT32_C(0x9DD277AF),UINT32_C(0x04DB2615),UINT32_C(0x73DC1683),
    UINT32_C(0xE3630B12),UINT32_C(0x94643B84),UINT32_C(0x0D6D6A3E),UINT32_C(0x7A6A5AA8),
    UINT32_C(0xE40ECF0B),UINT32_C(0x9309FF9D),UINT32_C(0x0A00AE27),UINT32_C(0x7D079EB1),
    UINT32_C(0xF00F9344),UINT32_C(0x8708A3D2),UINT32_C(0x1E01F268),UINT32_C(0x6906C2FE),
    UINT32_C(0xF762575D),UINT32_C(0x806567CB),UINT32_C(0x196C3671),UINT32_C(0x6E6B06E7),
    UINT32_C(0xFED41B76),UINT32_C(0x89D32BE0),UINT32_C(0x10DA7A5A),UINT32_C(0x67DD4ACC),
    UINT32_C(0xF9B9DF6F),UINT32_C(0x8EBEEFF9),UINT32_C(0x17B7BE43),UINT32_C(0x60B08ED5),
    UINT32_C(0xD6D6A3E8),UINT32_C(0xA1D1937E),UINT32_C(0x38D8C2C4),UINT32_C(0x4FDFF252),
    UINT32_C(0xD1BB67F1),UINT32_C(0xA6BC5767),UINT32_C(0x3FB506DD),UINT32_C(0x48B2364B),
    UINT32_C(0xD80D2BDA),UINT32_C(0xAF0A1B4C),UINT32_C(0x36034AF6),UINT32_C(0x41047A60),
    UINT32_C(0xDF60EFC3),UINT32_C(0xA867DF55),UINT32_C(0x316E8EEF),UINT32_C(0x4669BE79),
    UINT32_C(0xCB61B38C),UINT32_C(0xBC66831A),UINT32_C(0x256FD2A0),UINT32_C(0x5268E236),
    UINT32_C(0xCC0C7795),UINT32_C(0xBB0B4703),UINT32_C(0x220216B9),UINT32_C(0x5505262F),
    UINT32_C(0xC5BA3BBE),UINT32_C(0xB2BD0B28),UINT32_C(0x2BB45A92),UINT32_C(0x5CB36A04),
    UINT32_C(0xC2D7FFA7),UINT32_C(0xB5D0CF31),UINT32_C(0x2CD99E8B),UINT32_C(0x5BDEAE1D),
    UINT32_C(0x9B64C2B0),UINT32_C(0xEC63F226),UINT32_C(0x756AA39C),UINT32_C(0x026D930A),
    UINT32_C(0x9C0906A9),UINT32_C(0xEB0E363F),UINT32_C(0x72076785),UINT32_C(0x05005713),
    UINT32_C(0x95BF4A82),UINT32_C(0xE2B87A14),UINT32_C(0x7BB12BAE),UINT32_C(0x0CB61B38),
    UINT32_C(0x92D28E9B),UINT32_C(0xE5D5BE0D),UINT32_C(0x7CDCEFB7),UINT32_C(0x0BDBDF21),
    UINT32_C(0x86D3D2D4),UINT32_C(0xF1D4E242),UINT32_C(0x68DDB3F8),UINT32_C(0x1FDA836E),
    UINT32_C(0x81BE16CD),UINT32_C(0xF6B9265B),UINT32_C(0x6FB077E1),UINT32_C(0x18B74777),
    UINT32_C(0x88085AE6),UINT32_C(0xFF0F6A70),UINT32_C(0x66063BCA),UINT32_C(0x11010B5C),
    UINT32_C(0x8F659EFF),UINT32_C(0xF862AE69),UINT32_C(0x616BFFD3),UINT32_C(0x166CCF45),
    UINT32_C(0xA00AE278),UINT32_C(0xD70DD2EE),UINT32_C(0x4E048354),UINT32_C(0x3903B3C2),
    UINT32_C(0xA7672661),UINT32_C(0xD06016F7),UINT32_C(0x4969474D),UINT32_C(0x3E6E77DB),
    UINT32_C(0xAED16A4A),UINT32_C(0xD9D65ADC),UINT32_C(0x40DF0B66),UINT32_C(0x37D83BF0),
    UINT32_C(0xA9BCAE53),UINT32_C(0xDEBB9EC5),UINT32_C(0x47B2CF7F),UINT32_C(0x30B5FFE9),
    UINT32_C(0xBDBDF21C),UINT32_C(0xCABAC28A),UINT32_C(0x53B39330),UINT32_C(0x24B4A3A6),
    UINT32_C(0xBAD03605),UINT32_C(0xCDD70693),UINT32_C(0x54DE5729),UINT32_C(0x23D967BF),
    UINT32_C(0xB3667A2E),UINT32_C(0xC4614AB8),UINT32_C(0x5D681B02),UINT32_C(0x2A6F2B94),
    UINT32_C(0xB40BBE37),UINT32_C(0xC30C8EA1),UINT32_C(0x5A05DF1B),UINT32_C(0x2D02EF8D)
};

// CRC-32-IEEE 802.3
static inline uint32 CRC32(const char* data, size_t length)
{
    uint32 CRC32Value = 0xFFFFFFFFU;
    if (data != nullptr)
    {
        for (size_t i = 0; i < length; i++)
        {
            CRC32Value = CRC32TABLE[(CRC32Value ^ data[i]) & 0xFF] ^ (CRC32Value >> 8);
        }
    }
    return ~CRC32Value;
}

static inline uint32 CRC32_File(const std::string& filename)
{

    FILE* fp = fopen(filename.c_str(), "rb");
    if (!fp) return 0;
    uint32 CRC32Value = 0xFFFFFFFFU;
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
            CRC32Value = CRC32TABLE[(CRC32Value ^ chunk_buf[i]) & 0xFF] ^ (CRC32Value >> 8);
        }
    }
    fclose(fp);
    delete chunk_buf;
    return ~CRC32Value;
}


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
typedef basic_crc<uint32, 0x04C11DB7, 0xFFFFFFFFU, 0xFFFFFFFFU> crc32;
typedef basic_crc<uint64, 0x42F0E1EBA9EA3693, 0xFFFFFFFFFFFFFFFFULL, 0xFFFFFFFFFFFFFFFFULL> crc64;


NS_FK_END

#endif // END OF FOUNDATIONKIT_CRC_HPP


