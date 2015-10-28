#pragma once
#include <math.h>
#include <functional>
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

    Size();

    explicit Size(const Vector2& point);

    Size(float width, float height);

    Size(const Size& other);

    Size& operator= (const Size& other);

    Size& operator= (const Vector2& point);

    Size operator+(const Size& right) const;

    Size operator-(const Size& right) const;

    Size operator*(float a) const;

    Size operator/(float a) const;

    void setSize(float width, float height);

    bool equals(const Size& target) const;
    
    static const Size ZERO;
};

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