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
#include "FoundationKit/Base/scope_locale.hpp"
#include "FoundationKit/Foundation/StringUtils.hpp"
#include "FoundationKit/external/ConvertUTF/ConvertUTF.h"

NS_FK_BEGIN

std::string StringUtils::Tolower( const std::string& str )
{
	std::string ret;
    std::string::const_iterator chIter = str.begin();
    for (chIter; chIter != str.end();++chIter)
    {
        char ch = *chIter;
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
    std::string::const_iterator chIter = str.begin();
    for (chIter; chIter != str.end();++chIter)
    {
        char ch = *chIter;
		if (ch >= 'a' && ch <= 'z')
			ret += ch ^ 32;
		else
			ret += ch;
	}
	return ret;
}

std::string StringUtils::Format( const char* format, ... )
{
	const static unsigned int MAX_STRING_LENGTH = 64;
	va_list arglist;
	int size = MAX_STRING_LENGTH;
	std::vector<char> dynamicBuffer(MAX_STRING_LENGTH);
	char* str = &dynamicBuffer[0];
	for (;;)
	{
        va_start(arglist, format);
        // Pass one less than size to leave room for NULL terminator
        int needed = vsnprintf(str, size - 1, format, arglist);
        va_end(arglist);

		// NOTE: Some platforms return -1 when vsnprintf runs out of room, while others return
		// the number of characters actually needed to fill the buffer.
		if (needed >= 0 && needed < size)
		{
			// Successfully wrote buffer. Added a NULL terminator in case it wasn't written.
			str[needed] = '\0';
			break;
		}
        if (needed < 0)
        {
            return format;
        }
		size = needed > 0 ? (needed + 1) : (size * 2);
		dynamicBuffer.resize(size);
		str = &dynamicBuffer[0];
	}
	return str;
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
	std::stringstream ss(s);
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

std::string StringUtils::Join(std::string delim, std::vector<std::string>& values)
{
	std::string ret;
	size_t index = 0;
	size_t valuesSize = values.size();
    std::vector<std::string>::iterator iter = values.begin();
    for (iter; iter!= values.end();++iter)
	{
		++index;
		ret.append(*iter);
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
    if (mbs_length == ((size_t)-1))
    {
        return result;
    }
    assert(mbs_length > 0);
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
    if (wcs_length == ((size_t)-1))
    {
        return result;
    }

    assert(wcs_length > 0);
    std::vector<wchar_t> wcs_v(wcs_length+1);
    // Now, convert.
    if (mbstowcs(&wcs_v[0], input.c_str(), wcs_length) == ((size_t)-1))
    {
        return result;
    }
    result = &wcs_v[0];
    return result;
}


bool StringUtils::UTF8ToUTF16(const std::string& utf8, std::u16string& outUtf16)
{
    if (utf8.empty())
    {
        outUtf16.clear();
        return true;
    }

    bool ret = false;

    const size_t utf16Bytes = (utf8.length() + 1) * sizeof(char16_t);
    char16_t* utf16 = (char16_t*)malloc(utf16Bytes);
    memset(utf16, 0, utf16Bytes);

    char* utf16ptr = reinterpret_cast<char*>(utf16);
    const UTF8* error = nullptr;

    if (llvm::ConvertUTF8toWide(2, utf8, utf16ptr, error))
    {
        outUtf16 = utf16;
        ret = true;
    }
    free(utf16);
    return ret;
}

bool StringUtils::UTF16ToUTF8(const std::u16string& utf16, std::string& outUtf8)
{
    if (utf16.empty())
    {
        outUtf8.clear();
        return true;
    }

    return llvm::convertUTF16ToUTF8String(utf16, outUtf8);
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
