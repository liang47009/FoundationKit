/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.
 
  losemymind.libo@gmail.com

****************************************************************************/
#ifndef FOUNDATIONKIT_SCOPE_LOCALE_HPP
#define FOUNDATIONKIT_SCOPE_LOCALE_HPP
#include <locale>
#include "FoundationKit/GenericPlatformMacros.hpp"
NS_FK_BEGIN
class scope_locale 
{
public:
    scope_locale(const char *_Locname = "")
    {
        old_loc = std::locale::global(std::locale(_Locname));
    }

    ~scope_locale()
    {
        std::locale::global(old_loc);
    }

private:
    std::locale old_loc;
};
NS_FK_END
#endif // FOUNDATIONKIT_SCOPE_LOCALE_HPP
