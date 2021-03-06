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

#ifndef LOSEMYMIND_VECTOR4_INL
#define LOSEMYMIND_VECTOR4_INL

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include "FoundationKit/Foundation/Vector4.hpp"

NS_FK_BEGIN

inline const Vector4 Vector4::operator+(const Vector4& v) const
{
    Vector4 result(*this);
    result.Add(v);
    return result;
}

inline Vector4& Vector4::operator+=(const Vector4& v)
{
    Add(v);
    return *this;
}

inline const Vector4 Vector4::operator-(const Vector4& v) const
{
    Vector4 result(*this);
    result.Subtract(v);
    return result;
}

inline Vector4& Vector4::operator-=(const Vector4& v)
{
    Subtract(v);
    return *this;
}

inline const Vector4 Vector4::operator-() const
{
    Vector4 result(*this);
    result.Negate();
    return result;
}

inline const Vector4 Vector4::operator*(float x) const
{
    Vector4 result(*this);
    result.Scale(x);
    return result;
}

inline Vector4& Vector4::operator*=(float x)
{
    Scale(x);
    return *this;
}

inline const Vector4 Vector4::operator/(const float x) const
{
    return Vector4(this->X / x, this->Y / x, this->Z / x, this->W / x);
}

inline bool Vector4::operator<(const Vector4& v) const
{
    if (X == v.X)
    {
        if (Y == v.Y)
        {
            if (Z == v.Z)
            {
                return W < v.W;
            }
            return Z < v.Z;
        }
        return Y < v.Y;
    }
    return X < v.X;
}

inline bool Vector4::operator==(const Vector4& v) const
{
    return X==v.X && Y==v.Y && Z==v.Z && W==v.W;
}

inline bool Vector4::operator!=(const Vector4& v) const
{
    return X!=v.X || Y!=v.Y || Z!=v.Z || W!=v.W;
}

inline const Vector4 operator*(float x, const Vector4& v)
{
    Vector4 result(v);
    result.Scale(x);
    return result;
}

NS_FK_END
#endif // LOSEMYMIND_VECTOR4_INL



