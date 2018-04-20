/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.
 
  losemymind.libo@gmail.com

****************************************************************************/
#ifndef FOUNDATIONKIT_TYPE_CAST_HPP
#define FOUNDATIONKIT_TYPE_CAST_HPP

#include <string>
#include <type_traits>
#include "FoundationKit/GenericPlatformMacros.hpp"

#if(PLATFORM_ANDROID) && !defined(__clang__)
#include <stdexcept>
#include <sstream>
#include <iomanip>
#include <stdlib.h>
#include <wchar.h>
#include <errno.h>
#define _STRTO_LL	strtoll
#define _STRTO_ULL	strtoull 
#define _STRTO_F	strtod
#define _STRTO_LD	strtod

#define _WCSTO_LL	wcstol
#define _WCSTO_ULL	wcstoul
#define _WCSTO_F	wcstod
#define _WCSTO_LD	wcstod
namespace std
{
    inline int stoi(const string& _Str, size_t *_Idx = 0,
        int _Base = 10)
    {	// convert string to int
        const char *_Ptr = _Str.c_str();
        char *_Eptr;
        errno = 0;
        long _Ans = ::strtol(_Ptr, &_Eptr, _Base);

        if (_Ptr == _Eptr)
            throw std::invalid_argument("invalid stoi argument");
        if (errno == ERANGE || _Ans < INT_MIN || INT_MAX < _Ans)
            throw std::out_of_range("stoi argument out of range");
        if (_Idx != 0)
            *_Idx = (size_t)(_Eptr - _Ptr);
        return ((int)_Ans);
    }

    inline long stol(const string& _Str, size_t *_Idx = 0,
        int _Base = 10)
    {	// convert string to long
        const char *_Ptr = _Str.c_str();
        char *_Eptr;
        errno = 0;
        long _Ans = ::strtol(_Ptr, &_Eptr, _Base);

        if (_Ptr == _Eptr)
            throw std::invalid_argument("invalid stol argument");
        if (errno == ERANGE)
            throw std::out_of_range("stol argument out of range");
        if (_Idx != 0)
            *_Idx = (size_t)(_Eptr - _Ptr);
        return (_Ans);
    }

    inline unsigned long stoul(const string& _Str, size_t *_Idx = 0,
        int _Base = 10)
    {	// convert string to unsigned long
        const char *_Ptr = _Str.c_str();
        char *_Eptr;
        errno = 0;
        unsigned long _Ans = ::strtoul(_Ptr, &_Eptr, _Base);

        if (_Ptr == _Eptr)
            throw std::invalid_argument("invalid stoul argument");
        if (errno == ERANGE)
            throw std::out_of_range("stoul argument out of range");
        if (_Idx != 0)
            *_Idx = (size_t)(_Eptr - _Ptr);
        return (_Ans);
    }

    inline long long stoll(const string& _Str, size_t *_Idx = 0,
        int _Base = 10)
    {	// convert string to long long
        const char *_Ptr = _Str.c_str();
        char *_Eptr;
        errno = 0;
        long long _Ans = _STRTO_LL(_Ptr, &_Eptr, _Base);

        if (_Ptr == _Eptr)
            throw std::invalid_argument("invalid stoll argument");
        if (errno == ERANGE)
            throw std::out_of_range("stoll argument out of range");
        if (_Idx != 0)
            *_Idx = (size_t)(_Eptr - _Ptr);
        return (_Ans);
    }

    inline unsigned long long stoull(const string& _Str, size_t *_Idx = 0,
        int _Base = 10)
    {	// convert string to unsigned long long
        const char *_Ptr = _Str.c_str();
        errno = 0;
        char *_Eptr;
        unsigned long long _Ans = _STRTO_ULL(_Ptr, &_Eptr, _Base);

        if (_Ptr == _Eptr)
            throw std::invalid_argument("invalid stoull argument");
        if (errno == ERANGE)
            throw std::out_of_range("stoull argument out of range");
        if (_Idx != 0)
            *_Idx = (size_t)(_Eptr - _Ptr);
        return (_Ans);
    }

    inline float stof(const string& _Str, size_t *_Idx = 0)
    {	// convert string to float
        const char *_Ptr = _Str.c_str();
        errno = 0;
        char *_Eptr;
        float _Ans = (float)::_STRTO_F(_Ptr, &_Eptr);

        if (_Ptr == _Eptr)
            throw std::invalid_argument("invalid stof argument");
        if (errno == ERANGE)
            throw std::out_of_range("stof argument out of range");
        if (_Idx != 0)
            *_Idx = (size_t)(_Eptr - _Ptr);
        return (_Ans);
    }

