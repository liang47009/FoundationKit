/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.
 
  losemymind.libo@gmail.com

****************************************************************************/
#ifndef FOUNDATIONKIT_LOCALE_HPP
#define FOUNDATIONKIT_LOCALE_HPP

#include <locale>
#include "FoundationKit/GenericPlatformMacros.hpp"
NS_FK_BEGIN


inline std::locale default_locale()
{
return std::locale("");
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
        if(_Locname == nullptr)
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