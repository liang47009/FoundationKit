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
//#include <codecvt>
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
    std::vector<char> dynamicBuffer;
    char* result = nullptr;
    int BufferSize = MAX_LENGTH;
    int needed = 0;
    int loopCount = 0;
    va_list arglist;
    va_start(arglist, format);
    do
    {
        // Pass one greater needed size to leave room for NULL terminator
        dynamicBuffer.resize(BufferSize + 1);
        result = &dynamicBuffer[0];
        /*
        pitfall: The behavior of vsnprintf between VS2013 and VS2015/2017 is different
        VS2013 or Unix-Like System will return -1 when buffer not enough, but VS2015/2017
        will return the actural needed length for buffer at this station
        The _vsnprintf behavior is compatible API which always return -1 when buffer isn't
        enough at VS2013/2015/2017 Yes, The vsnprintf is more efficient implemented by MSVC 19.0 or later, AND it's also standard-compliant, see reference: http://www.cplusplus.com/reference/cstdio/vsnprintf/
        */
        // Pass one greater needed size to leave room for NULL terminator
        needed = vsnprintf(result, BufferSize + 1, format, arglist);
        if (needed >= 0 && needed < BufferSize)
        {
            break;
        }
        else
        {
            BufferSize *= 2;
        }
    } while (++loopCount < 10);
    va_end(arglist);
    return result;
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
std::string StringUtils::wstring2UTF8string(const std::wstring &input)
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

std::wstring StringUtils::string2UTF8wstring(const std::string &input)
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

/*
std::u16string StringUtils::UTF8ToUTF16(const std::string &s)
{
    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> conv;
    return conv.from_bytes(s);
}

std::u32string StringUtils::UTF8ToUTF32(const std::string &s)
{
    std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> conv;
    return conv.from_bytes(s);
}

std::string StringUtils::UTF16ToUTF8(const std::u16string &s)
{
    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> conv;
    return conv.to_bytes(s);
}

std::u32string StringUtils::UTF16ToUTF32(const std::u16string &s)
{
    const char16_t *pData = s.c_str();
    std::wstring_convert<std::codecvt_utf16<char32_t>, char32_t> conv;
    return conv.from_bytes(reinterpret_cast<const char*>(pData), reinterpret_cast<const char*>(pData + s.length()));
}

std::string StringUtils::UTF32ToUTF8(const std::u32string &s)
{
    std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> conv;
    return conv.to_bytes(s);
}


std::u16string StringUtils::UTF32ToUTF16(const std::u32string &s)
{
    std::wstring_convert<std::codecvt_utf16<char32_t>, char32_t> conv;
    std::string bytes = conv.to_bytes(s);
    return std::u16string(reinterpret_cast<const char16_t*>(bytes.c_str()), bytes.length() / sizeof(char16_t));
}
*/
NS_FK_END
