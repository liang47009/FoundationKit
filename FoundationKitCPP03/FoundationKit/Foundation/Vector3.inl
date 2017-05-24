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

#ifndef FOUNDATIONKIT_VECTOR3_INL
#define FOUNDATIONKIT_VECTOR3_INL

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include "FoundationKit/Foundation/Vector3.hpp"

NS_FK_BEGIN

inline const Vector3 Vector3::operator+(const Vector3& v) const
{
    Vector3 result(*this);
    result.Add(v);
    return result;
}

inline Vector3& Vector3::operator+=(const Vector3& v)
{
    Add(v);
    return *this;
}

inline const Vector3 Vector3::operator-(const Vector3& v) const
{
    Vector3 result(*this);
    result.Subtract(v);
    return result;
}

inline Vector3& Vector3::operator-=(const Vector3& v)
{
    Subtract(v);
    return *this;
}

inline const Vector3 Vector3::operator-() const
{
    Vector3 result(*this);
    result.Negate();
    return result;
}

inline const Vector3 Vector3::operator*(float x) const
{
    Vector3 result(*this);
    result.Scale(x);
    return result;
}

inline Vector3& Vector3::operator*=(float x)
{
    Scale(x);
    return *this;
}

inline const Vector3 Vector3::operator/(const float x) const
{
    return Vector3(this->x / x, this->y / x, this->z / x);
}

inline bool Vector3::operator<(const Vector3& v) const
{
    if (x == v.x)
    {
        if (y == v.y)
        {
            return z < v.z;
        }
        return y < v.y;
    }
    return x < v.x;
}

inline bool Vector3::operator==(const Vector3& v) const
{
    return x==v.x && y==v.y && z==v.z;
}

inline bool Vector3::operator!=(const Vector3& v) const
{
    return x!=v.x || y!=v.y || z!=v.z;
}

inline const Vector3 operator*(float x, const Vector3& v)
{
    Vector3 result(v);
    result.Scale(x);
    return result;
}

NS_FK_END
#endif // FOUNDATIONKIT_VECTOR3_INL

