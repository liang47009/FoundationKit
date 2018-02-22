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
#include "FoundationKit/Math/Vector2.hpp"
#include "FoundationKit/Math/Size.hpp"

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

    void SetRect(float x, float y, float width, float height);

    float GetMinX() const; /// return the leftmost x-value of current rect

    float GetMidX() const; /// return the midpoint x-value of current rect

    float GetMaxX() const; /// return the rightmost x-value of current rect

    float GetMinY() const; /// return the bottommost y-value of current rect

    float GetMidY() const; /// return the midpoint y-value of current rect

    float GetMaxY() const; /// return the topmost y-value of current rect

    bool Equals(const Rect& rect) const;

    bool ContainsPoint(const Vector2& point) const;

    bool IntersectsRect(const Rect& rect) const;

    Rect UnionWithRect(const Rect & rect) const;
    
    bool IsZero()
    {
        return (origin == Vector2::ZERO && size.IsZero());
    }
};
NS_FK_END

#endif // FOUNDATIONKIT_RECT_HPP
