/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.
 
  losemymind.libo@gmail.com

****************************************************************************/
#ifndef FOUNDATIONKIT_LOCALE_HPP
#define FOUNDATIONKIT_LOCALE_HPP

#include <locale>

#if defined(__GNUC__) && !defined(__clang__)
#include "bits/codecvt.h"
#else
#include <codecvt>
#endif // #if defined(__GNUC__) 

#include "FoundationKit/GenericPlatformMacros.hpp"
NS_FK_BEGIN


inline std::locale default_locale()
{
    return std::locale("");
/**
# if PLATFORM_WINDOWS
    std::locale global_loc = std::locale("");
    return std::locale(global_loc, new std::codecvt_utf8<wchar_t>);
# elif defined(macintosh) || defined(__APPLE__) || defined(__APPLE_CC__) \
|| defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__HAIKU__)
    std::locale global_loc = std::locale();
    return std::locale(global_loc, new std::codecvt_utf8<wchar_t>);
# else  // Other POSIX
    // ISO C calls std::locale("") "the locale-specific native environment", and this
    // locale is the default for many POSIX-based operating systems such as Linux.
    return std::locale("");
# endif
*/
}

inline std::locale& process_locale()
{
    static std::locale loc(default_locale());
    return loc;
}

class scope_locale
{
public:
    scope_locale(const char *_Locname)
    {
        if (_Locname == nullptr || _Locname[0] == '\0')
            old_loc = std::locale::global(default_locale());
        else
            old_loc = std::locale::global(std::locale(_Locname));
    }

    scope_locale(const std::locale& _LocalVal = default_locale())
    {
        old_loc = std::locale::global(_LocalVal);
    }

    ~scope_locale()
    {
        std::locale::global(old_loc);
    }

private:
    std::locale old_loc;
};
NS_FK_END

#endif // END OF FOUNDATIONKIT_LOCALE_HPP