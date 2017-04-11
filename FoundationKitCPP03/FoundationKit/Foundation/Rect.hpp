/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.
 
  losemymind.libo@gmail.com

****************************************************************************/
#ifndef FOUNDATIONKIT_RECT_HPP
#define FOUNDATIONKIT_RECT_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <math.h>
#include <functional>
#include "FoundationKit/GenericPlatformMacros.hpp"
#include "FoundationKit/Foundation/Vector2.hpp"
#include "FoundationKit/Foundation/Size.hpp"

NS_FK_BEGIN

class Rect
{
public:
    Vector2 origin;
    Size    size;

public:

    Rect();

    Rect(float x, float y, float width, float height);

    Rect(const Rect& other);

    Rect& operator= (const Rect& other);

    void setRect(float x, float y, float width, float height);

    float getMinX() const; /// return the leftmost x-value of current rect

    float getMidX() const; /// return the midpoint x-value of current rect

    float getMaxX() const; /// return the rightmost x-value of current rect

    float getMinY() const; /// return the bottommost y-value of current rect

    float getMidY() const; /// return the midpoint y-value of current rect

    float getMaxY() const; /// return the topmost y-value of current rect

    bool equals(const Rect& rect) const;

    bool containsPoint(const Vector2& point) const;

    bool intersectsRect(const Rect& rect) const;

    Rect unionWithRect(const Rect & rect) const;
    
    static const Rect ZERO;
};
NS_FK_END

#endif // FOUNDATIONKIT_RECT_HPP
