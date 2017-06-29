/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.
 
  losemymind.libo@gmail.com

****************************************************************************/
#ifndef FOUNDATIONKIT_NONMOVABLE_HPP
#define FOUNDATIONKIT_NONMOVABLE_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include "FoundationKit/GenericPlatformMacros.hpp"

NS_FK_BEGIN
//  Private move constructor and move assignment ensure classes derived from
//  class nonmovable cannot be copied.
//  Contributed by libo

namespace nonmovable_  // protection from unintended ADL
{
    class nonmovable
    {
        nonmovable(nonmovable&&) = delete;
        nonmovable& operator=(nonmovable&&) = delete;
    protected:
        nonmovable() noexcept = default;

    };
}

typedef nonmovable_::nonmovable nonmovable;

NS_FK_END

#endif // FOUNDATIONKIT_NONMOVABLE_HPP
