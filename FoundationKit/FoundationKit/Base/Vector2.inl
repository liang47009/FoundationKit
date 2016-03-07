/**
 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

 http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.

 Original file from GamePlay3D: http://gameplay3d.org

 This file was modified to fit the FoundationKit project
 */
#include "Vector2.h"

#ifndef LOSEMYMIND_VECTOR2_INL
#define LOSEMYMIND_VECTOR2_INL

NS_FK_BEGIN

inline const Vector2 Vector2::operator+(const Vector2& v) const
{
    Vector2 result(*this);
    result.add(v);
    return result;
}

inline Vector2& Vector2::operator+=(const Vector2& v)
{
    add(v);
    return *this;
}

inline const Vector2 Vector2::operator-(const Vector2& v) const
{
    Vector2 result(*this);
    result.subtract(v);
    return result;
}

inline Vector2& Vector2::operator-=(const Vector2& v)
{
    subtract(v);
    return *this;
}

inline const Vector2 Vector2::operator-() const
{
    Vector2 result(*this);
    result.negate();
    return result;
}

inline const Vector2 Vector2::operator*(float x) const
{
    Vector2 result(*this);
    result.scale(x);
    return result;
}

inline Vector2& Vector2::operator*=(float x)
{
    scale(x);
    return *this;
}

inline const Vector2 Vector2::operator/(const float x) const
{
    return Vector2(this->x / x, this->y / x);
}

inline bool Vector2::operator<(const Vector2& v) const
{
    if (x == v.x)
    {
        return y < v.y;
    }
    return x < v.x;
}

inline bool Vector2::operator==(const Vector2& v) const
{
    return x==v.x && y==v.y;
}

inline bool Vector2::operator!=(const Vector2& v) const
{
    return x!=v.x || y!=v.y;
}

inline const Vector2 operator*(float x, const Vector2& v)
{
    Vector2 result(v);
    result.scale(x);
    return result;
}

NS_FK_END
#endif // LOSEMYMIND_VECTOR2_INL
