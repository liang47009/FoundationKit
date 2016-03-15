/****************************************************************************
Copyright (c) 2015 libo All rights reserved.

losemymind.libo@gmail.com

****************************************************************************/
#pragma once
#include "FoundationKit/FoundationKitDefines.h"

NS_FK_BEGIN
//  Private move constructor and move assignment ensure classes derived from
//  class nonmovable cannot be copied.

//  Contributed by libo

namespace nonmovable_  // protection from unintended ADL
{
#if (CPP_TARGET_VERSION < CPP_VERSION_11)
    class nonmovable
    {
    protected:
        nonmovable(){};
        ~nonmovable() {}
    private:
        nonmovable(nonmovable&&);
        nonmovable& operator=(nonmovable&&);

    };
#else
    class nonmovable
    {
    protected:
        constexpr nonmovable() = default;
        ~nonmovable() = default;
        nonmovable(nonmovable&&) = delete;
        nonmovable& operator=(nonmovable&&) = delete;

    };
#endif
}

typedef nonmovable_::nonmovable nonmovable;

NS_FK_END