    inline double stod(const string& _Str, size_t *_Idx = 0)
    {	// convert string to double
        const char *_Ptr = _Str.c_str();
        errno = 0;
        char *_Eptr;
        double _Ans = ::strtod(_Ptr, &_Eptr);

        if (_Ptr == _Eptr)
            throw std::invalid_argument("invalid stod argument");
        if (errno == ERANGE)
            throw std::out_of_range("stod argument out of range");
        if (_Idx != 0)
            *_Idx = (size_t)(_Eptr - _Ptr);
        return (_Ans);
    }

    inline long double stold(const string& _Str, size_t *_Idx = 0)
    {	// convert string to long double
        const char *_Ptr = _Str.c_str();
        errno = 0;
        char *_Eptr;
        long double _Ans = ::_STRTO_LD(_Ptr, &_Eptr);

        if (_Ptr == _Eptr)
            throw std::invalid_argument("invalid stold argument");
        if (errno == ERANGE)
            throw std::out_of_range("stold argument out of range");
        if (_Idx != 0)
            *_Idx = (size_t)(_Eptr - _Ptr);
        return (_Ans);
    }

    // sto* WIDE CONVERSIONS
    inline int stoi(const wstring& _Str, size_t *_Idx = 0,
        int _Base = 10)
    {	// convert wstring to int
        const wchar_t *_Ptr = _Str.c_str();
        wchar_t *_Eptr;
        errno = 0;
        long _Ans = ::wcstol(_Ptr, &_Eptr, _Base);

        if (_Ptr == _Eptr)
            throw std::invalid_argument("invalid stoi argument");
        if (errno == ERANGE || _Ans < INT_MIN || INT_MAX < _Ans)
            throw std::out_of_range("stoi argument out of range");
        if (_Idx != 0)
            *_Idx = (size_t)(_Eptr - _Ptr);
        return ((int)_Ans);
    }

    inline long stol(const wstring& _Str, size_t *_Idx = 0,
        int _Base = 10)
    {	// convert wstring to long
        const wchar_t *_Ptr = _Str.c_str();
        wchar_t *_Eptr;
        errno = 0;
        long _Ans = ::wcstol(_Ptr, &_Eptr, _Base);

        if (_Ptr == _Eptr)
            throw std::invalid_argument("invalid stol argument");
        if (errno == ERANGE)
            throw std::out_of_range("stol argument out of range");
        if (_Idx != 0)
            *_Idx = (size_t)(_Eptr - _Ptr);
        return (_Ans);
    }

    inline unsigned long stoul(const wstring& _Str, size_t *_Idx = 0,
        int _Base = 10)
    {	// convert wstring to unsigned long
        const wchar_t *_Ptr = _Str.c_str();
        wchar_t *_Eptr;
        errno = 0;
        unsigned long _Ans = ::wcstoul(_Ptr, &_Eptr, _Base);

        if (_Ptr == _Eptr)
            throw std::invalid_argument("invalid stoul argument");
        if (errno == ERANGE)
            throw std::out_of_range("stoul argument out of range");
        if (_Idx != 0)
            *_Idx = (size_t)(_Eptr - _Ptr);
        return (_Ans);
    }

    inline long long stoll(const wstring& _Str, size_t *_Idx = 0,
        int _Base = 10)
    {	// convert wstring to long long
        const wchar_t *_Ptr = _Str.c_str();
        wchar_t *_Eptr;
        errno = 0;
        long long _Ans = _WCSTO_LL(_Ptr, &_Eptr, _Base);

        if (_Ptr == _Eptr)
            throw std::invalid_argument("invalid stoll argument");
        if (errno == ERANGE)
            throw std::out_of_range("stoll argument out of range");
        if (_Idx != 0)
            *_Idx = (size_t)(_Eptr - _Ptr);
        return (_Ans);
    }

    inline unsigned long long stoull(const wstring& _Str, size_t *_Idx = 0,
        int _Base = 10)
    {	// convert wstring to unsigned long long
        const wchar_t *_Ptr = _Str.c_str();
        errno = 0;
        wchar_t *_Eptr;
        unsigned long long _Ans = _WCSTO_ULL(_Ptr, &_Eptr, _Base);

        if (_Ptr == _Eptr)
            throw std::invalid_argument("invalid stoull argument");
        if (errno == ERANGE)
            throw std::out_of_range("stoull argument out of range");
        if (_Idx != 0)
            *_Idx = (size_t)(_Eptr - _Ptr);
        return (_Ans);
    }

    inline float stof(const wstring& _Str, size_t *_Idx = 0)
    {	// convert wstring to float
        const wchar_t *_Ptr = _Str.c_str();
        errno = 0;
        wchar_t *_Eptr;
        float _Ans = (float)::_WCSTO_F(_Ptr, &_Eptr);

        if (_Ptr == _Eptr)
            throw std::invalid_argument("invalid stof argument");
        if (errno == ERANGE)
            throw std::out_of_range("stof argument out of range");
        if (_Idx != 0)
            *_Idx = (size_t)(_Eptr - _Ptr);
        return (_Ans);
    }

