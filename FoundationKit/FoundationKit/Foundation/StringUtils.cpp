/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.

  losemymind.libo@gmail.com

****************************************************************************/
#include <locale>
#include <stdarg.h>
#include <algorithm>
#include <functional>
#include <cctype>
#include <sstream>
#include <cassert>
#include "FoundationKit/Base/locale.hpp"
#include "FoundationKit/Foundation/StringUtils.hpp"
#include "FoundationKit/external/ConvertUTF/ConvertUTF.h"

NS_FK_BEGIN

std::string StringUtils::Tolower( const std::string& str )
{
	std::string ret;
	for (auto ch : str)
	{
		if (ch >= 'A' && ch <= 'Z')
			ret += ch | 32;
		else
			ret += ch;
	}
	return ret;
}

std::string StringUtils::Toupper( const std::string& str )
{
	std::string ret;
	for (auto ch : str)
	{
		if (ch >= 'a' && ch <= 'z')
			ret += ch ^ 32;
		else
			ret += ch;
	}
	return ret;
}

std::string StringUtils::Format( const char* format, ... )
{
	const static int MAX_LENGTH = 64;
    // Pass one greater needed size to leave room for NULL terminator
    std::vector<char> DynamicBuffer;
    char* ResultStr = nullptr;
    int BufferSize = MAX_LENGTH;
    int LoopCount = 0;
    va_list ArgList;
    do
    {
        // Pass one greater needed size to leave room for NULL terminator
        DynamicBuffer.resize(BufferSize + 1);
        ResultStr = &DynamicBuffer[0];
        /*
        pitfall: The behavior of vsnprintf between VS2013 and VS2015/2017 is different
        VS2013 or Unix-Like System will return -1 when buffer not enough, but VS2015/2017
        will return the actural needed length for buffer at this station
        The _vsnprintf behavior is compatible API which always return -1 when buffer isn't
        enough at VS2013/2015/2017 Yes, The vsnprintf is more efficient implemented by MSVC 19.0 or later, AND it's also standard-compliant, see reference: http://www.cplusplus.com/reference/cstdio/vsnprintf/
        */
        va_start(ArgList, format);
        // Pass one greater needed size to leave room for NULL terminator
        int Needed = vsnprintf(ResultStr, BufferSize + 1, format, ArgList);
        va_end(ArgList);
        if (Needed >= 0 && Needed < BufferSize)
        {
            break;
        }
        else
        {
            BufferSize *= 2;
        }
    } while (++LoopCount < 10);
    return ResultStr;
}

std::string & StringUtils::LTrim( std::string &s )
{
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
	return s;
}

std::string & StringUtils::RTrim( std::string &s )
{
	s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
	return s;
}

std::string & StringUtils::Trim( std::string &s )
{
	return LTrim(RTrim(s));
}

std::vector<std::string> StringUtils::Split( const std::string &s, char delim, std::vector<std::string> &elems )
{
    std::string swapStr = s;
    if (swapStr.size() > 0 && swapStr[0] == delim)
        swapStr = swapStr.substr(1, swapStr.size() - 1);
    std::stringstream ss(swapStr);
    std::string item;
    while (std::getline(ss, item, delim))
    {
        elems.push_back(item);
    }
    return elems;
}

std::vector<std::string> StringUtils::Split( const std::string &s, char delim )
{
	std::vector<std::string> elems;
	Split(s, delim, elems);
	return elems;
}

std::vector<std::string> StringUtils::Split(const std::string &s, std::string delim)
{
    std::vector<std::string> elems;
    size_t delimLen = delim.size();
    size_t currentIndex = 0;
    size_t findPos = s.find(delim, currentIndex);
    if (findPos == std::string::npos)
    {
        elems.push_back(s);
    }
    else
    {
        if (findPos == currentIndex)
        {
            currentIndex = findPos + delimLen;
            findPos = s.find(delim, currentIndex);
        }
        do 
        {
            elems.push_back(s.substr(currentIndex, findPos - currentIndex));
            currentIndex = findPos + delimLen;
            findPos = s.find(delim, currentIndex);
        } while (findPos != std::string::npos);

        if (currentIndex < s.size())
        {
            elems.push_back(s.substr(currentIndex, s.size() - currentIndex));
        }
    }

    return elems;
}

