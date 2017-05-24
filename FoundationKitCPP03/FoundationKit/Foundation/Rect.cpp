/****************************************************************************
 Copyright (c) 2015 libo All rights reserved.
 
 losemymind.libo@gmail.com
 
 ****************************************************************************/
#include <algorithm>
#include <assert.h>
#include <float.h>
#include "FoundationKit/Foundation/Rect.hpp"

NS_FK_BEGIN

// implementation of Rect

Rect::Rect(void)
{
    SetRect(0.0f, 0.0f, 0.0f, 0.0f);
}

Rect::Rect(float x, float y, float width, float height)
{
    SetRect(x, y, width, height);
}

Rect::Rect(const Rect& other)
{
    SetRect(other.origin.x, other.origin.y, other.size.width, other.size.height);
}

Rect& Rect::operator= (const Rect& other)
{
    SetRect(other.origin.x, other.origin.y, other.size.width, other.size.height);
    return *this;
}

void Rect::SetRect(float x, float y, float width, float height)
{
    origin.x = x;
    origin.y = y;
    size.width = width;
    size.height = height;
}

bool Rect::Equals(const Rect& rect) const
{
    return ((origin == rect.origin) && 
            size.Equals(rect.size));
}

float Rect::GetMaxX() const
{
    return origin.x + size.width;
}

float Rect::GetMidX() const
{
    return origin.x + size.width / 2.0f;
}

float Rect::GetMinX() const
{
    return origin.x;
}

float Rect::GetMaxY() const
{
    return origin.y + size.height;
}

float Rect::GetMidY() const
{
    return origin.y + size.height / 2.0f;
}

float Rect::GetMinY() const
{
    return origin.y;
}

bool Rect::ContainsPoint(const Vector2& point) const
{
    bool bRet = false;

    if (point.x >= GetMinX() && point.x <= GetMaxX()
        && point.y >= GetMinY() && point.y <= GetMaxY())
    {
        bRet = true;
    }

    return bRet;
}

bool Rect::IntersectsRect(const Rect& rect) const
{
    return !(     GetMaxX() < rect.GetMinX() ||
             rect.GetMaxX() <      GetMinX() ||
                  GetMaxY() < rect.GetMinY() ||
             rect.GetMaxY() <      GetMinY());
}

Rect Rect::UnionWithRect(const Rect & rect) const
{
    float thisLeftX   = origin.x;
    float thisRightX  = origin.x + size.width;
    float thisTopY    = origin.y + size.height;
    float thisBottomY = origin.y;
    
    if (thisRightX < thisLeftX)
    {
        std::swap(thisRightX, thisLeftX);   // This rect has negative width
    }
    
    if (thisTopY < thisBottomY)
    {
        std::swap(thisTopY, thisBottomY);   // This rect has negative height
    }
    
    float otherLeftX   = rect.origin.x;
    float otherRightX  = rect.origin.x + rect.size.width;
    float otherTopY    = rect.origin.y + rect.size.height;
    float otherBottomY = rect.origin.y;
    
    if (otherRightX < otherLeftX)
    {
        std::swap(otherRightX, otherLeftX);   // Other rect has negative width
    }
    
    if (otherTopY < otherBottomY)
    {
        std::swap(otherTopY, otherBottomY);   // Other rect has negative height
    }
    
    float combinedLeftX   = std::min(thisLeftX, otherLeftX);
    float combinedRightX  = std::max(thisRightX, otherRightX);
    float combinedTopY    = std::max(thisTopY, otherTopY);
    float combinedBottomY = std::min(thisBottomY, otherBottomY);
    
    return Rect(combinedLeftX, combinedBottomY, combinedRightX - combinedLeftX, combinedTopY - combinedBottomY);
}

const Rect Rect::ZERO = Rect(0, 0, 0, 0);

NS_FK_END


