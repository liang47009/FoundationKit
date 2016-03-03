/****************************************************************************
  Copyright (c) 2015 libo All rights reserved.

  losemymind.libo@gmail.com

****************************************************************************/
#include <locale>
#include <stdarg.h>
#include <algorithm>
#include <functional>
#include <cctype>
#include <sstream>
#include "StringUtils.h"
#include "FoundationKit/external/ConvertUTF/ConvertUTF.h"

NS_FK_BEGIN

std::string StringUtils::tolower( const std::string& str )
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

std::string StringUtils::toupper( const std::string& str )
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

std::string StringUtils::format( const char* format, ... )
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
		size = needed > 0 ? (needed + 1) : (size * 2);
		dynamicBuffer.resize(size);
		str = &dynamicBuffer[0];
	}
	return str;
}

std::string & StringUtils::ltrim( std::string &s )
{
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
	return s;
}

std::string & StringUtils::rtrim( std::string &s )
{
	s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
	return s;
}

std::string & StringUtils::trim( std::string &s )
{
	return ltrim(rtrim(s));
}

std::vector<std::string> & StringUtils::split( const std::string &s, char delim, std::vector<std::string> &elems )
{
	std::stringstream ss(s);
	std::string item;
	while (std::getline(ss, item, delim))
	{
		elems.push_back(item);
	}
	return elems;
}

std::vector<std::string> StringUtils::split( const std::string &s, char delim )
{
	std::vector<std::string> elems;
	split(s, delim, elems);
	return elems;
}

std::string StringUtils::join(std::string delim, std::vector<std::string> values)
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

bool StringUtils::isNumber( const std::string& val )
{
	std::stringstream str(val);
	float tst;
	str >> tst;
	return !str.fail() && str.eof();
}

// by Marius Bancila
// http://www.codeguru.com/cpp/misc/misc/multi-lingualsupport/article.php/c10451/
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
std::string StringUtils::wstring2UTF8string(const std::wstring &input)
{
	std::string output;

	for(unsigned int i=0; i < input.size(); i++)
	{
		// 0xxxxxxx
		if(input[i] < 0x80)
		{
			output.push_back((byte)input[i]);
		}
		// 110xxxxx 10xxxxxx
		else if(input[i] < 0x800)
		{
			output.push_back((byte)(MASK2BYTES | input[i] >> 6));
			output.push_back((byte)(MASKBYTE   | (input[i] & MASKBITS)));
		}
		// 1110xxxx 10xxxxxx 10xxxxxx
		else if(input[i] < 0x10000)
		{
			output.push_back((byte)(MASK3BYTES | input[i] >> 12));
			output.push_back((byte)(MASKBYTE   | (input[i] >> 6 & MASKBITS)));
			output.push_back((byte)(MASKBYTE   | (input[i] & MASKBITS)));
		}
		// 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
		else if(input[i] < 0x200000)
		{
			output.push_back((byte)(MASK4BYTES | input[i] >> 18));
			output.push_back((byte)(MASKBYTE   | (input[i] >> 12 & MASKBITS)));
			output.push_back((byte)(MASKBYTE   | (input[i] >> 6 & MASKBITS)));
			output.push_back((byte)(MASKBYTE   | (input[i] & MASKBITS)));
		}
		// 111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
		else if(input[i] < 0x4000000)
		{
			output.push_back((byte)(MASK5BYTES | input[i] >> 24));
			output.push_back((byte)(MASKBYTE   | (input[i] >> 18 & MASKBITS)));
			output.push_back((byte)(MASKBYTE   | (input[i] >> 12 & MASKBITS)));
			output.push_back((byte)(MASKBYTE   | (input[i] >> 6 & MASKBITS)));
			output.push_back((byte)(MASKBYTE   | (input[i] & MASKBITS)));
		}
		// 1111110x 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
		else if(input[i] < 0x8000000)
		{
			output.push_back((byte)(MASK6BYTES | input[i] >> 30));
			output.push_back((byte)(MASKBYTE   | (input[i] >> 18 & MASKBITS)));
			output.push_back((byte)(MASKBYTE   | (input[i] >> 12 & MASKBITS)));
			output.push_back((byte)(MASKBYTE   | (input[i] >> 6 & MASKBITS)));
			output.push_back((byte)(MASKBYTE   | (input[i] & MASKBITS)));
		}
	}
	return output;
}

std::wstring StringUtils::string2UTF8wstring(const std::string &input)
{
	std::wstring output;
	for(unsigned int i=0; i < input.size();)
	{
		Unicode4Bytes ch;

		// 1111110x 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
		if((input[i] & MASK6BYTES) == MASK6BYTES)
		{
			ch = ((input[i] & 0x01) << 30) 
				| ((input[i+1] & MASKBITS) << 24)
				| ((input[i+2] & MASKBITS) << 18) 
				| ((input[i+3]& MASKBITS)  << 12)
				| ((input[i+4] & MASKBITS) << 6) 
				| (input[i+5] & MASKBITS);
			i += 6;
		}
		// 111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
		else if((input[i] & MASK5BYTES) == MASK5BYTES)
		{
			ch = ((input[i] & 0x03) << 24) 
				| ((input[i+1] & MASKBITS) << 18)
				| ((input[i+2] & MASKBITS) << 12) 
				| ((input[i+3] & MASKBITS) << 6)
				| (input[i+4] & MASKBITS);
			i += 5;
		}
		// 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
		else if((input[i] & MASK4BYTES) == MASK4BYTES)
		{
			ch = ((input[i] & 0x07) << 18) 
				| ((input[i+1]& MASKBITS)  << 12)
				| ((input[i+2] & MASKBITS) << 6) 
				| (input[i+3] & MASKBITS);
			i += 4;
		}
		// 1110xxxx 10xxxxxx 10xxxxxx
		else if((input[i] & MASK3BYTES) == MASK3BYTES)
		{
			ch = ((input[i] & 0x0F) << 12) 
				| ((input[i+1] & MASKBITS) << 6)
				| (input[i+2] & MASKBITS);
			i += 3;
		}
		// 110xxxxx 10xxxxxx
		else if((input[i] & MASK2BYTES) == MASK2BYTES)
		{
			ch = ((input[i] & 0x1F) << 6) | (input[i+1] & MASKBITS);
			i += 2;
		}
		// 0xxxxxxx
		else //if(input[i] < MASKBYTE)
		{
			ch = input[i];
			i += 1;
		}
		output.push_back(static_cast<wchar_t>(ch));
	}
	return output;
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

NS_FK_END
