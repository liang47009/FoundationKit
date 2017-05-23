/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.
 
  losemymind.libo@gmail.com

****************************************************************************/
#ifndef FOUNDATIONKIT_SIZE_HPP
#define FOUNDATIONKIT_SIZE_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <math.h>
#include <float.h>
#include "FoundationKit/Foundation/Vector2.hpp"

NS_FK_BEGIN

class Size
{
public:
    float width;
    float height;
public:
    operator Vector2() const
    {
        return Vector2(width, height);
    }

public:

    Size() : width(0), height(0){}

    Size(float w, float h) : width(w), height(h){}

    explicit Size(const Vector2& point) : width(point.x), height(point.y){}

    Size(const Size& other) : width(other.width), height(other.height){}

    inline Size& operator= (const Size& other)
    {
        SetSize(other.width, other.height);
        return *this;
    }

    inline Size& operator= (const Vector2& point)
    {
        SetSize(point.x, point.y);
        return *this;
    }

    inline Size operator+(const Size& right) const
    {
        return Size(this->width + right.width, this->height + right.height);
    }

    inline Size operator-(const Size& right) const
    {
        return Size(this->width - right.width, this->height - right.height);
    }

    inline Size operator*(float a) const
    {
        return Size(this->width * a, this->height * a);
    }

    inline Size operator/(float a) const
    {
        return Size(this->width / a, this->height / a);
    }

    void SetSize(float width, float height);

    bool Equals(const Size& target) const;

    static const Size ZERO;
};

NS_FK_END

#endif // FOUNDATIONKIT_SIZE_HPP
