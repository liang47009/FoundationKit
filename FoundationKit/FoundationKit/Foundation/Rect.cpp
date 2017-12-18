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
    SetRect(other.origin.X, other.origin.Y, other.size.Width, other.size.Height);
}

Rect& Rect::operator= (const Rect& other)
{
    SetRect(other.origin.X, other.origin.Y, other.size.Width, other.size.Height);
    return *this;
}

void Rect::SetRect(float x, float y, float width, float height)
{
    origin.X    = x;
    origin.Y    = y;
    size.Width  = width;
    size.Height = height;
}

bool Rect::Equals(const Rect& rect) const
{
    return ((origin == rect.origin) && 
            size.Equals(rect.size));
}

float Rect::GetMaxX() const
{
    return origin.Y + size.Width;
}

float Rect::GetMidX() const
{
    return origin.X + size.Width / 2.0f;
}

float Rect::GetMinX() const
{
    return origin.X;
}

float Rect::GetMaxY() const
{
    return origin.Y + size.Height;
}

float Rect::GetMidY() const
{
    return origin.Y + size.Height / 2.0f;
}

float Rect::GetMinY() const
{
    return origin.Y;
}

bool Rect::ContainsPoint(const Vector2& point) const
{
    bool bRet = false;

    if (point.X >= GetMinX() && point.X <= GetMaxX()
        && point.Y >= GetMinY() && point.Y <= GetMaxY())
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
    float thisLeftX   = origin.X;
    float thisRightX  = origin.X + size.Width;
    float thisTopY    = origin.Y + size.Height;
    float thisBottomY = origin.Y;
    
    if (thisRightX < thisLeftX)
    {
        std::swap(thisRightX, thisLeftX);   // This rect has negative width
    }
    
    if (thisTopY < thisBottomY)
    {
        std::swap(thisTopY, thisBottomY);   // This rect has negative height
    }
    
    float otherLeftX   = rect.origin.X;
    float otherRightX  = rect.origin.X + rect.size.Width;
    float otherTopY    = rect.origin.Y + rect.size.Height;
    float otherBottomY = rect.origin.Y;
    
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

NS_FK_END


