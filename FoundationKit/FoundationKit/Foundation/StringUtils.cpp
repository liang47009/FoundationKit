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
	const static unsigned int MAX_LENGTH = 54;
    // Pass one greater needed size to leave room for NULL terminator
    std::vector<char> dynamicBuffer(MAX_LENGTH+1);
    char* result = &dynamicBuffer[0];
	va_list arglist;
    va_start(arglist, format);
    // Pass one greater needed size to leave room for NULL terminator
    int needed = vsnprintf(result, MAX_LENGTH+1, format, arglist);
    va_end(arglist);
    if (needed >= MAX_LENGTH)
    {
        // Pass one greater needed size to leave room for NULL terminator
        dynamicBuffer.resize(needed+1);
        result = &dynamicBuffer[0];
        va_start(arglist, format);
        // Pass one greater needed size to leave room for NULL terminator
        needed = vsnprintf(result, needed+1, format, arglist);
        va_end(arglist);
    }
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


bool StringUtils::UTF8ToUTF16(const std::string& utf8, std::u16string& outUtf16)
{
    if (utf8.empty())
    {
        outUtf16.clear();
        return true;
    }

    const size_t utf16Bytes = (utf8.length() + 1) * sizeof(char16_t);
    char16_t* utf16 = (char16_t*)malloc(utf16Bytes);
	if (utf16 == nullptr) return false;
    memset(utf16, 0, utf16Bytes);
    char* utf16ptr = reinterpret_cast<char*>(utf16);
    const UTF8* error = nullptr;
	bool ret = false;
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
