/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.
 
  losemymind.libo@gmail.com

****************************************************************************/
#ifndef FOUNDATIONKIT_NONCOPYABLE_HPP
#define FOUNDATIONKIT_NONCOPYABLE_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include "FoundationKit/GenericPlatformMacros.hpp"

NS_FK_BEGIN
//  Private copy constructor and copy assignment ensure classes derived from
//  class noncopyable cannot be copied.

//  Contributed by libo

namespace noncopyable_  // protection from unintended ADL
{
    class noncopyable
    {
        noncopyable(const noncopyable&) = delete;              // no copy constructor
        noncopyable& operator=(const noncopyable&) = delete;   // no assignment operator
	protected:
		noncopyable() noexcept = default;
    };
}

typedef noncopyable_::noncopyable noncopyable;

NS_FK_END

#endif // FOUNDATIONKIT_NONCOPYABLE_HPP
