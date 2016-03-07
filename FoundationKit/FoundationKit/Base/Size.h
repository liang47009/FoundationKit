/****************************************************************************
Copyright (c) 2015 libo All rights reserved.

losemymind.libo@gmail.com

****************************************************************************/
#ifndef FOUNDATIONKIT_BASE_SIZE_H
#define FOUNDATIONKIT_BASE_SIZE_H

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

    Size& operator= (const Size& other)
    {
        setSize(other.width, other.height);
        return *this;
    }

    Size& operator= (const Vector2& point)
    {
        setSize(point.x, point.y);
        return *this;
    }

    Size operator+(const Size& right) const
    {
        return Size(this->width + right.width, this->height + right.height);
    }

    Size operator-(const Size& right) const
    {
        return Size(this->width - right.width, this->height - right.height);
    }

    Size operator*(float a) const
    {
        return Size(this->width * a, this->height * a);
    }

    Size operator/(float a) const
    {
        FKASSERT(a != 0, "CCSize division by 0.");
        return Size(this->width / a, this->height / a);
    }

    void setSize(float width, float height)
    {
        this->width = width;
        this->height = height;
    }

    bool equals(const Size& target) const
    {
        return (fabs(this->width - target.width)  < FLT_EPSILON)
            && (fabs(this->height - target.height) < FLT_EPSILON);
    }

    static const Size ZERO;
};

const Size Size::ZERO = Size(0, 0);

NS_FK_END


#endif // FOUNDATIONKIT_BASE_SIZE_H


