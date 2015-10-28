/****************************************************************************
  Copyright (c) 2013-2014 libo.
 
  losemymind.libo@gmail.com

****************************************************************************/
#pragma once
#include "FoundationKit/FoundationKitDefines.h"

NS_FK_BEGIN
class noncopyable
{
protected:
    noncopyable() = default;
    ~noncopyable() = default;
    noncopyable( const noncopyable& ) = delete;
    noncopyable& operator=( const noncopyable& ) = delete;
};
NS_FK_END
