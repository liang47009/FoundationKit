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
#include "Vector4.h"
#ifndef LOSEMYMIND_VECTOR4_INL
#define LOSEMYMIND_VECTOR4_INL

NS_FK_BEGIN

inline const Vector4 Vector4::operator+(const Vector4& v) const
{
    Vector4 result(*this);
    result.add(v);
    return result;
}

inline Vector4& Vector4::operator+=(const Vector4& v)
{
    add(v);
    return *this;
}

inline const Vector4 Vector4::operator-(const Vector4& v) const
{
    Vector4 result(*this);
    result.subtract(v);
    return result;
}

inline Vector4& Vector4::operator-=(const Vector4& v)
{
    subtract(v);
    return *this;
}

inline const Vector4 Vector4::operator-() const
{
    Vector4 result(*this);
    result.negate();
    return result;
}

inline const Vector4 Vector4::operator*(float x) const
{
    Vector4 result(*this);
    result.scale(x);
    return result;
}

inline Vector4& Vector4::operator*=(float x)
{
    scale(x);
    return *this;
}

inline const Vector4 Vector4::operator/(const float x) const
{
    return Vector4(this->x / x, this->y / x, this->z / x, this->w / x);
}

inline bool Vector4::operator<(const Vector4& v) const
{
    if (x == v.x)
    {
        if (y == v.y)
        {
            if (z == v.z)
            {
                return w < v.w;
            }
            return z < v.z;
        }
        return y < v.y;
    }
    return x < v.x;
}

inline bool Vector4::operator==(const Vector4& v) const
{
    return x==v.x && y==v.y && z==v.z && w==v.w;
}

inline bool Vector4::operator!=(const Vector4& v) const
{
    return x!=v.x || y!=v.y || z!=v.z || w!=v.w;
}

inline const Vector4 operator*(float x, const Vector4& v)
{
    Vector4 result(v);
    result.scale(x);
    return result;
}

NS_FK_END
#endif // LOSEMYMIND_VECTOR4_INL



