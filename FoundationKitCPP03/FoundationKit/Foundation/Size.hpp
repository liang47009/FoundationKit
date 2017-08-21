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
    float Width;
    float Height;
public:
    operator Vector2() const
    {
        return Vector2(Width, Height);
    }

public:

    Size() : Width(0), Height(0){}

    Size(float w, float h) : Width(w), Height(h){}

    explicit Size(const Vector2& point) : Width(point.X), Height(point.Y){}

    Size(const Size& other) : Width(other.Width), Height(other.Height){}

    inline Size& operator= (const Size& other)
    {
        SetSize(other.Width, other.Height);
        return *this;
    }

    inline Size& operator= (const Vector2& point)
    {
        SetSize(point.X, point.Y);
        return *this;
    }

    inline Size operator+(const Size& right) const
    {
        return Size(this->Width + right.Width, this->Height + right.Height);
    }

    inline Size operator-(const Size& right) const
    {
        return Size(this->Width - right.Width, this->Height - right.Height);
    }

    inline Size operator*(float a) const
    {
        return Size(this->Width * a, this->Height * a);
    }

    inline Size operator/(float a) const
    {
        return Size(this->Width / a, this->Height / a);
    }

    void SetSize(float width, float height)
    {
        this->Width = width;
        this->Height = height;
    }

    bool Equals(const Size& target) const
    {
        return (fabs(this->Width - target.Width) < FLT_EPSILON)
            && (fabs(this->Height - target.Height) < FLT_EPSILON);
    }

    bool IsZero()
    {
        return (Width == 0 && Height == 0);
    }
};

NS_FK_END

#endif // FOUNDATIONKIT_SIZE_HPP