    inline double stod(const wstring& _Str, size_t *_Idx = 0)
    {	// convert wstring to double
        const wchar_t *_Ptr = _Str.c_str();
        errno = 0;
        wchar_t *_Eptr;
        double _Ans = ::wcstod(_Ptr, &_Eptr);

        if (_Ptr == _Eptr)
            throw std::invalid_argument("invalid stod argument");
        if (errno == ERANGE)
            throw std::out_of_range("stod argument out of range");
        if (_Idx != 0)
            *_Idx = (size_t)(_Eptr - _Ptr);
        return (_Ans);
    }

    inline long double stold(const wstring& _Str, size_t *_Idx = 0)
    {	// convert wstring to long double
        const wchar_t *_Ptr = _Str.c_str();
        errno = 0;
        wchar_t *_Eptr;
        long double _Ans = ::_WCSTO_LD(_Ptr, &_Eptr);

        if (_Ptr == _Eptr)
            throw std::invalid_argument("invalid stold argument");
        if (errno == ERANGE)
            throw std::out_of_range("stold argument out of range");
        if (_Idx != 0)
            *_Idx = (size_t)(_Eptr - _Ptr);
        return (_Ans);
    }

    template<class _Val>
    inline std::wstring to_wstring(_Val val/*, int precision = 2*/)
    {
        std::wostringstream oss;
        oss << /*std::fixed << std::setprecision(precision) <<*/ val;
        return oss.str();
    }

    template<class _Val>
    inline std::string to_string(_Val val/*, int precision = 2*/)
    {
        std::ostringstream oss;
        oss << /*std::fixed << std::setprecision(precision) <<*/ val;
        return oss.str();
    }
}
#endif // #if(PLATFORM_ANDROID)

NS_FK_BEGIN

namespace detail
{
    template<typename To, typename From>
    struct converter
    {

    };

    template<typename From>
    struct converter<int,From >
    {
        static int convert(const From& from)
        {
            return std::stoi(from);
        }
    };

    template<typename From>
    struct converter < unsigned int, From >
    {
        static unsigned int convert(const From& from)
        {
            return std::stoi(from);
        }
    };

    template<typename From>
    struct converter < long, From >
    {
        static long convert(const From& from)
        {
            return std::stol(from);
        }
    };

    template<typename From>
    struct converter < unsigned long, From >
    {
        static unsigned long convert(const From& from)
        {
            return std::stoul(from);
        }
    };

    template<typename From>
    struct converter < long long, From >
    {
        static long long convert(const From& from)
        {
            return std::stoll(from);
        }
    };

    template<typename From>
    struct converter < unsigned long long, From >
    {
        static unsigned long long convert(const From& from)
        {
            return std::stoull(from);
        }
    };

    template<typename From>
    struct converter < float, From >
    {
        static float convert(const From& from)
        {
            return std::stof(from);
        }
    };

    template<typename From>
    struct converter < double, From >
    {
        static double convert(const From& from)
        {
            return std::stod(from);
        }
    };

    template<typename From>
    struct converter < long double, From >
    {
        static long double convert(const From& from)
        {
            return std::stold(from);
        }
    };

    template<>
    struct converter < std::string, bool >
    {
        static std::string convert(bool from)
        {
            return (from ? "true" : "false");
        }
    };

    template<typename From>
    struct converter < std::string, From >
    {
        static std::string convert(const From& from)
        {
            return std::to_string(from);
        }
    };

    template<>
    struct converter < std::wstring, bool >
    {
        static std::wstring convert(bool from)
        {
            return (from ? L"true" : L"false");
        }
    };

    template<typename From>
    struct converter < std::wstring, From >
    {
        static std::wstring convert(const From& from)
        {
            return std::to_wstring(from);
        }
    };

    template<>
    struct converter < bool, std::string >
    {
        static bool convert(const std::string& from)
        {
            if (from.compare("true") == 0) 
                return true;
            if (from.compare("false") == 0)
                return false;
            throw std::invalid_argument("argument is invalid.");
        }
    };

    template<>
    struct converter < bool, char* > :public converter < bool, std::string > {};

    template<>
    struct converter < bool, const char* > :public converter < bool, std::string > {};

    template<unsigned N>
    struct converter < bool, char[N] > :public converter < bool, std::string > {};

    template<unsigned N>
    struct converter < bool, const char[N] > :public converter < bool, std::string > {};
} // namespace detail

template<typename To, typename From>
typename std::enable_if<!std::is_same<To, From>::value, To>::type 
lexical_cast(const From& from)
{
    return detail::converter<To, From>::convert(from);
}

template<typename To, typename From>
typename std::enable_if<std::is_same<To, From>::value, To>::type 
lexical_cast(const From& from)
{
    return from;
}

template<typename To, typename From>
To lexical_pointer_cast(From from)
{
    return *static_cast<To*>(static_cast<void*>(&from));
}


NS_FK_END
#endif // FOUNDATIONKIT_TYPE_CAST_HPP