std::string StringUtils::Join(std::string delim, const std::vector<std::string>& values)
{
	std::string ret;
	size_t index = 0;
	size_t valuesSize = values.size();
	for (auto& str : values)
	{
		++index;
		ret.append(str);
		if (index < valuesSize)
		{
			ret.append(delim);
		}
	}
	return ret;
}

bool StringUtils::IsNumber( const std::string& val )
{
	std::stringstream str(val);
	float tst;
	str >> tst;
	return !str.fail() && str.eof();
}


// Other impl see:http://blog.poxiao.me/p/unicode-character-encoding-conversion-in-cpp11/
std::string StringUtils::wstring2string(const std::wstring &input)
{
    scope_locale  sl("");
    std::string result;
    // First, determine the length of the destination buffer.
    size_t mbs_length = wcstombs(NULL, input.c_str(), 0);
	if (mbs_length == ((size_t)-1) || mbs_length == 0)
    {
        return result;
    }
    std::vector<char> mbs_v(mbs_length+1);
    // Now, convert.
    if (wcstombs(&mbs_v[0], input.c_str(), mbs_length) == ((size_t)-1))
    {
        return result;
    }
    result = &mbs_v[0];
    return result;
}

std::wstring StringUtils::string2wstring(const std::string &input)
{
    scope_locale  sl("");
    std::wstring result;
    // First, determine the length of the destination buffer.
    size_t wcs_length = mbstowcs(NULL, input.c_str(), 0);
	if (wcs_length == ((size_t)-1) || wcs_length == 0)
    {
        return result;
    }
    std::vector<wchar_t> wcs_v(wcs_length+1);
    // Now, convert.
    if (mbstowcs(&wcs_v[0], input.c_str(), wcs_length) == ((size_t)-1))
    {
        return result;
    }
    result = &wcs_v[0];
    return result;
}

using namespace llvm;
namespace detail
{
    template <typename T>
    struct ConvertTrait 
    {
        typedef T ArgType;
    };

    template <>
    struct ConvertTrait<char> 
    {
        typedef UTF8 ArgType;
    };

    template <>
    struct ConvertTrait<char16_t> 
    {
        typedef UTF16 ArgType;
    };

    template <>
    struct ConvertTrait<wchar_t>
    {
        typedef UTF16 ArgType;
    };

    template <>
    struct ConvertTrait<char32_t>
    {
        typedef UTF32 ArgType;
    };

    template <typename From, typename To, typename FromTrait = ConvertTrait<From>, typename ToTrait = ConvertTrait<To> >
    bool UTFConvert(const std::basic_string<From>& from
        , std::basic_string<To>& to
        , ConversionResult(*cvtfunc)(const typename FromTrait::ArgType**, const typename FromTrait::ArgType*,
            typename ToTrait::ArgType**, typename ToTrait::ArgType*,
            ConversionFlags)
    )
    {
        static_assert(sizeof(From) == sizeof(typename FromTrait::ArgType), "Error size mismatched");
        static_assert(sizeof(To) == sizeof(typename ToTrait::ArgType), "Error size mismatched");

        if (from.empty())
        {
            to.clear();
            return true;
        }

        // See: http://unicode.org/faq/utf_bom.html#gen6
        static const int most_bytes_per_character = 4;

        const size_t maxNumberOfChars = from.length(); // all UTFs at most one element represents one character.
        const size_t numberOfOut = maxNumberOfChars * most_bytes_per_character / sizeof(To);

        std::basic_string<To> working(numberOfOut, 0);

        auto inbeg = reinterpret_cast<const typename FromTrait::ArgType*>(&from[0]);
        auto inend = inbeg + from.length();


        auto outbeg = reinterpret_cast<typename ToTrait::ArgType*>(&working[0]);
        auto outend = outbeg + working.length();
        auto r = cvtfunc(&inbeg, inend, &outbeg, outend, strictConversion);
        if (r != conversionOK)
            return false;

        working.resize(reinterpret_cast<To*>(outbeg) - &working[0]);
        to = std::move(working);

        return true;
    };

}

