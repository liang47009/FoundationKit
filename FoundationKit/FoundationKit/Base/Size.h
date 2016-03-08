/****************************************************************************
Copyright (c) 2015 libo All rights reserved.

losemymind.libo@gmail.com

****************************************************************************/

#ifndef LOSEMYMIND_SIZE_H
#define LOSEMYMIND_SIZE_H

#pragma once
#include <math.h>
#include <float.h>
#include "Vector2.h"

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
        setSize(other.width, other.height);
        return *this;
    }

    inline Size& operator= (const Vector2& point)
    {
        setSize(point.x, point.y);
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
        FKASSERT(a != 0, "CCSize division by 0.");
        return Size(this->width / a, this->height / a);
    }

    void setSize(float width, float height);

    bool equals(const Size& target) const;

    static const Size ZERO;
};

NS_FK_END
#endif // LOSEMYMIND_SIZE_H





