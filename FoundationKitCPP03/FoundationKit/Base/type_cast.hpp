/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.
 
  losemymind.libo@gmail.com

****************************************************************************/
#ifndef FOUNDATIONKIT_TYPE_CAST_HPP
#define FOUNDATIONKIT_TYPE_CAST_HPP

#include <string>
#include <stdexcept>
#include "FoundationKit/GenericPlatformMacros.hpp"

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
            ASSERTED(false,"argument is invalid.");
            return false;
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

}

template<typename To, typename From>
typename std::enable_if<!std::is_same<To, From>::value, To>::type lexical_cast(const From& from)
{
    return detail::converter<To, From>::convert(from);
}

template<typename To, typename From>
typename std::enable_if<std::is_same<To, From>::value, To>::type lexical_cast(const From& from)
{
    return from;
}

NS_FK_END
#endif // FOUNDATIONKIT_TYPE_CAST_HPP