bool StringUtils::UTF8ToUTF16(const std::string& inUtf8, std::u16string& outUtf16)
{
    return detail::UTFConvert(inUtf8, outUtf16, ConvertUTF8toUTF16);
}

bool StringUtils::UTF8ToUTF32(const std::string& inUtf8, std::u32string& outUtf32)
{
    return detail::UTFConvert(inUtf8, outUtf32, ConvertUTF8toUTF32);
}

bool StringUtils::UTF16ToUTF8(const std::u16string& inUtf16, std::string& outUtf8)
{
    return detail::UTFConvert(inUtf16, outUtf8, ConvertUTF16toUTF8);
}

bool StringUtils::UTF16ToUTF32(const std::u16string& inUtf16, std::u32string& outUtf32)
{
    return detail::UTFConvert(inUtf16, outUtf32, ConvertUTF16toUTF32);
}

bool StringUtils::UTF32ToUTF8(const std::u32string& inUtf32, std::string& outUtf8)
{
    return detail::UTFConvert(inUtf32, outUtf8, ConvertUTF32toUTF8);
}
bool StringUtils::UTF32ToUTF16(const std::u32string& inUtf32, std::u16string& outUtf16)
{
    return detail::UTFConvert(inUtf32, outUtf16, ConvertUTF32toUTF16);
}

