/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.
 
  losemymind.libo@gmail.com

****************************************************************************/
#ifndef FOUNDATIONKIT_STRINGUTILS_HPP
#define FOUNDATIONKIT_STRINGUTILS_HPP

#pragma once
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include "FoundationKit/GenericPlatformMacros.hpp"

NS_FK_BEGIN

class StringUtils
{
public:
static std::string Tolower(const std::string& str);

static std::string Toupper(const std::string& str);

static std::string Format(const char* format, ...);

// trim from start
static std::string &LTrim(std::string &s);

// trim from end
static std::string &RTrim(std::string &s);

// trim from both ends
static std::string &Trim(std::string &s);

/*
 * std::string str = "|aaaa|bbbb|cccc|dddd|";
 * auto splitVec = StringUtils::Split(str, '|');
 * splitVec[1]="aaaa";
 * splitVec[2]="bbbb";
 * splitVec[3]="cccc";
 * splitVec[4]="dddd";
 */
static std::vector<std::string> Split(const std::string &s, char delim, std::vector<std::string> &elems);

static std::vector<std::string> Split(const std::string &s, char delim);

static std::vector<std::string> Split(const std::string &s, std::string delim);
	
static std::string Join(std::string delim, const std::vector<std::string>& values);

/**
 * @brief Converts basic numeric types to strings.
 * @param[in]  val         The basic numeric.
 * @param[in]  precision   Number of precision.
 * @usage tostring(val);
 */
template<class _Val>
static std::wstring Towstring(_Val val,int precision = 2)
{
	std::wostringstream oss;
	oss << std::fixed << std::setprecision(precision) << val;
	return oss.str();
}

template<class _Val>
static std::string Tostring(_Val val, int precision = 2)
{
	std::ostringstream oss;
	oss << std::fixed << std::setprecision(precision) << val;
	return oss.str();
}

/**
 * @brief Converts boolean types to strings.
 * @usage tostring(val);
 */
static std::string Tostring(bool val)
{
	return val ? "true" : "false";
}

static bool IsNumber(const std::string& val);

static std::string wstring2UTF8string(const std::wstring &input);

static std::wstring string2UTF8wstring(const std::string &input);

static std::wstring string2wstring(std::string s)
{
	std::wstring ws;
	ws.assign(s.begin(), s.end());
	return ws;
}

static std::string wstring2string(std::wstring ws)
{
	std::string s;
	s.assign(ws.begin(), ws.end());
	return s;
}

static unsigned char* ToUnsigned(char* p)
{
	return reinterpret_cast<unsigned char*>(p);
}

static unsigned char* ToUnsigned(std::string& str)
{
	return reinterpret_cast<unsigned char*>(&*str.begin());
}


/**
 *  @brief Converts from UTF8 string to UTF16 string.
 *
 *  This function resizes \p outUtf16 to required size and
 *  fill its contents with result UTF16 string if conversion success.
 *  If conversion fails it guarantees not to change \p outUtf16.
 *
 *  @param inUtf8 The source UTF8 string to be converted from.
 *  @param outUtf16 The output string to hold the result UTF16s.
 *  @return True if succeed, otherwise false.
 *  @note Please check the return value before using \p outUtf16
 *  e.g.
 *  @code
 *    std::u16string utf16;
 *    bool ret = StringUtils::UTF8ToUTF16("ÄãºÃhello", utf16);
 *    if (ret) {
 *        do_some_thing_with_utf16(utf16);
 *    }
 *  @endcode
 */
static bool UTF8ToUTF16(const std::string& inUtf8, std::u16string& outUtf16);

/**
 *  @brief Same as \a UTF8ToUTF16 but converts form UTF8 to UTF32.
 *
 *  @see UTF8ToUTF16
 */
static bool UTF8ToUTF32(const std::string& inUtf8, std::u32string& outUtf32);

/**
 *  @brief Same as \a UTF8ToUTF16 but converts form UTF16 to UTF8.
 *
 *  @see UTF8ToUTF16
 */
static bool UTF16ToUTF8(const std::u16string& inUtf16, std::string& outUtf8);

/**
 *  @brief Same as \a UTF8ToUTF16 but converts form UTF16 to UTF32.
 *
 *  @see UTF8ToUTF16
 */
static bool UTF16ToUTF32(const std::u16string& inUtf16, std::u32string& outUtf32);

/**
 *  @brief Same as \a UTF8ToUTF16 but converts form UTF32 to UTF8.
 *
 *  @see UTF8ToUTF16
 */
static bool UTF32ToUTF8(const std::u32string& inUtf32, std::string& outUtf8);

/**
 *  @brief Same as \a UTF8ToUTF16 but converts form UTF32 to UTF16.
 *
 *  @see UTF8ToUTF16
 */
static bool UTF32ToUTF16(const std::u32string& inUtf32, std::u16string& outUtf16);

/**
static std::u16string UTF8ToUTF16(const std::string &s);
static std::u32string UTF8ToUTF32(const std::string &s);
static std::string    UTF16ToUTF8(const std::u16string &s);
static std::u32string UTF16ToUTF32(const std::u16string &s);
static std::string    UTF32ToUTF8(const std::u32string &s);
static std::u16string UTF32ToUTF16(const std::u32string &s);
*/

};

NS_FK_END

#endif // FOUNDATIONKIT_STRINGUTILS_HPP
