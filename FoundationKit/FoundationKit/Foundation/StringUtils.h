/****************************************************************************
Copyright (c) 2013-2014 libo.

losemymind.libo@gmail.com

****************************************************************************/
#pragma once
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include "FoundationKit/FoundationKitDefines.h"

NS_FK_BEGIN

class StringUtils
{
public:
	typedef std::basic_string < unsigned char, std::char_traits<unsigned char>, std::allocator< unsigned char> > ustring;

	static std::string tolower(const std::string& str);

	static std::string toupper(const std::string& str);

	static std::string format(const char* format, ...);

	// trim from start
	static std::string &ltrim(std::string &s);

	// trim from end
	static std::string &rtrim(std::string &s);

	// trim from both ends
	static std::string &trim(std::string &s);

	static std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems);

	static std::vector<std::string> split(const std::string &s, char delim);
    
    static std::string join(std::string delim, std::vector<std::string> values);

   /**
	* @brief Converts basic numeric types to strings.
	* @param[in]  val         The basic numeric.
	* @param[in]  percision   Number of precision.
	* @usage tostring(val);
	*/
	template<class _Val>
	static std::string to_string(_Val val,int precision = 2)
	{
		std::ostringstream oss;
		oss << std::fixed << std::setprecision(precision) << val;
		return oss.str();
	}

	/**
	* @brief Converts boolean types to strings.
	* @usage tostring(val);
	*/
	static std::string to_string(bool val)
	{
		return val ? "true" : "false";
	}

	static bool isNumber(const std::string& val);

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

	static unsigned char* to_unsigned(char* p)
	{
		return reinterpret_cast<unsigned char*>(p);
	}

	static unsigned char* to_unsigned(std::string& str)
	{
		return reinterpret_cast<unsigned char*>(&*str.begin());
	}
};

NS_FK_END