/**
//https://www.codeguru.com/cpp/misc/misc/multi-lingualsupport/article.php/c10451/The-Basics-of-UTF8.htm
#define         MASKBITS                0x3F
#define         MASKBYTE                0x80
#define         MASK2BYTES              0xC0
#define         MASK3BYTES              0xE0
#define         MASK4BYTES              0xF0
#define         MASK5BYTES              0xF8
#define         MASK6BYTES              0xFC

typedef unsigned short   Unicode2Bytes;
typedef unsigned int     Unicode4Bytes;
typedef unsigned char    byte;

std::wstring StringUtils::UTF8ToUTF16(const std::string& input)
{
    std::wstring output;
    const unsigned char* szInput = reinterpret_cast<const unsigned char*>(input.data());
    for (size_t i = 0; i < input.size();)
    {
        Unicode2Bytes ch;
        auto val = szInput[i] & MASK3BYTES;
        // 1110xxxx 10xxxxxx 10xxxxxx
        if ((szInput[i] & MASK3BYTES) == MASK3BYTES)
        {
            ch = ((szInput[i] & 0x0F) << 12) | (
                (szInput[i + 1] & MASKBITS) << 6)
                | (szInput[i + 2] & MASKBITS);
            i += 3;
        }
        // 110xxxxx 10xxxxxx
        else if ((szInput[i] & MASK2BYTES) == MASK2BYTES)
        {
            ch = ((szInput[i] & 0x1F) << 6) | (szInput[i + 1] & MASKBITS);
            i += 2;
        }
        // 0xxxxxxx
        else if (szInput[i] < MASKBYTE)
        {
            ch = szInput[i];
            i += 1;
        }

        output.push_back(ch);
    }
    return output;
}


std::string StringUtils::UTF16ToUTF8(const std::wstring& input)
{
    std::string output;
    for (size_t i = 0; i < input.size(); i++)
    {
        // 0xxxxxxx
        if (input[i] < 0x80)
        {
            output.push_back((byte)input[i]);
        }
        // 110xxxxx 10xxxxxx
        else if (input[i] < 0x800)
        {
            output.push_back((byte)(MASK2BYTES | input[i] >> 6));
            output.push_back((byte)(MASKBYTE | input[i] & MASKBITS));
        }
        // 1110xxxx 10xxxxxx 10xxxxxx
        else if (input[i] < 0x10000)
        {
            output.push_back((byte)(MASK3BYTES | input[i] >> 12));
            output.push_back((byte)(MASKBYTE | input[i] >> 6 & MASKBITS));
            output.push_back((byte)(MASKBYTE | input[i] & MASKBITS));
        }
    }
    return output;
}



std::string StringUtils::UTF32ToUTF8(const std::u32string& input)
{
    std::string output;
    for (size_t i = 0; i < input.size(); i++)
    {
        // 0xxxxxxx
        if (input[i] < 0x80)
        {
            output.push_back((byte)input[i]);
        }
        // 110xxxxx 10xxxxxx
        else if (input[i] < 0x800)
        {
            output.push_back((byte)(MASK2BYTES | input[i] > 6));
            output.push_back((byte)(MASKBYTE | input[i] & MASKBITS));
        }
        // 1110xxxx 10xxxxxx 10xxxxxx
        else if (input[i] < 0x10000)
        {
            output.push_back((byte)(MASK3BYTES | input[i] >> 12));
            output.push_back((byte)(MASKBYTE | input[i] >> 6 & MASKBITS));
            output.push_back((byte)(MASKBYTE | input[i] & MASKBITS));
        }
        // 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
        else if (input[i] < 0x200000)
        {
            output.push_back((byte)(MASK4BYTES | input[i] >> 18));
            output.push_back((byte)(MASKBYTE | input[i] >> 12 & MASKBITS));
            output.push_back((byte)(MASKBYTE | input[i] >> 6 & MASKBITS));
            output.push_back((byte)(MASKBYTE | input[i] & MASKBITS));
        }
        // 111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
        else if (input[i] < 0x4000000)
        {
            output.push_back((byte)(MASK5BYTES | input[i] >> 24));
            output.push_back((byte)(MASKBYTE | input[i] >> 18 & MASKBITS));
            output.push_back((byte)(MASKBYTE | input[i] >> 12 & MASKBITS));
            output.push_back((byte)(MASKBYTE | input[i] >> 6 & MASKBITS));
            output.push_back((byte)(MASKBYTE | input[i] & MASKBITS));
        }
        // 1111110x 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
        else if (input[i] < 0x8000000)
        {
            output.push_back((byte)(MASK6BYTES | input[i] >> 30));
            output.push_back((byte)(MASKBYTE | input[i] >> 18 & MASKBITS));
            output.push_back((byte)(MASKBYTE | input[i] >> 12 & MASKBITS));
            output.push_back((byte)(MASKBYTE | input[i] >> 6 & MASKBITS));
            output.push_back((byte)(MASKBYTE | input[i] & MASKBITS));
        }
    }
    return output;
}

std::u32string StringUtils::UTF8ToUTF32(const std::string& input)
{
    std::u32string output;
    for (size_t i = 0; i < input.size();)
    {
        Unicode4Bytes ch;

        // 1111110x 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
        if ((input[i] & MASK6BYTES) == MASK6BYTES)
        {
            ch = ((input[i] & 0x01) << 30) | ((input[i + 1] & MASKBITS) << 24)
                | ((input[i + 2] & MASKBITS) << 18) | ((input[i + 3]
                    & MASKBITS) << 12)
                | ((input[i + 4] & MASKBITS) << 6) | (input[i + 5] & MASKBITS);
            i += 6;
        }
        // 111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
        else if ((input[i] & MASK5BYTES) == MASK5BYTES)
        {
            ch = ((input[i] & 0x03) << 24) | ((input[i + 1]
                & MASKBITS) << 18)
                | ((input[i + 2] & MASKBITS) << 12) | ((input[i + 3]
                    & MASKBITS) << 6)
                | (input[i + 4] & MASKBITS);
            i += 5;
        }
        // 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
        else if ((input[i] & MASK4BYTES) == MASK4BYTES)
        {
            ch = ((input[i] & 0x07) << 18) | ((input[i + 1]
                & MASKBITS) << 12)
                | ((input[i + 2] & MASKBITS) << 6) | (input[i + 3] & MASKBITS);
            i += 4;
        }
        // 1110xxxx 10xxxxxx 10xxxxxx
        else if ((input[i] & MASK3BYTES) == MASK3BYTES)
        {
            ch = ((input[i] & 0x0F) << 12) | ((input[i + 1] & MASKBITS) << 6)
                | (input[i + 2] & MASKBITS);
            i += 3;
        }
        // 110xxxxx 10xxxxxx
        else if ((input[i] & MASK2BYTES) == MASK2BYTES)
        {
            ch = ((input[i] & 0x1F) << 6) | (input[i + 1] & MASKBITS);
            i += 2;
        }
        // 0xxxxxxx
        else if (input[i] < MASKBYTE)
        {
            ch = input[i];
            i += 1;
        }
        output.push_back(ch);
    }
    return output;
}
*/

NS_FK_END
