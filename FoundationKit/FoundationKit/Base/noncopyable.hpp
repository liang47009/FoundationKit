/****************************************************************************
  Copyright (c) 2015 libo All rights reserved.
 
  losemymind.libo@gmail.com

****************************************************************************/
#pragma once
#include "FoundationKit/FoundationKitDefines.h"

NS_FK_BEGIN
//  Private copy constructor and copy assignment ensure classes derived from
//  class noncopyable cannot be copied.

//  Contributed by libo

namespace noncopyable_  // protection from unintended ADL
{
#if (CPP_TARGET_VERSION < CPP_VERSION_11)

    class noncopyable
    {
    protected:

        noncopyable() {}
        ~noncopyable() {}
    private:  // emphasize the following members are private
        noncopyable(const noncopyable&);
        noncopyable& operator=(const noncopyable&);
    };
#else
    class noncopyable
    {
    protected:
        constexpr noncopyable() = default;
        ~noncopyable() = default;
        noncopyable(const noncopyable&) = delete;
        noncopyable& operator=(const noncopyable&) = delete;
    };
#endif
}

typedef noncopyable_::noncopyable noncopyable;

NS_FK_END
