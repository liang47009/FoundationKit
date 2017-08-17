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
 * auto splitVec = StringUtils::split(str, '|');
 * splitVec[0]="";
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

/** 
 * Convert std::wstring to std::string, the string code page is UTF-8.
 */
static std::string wstring2UTF8string(const std::wstring &input);

/** 
 * Convert std::string to std::wstring, the string code page is UTF-8.
 */
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
 *  @brief Converts utf8 string to utf16 string.
 *  @param utf8 The utf8 string to be converted.
 *  @param outUtf16 The output utf16 string.
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
static bool UTF8ToUTF16(const std::string& utf8, std::u16string& outUtf16);

/**
 *  @brief Converts utf16 string to utf8 string.
 *  @param utf16 The utf16 string to be converted.
 *  @param outUtf8 The output utf8 string.
 *  @return True if succeed, otherwise false.
 *  @note Please check the return value before using \p outUtf8
 *  e.g.
 *  @code
 *    std::string utf8;
 *    bool ret = StringUtils::UTF16ToUTF8(u"\u4f60\u597d", utf16);
 *    if (ret) {
 *        do_some_thing_with_utf8(utf8);
 *    }
 *  @endcode
 */
static bool UTF16ToUTF8(const std::u16string& utf16, std::string& outUtf8);

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
