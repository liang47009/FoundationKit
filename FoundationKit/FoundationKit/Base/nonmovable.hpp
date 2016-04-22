/****************************************************************************
Copyright (c) 2015 libo All rights reserved.

losemymind.libo@gmail.com

****************************************************************************/
#pragma once
#include "FoundationKit/GenericPlatformMacros.h"

NS_FK_BEGIN
//  Private move constructor and move assignment ensure classes derived from
//  class nonmovable cannot be copied.

//  Contributed by libo

namespace nonmovable_  // protection from unintended ADL
{
    class nonmovable
    {
    protected:
        constexpr nonmovable() = default;
        ~nonmovable() = default;
        nonmovable(nonmovable&&) = delete;
        nonmovable& operator=(nonmovable&&) = delete;

    };
}

typedef nonmovable_::nonmovable nonmovable;

NS_FK_END








