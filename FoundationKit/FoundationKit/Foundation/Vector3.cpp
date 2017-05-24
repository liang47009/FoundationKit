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
#include <cassert>
#include "FoundationKit/Foundation/MathContent.hpp"
#include "FoundationKit/Foundation/MathUtil.hpp"
#include "FoundationKit/Foundation/Vector3.hpp"


NS_FK_BEGIN

Vector3 Vector3::ZERO     = Vector3(0.f, 0.f, 0.f);
Vector3 Vector3::ON       = Vector3(1.f, 1.f, 1.f);
Vector3 Vector3::UNITX    = Vector3(1.f, 0.f, 0.f);
Vector3 Vector3::UNITY    = Vector3(0.f, 1.f, 0.f);
Vector3 Vector3::UNITZ    = Vector3(0.f, 0.f, 1.f);
Vector3 Vector3::UP       = Vector3(0.f, 1.f, 0.f);
Vector3 Vector3::DOWN     = Vector3(0.f, -1.f, 0.f);
Vector3 Vector3::RIGHT    = Vector3(1.f, 0.f, 0.f);
Vector3 Vector3::LEFT     = Vector3(-1.f, 0.f, 0.f);
Vector3 Vector3::FORWARD  = Vector3(0.f, 0.f, -1.f);
Vector3 Vector3::BACKWARD = Vector3(0.f, 0.f, 1.f);

Vector3::Vector3()
    : x(0.0f), y(0.0f), z(0.0f)
{
}

Vector3::Vector3(float x, float y, float z)
    : x(x), y(y), z(z)
{
}

Vector3::Vector3(const float* array)
{
    Set(array);
}

Vector3::Vector3(const Vector3& p1, const Vector3& p2)
{
    Set(p1, p2);
}

Vector3::Vector3(const Vector3& copy)
{
    Set(copy);
}

Vector3 Vector3::FromColor(unsigned int color)
{
    float components[3];
    int componentIndex = 0;
    for (int i = 2; i >= 0; --i)
    {
        int component = (color >> i*8) & 0x0000ff;

        components[componentIndex++] = static_cast<float>(component) / 255.0f;
    }

    Vector3 value(components);
    return value;
}

Vector3::~Vector3()
{
}

const Vector3& Vector3::Zero()
{
    static Vector3 value(0.0f, 0.0f, 0.0f);
    return value;
}

const Vector3& Vector3::One()
{
    static Vector3 value(1.0f, 1.0f, 1.0f);
    return value;
}

const Vector3& Vector3::UnitX()
{
    static Vector3 value(1.0f, 0.0f, 0.0f);
    return value;
}

const Vector3& Vector3::UnitY()
{
    static Vector3 value(0.0f, 1.0f, 0.0f);
    return value;
}

const Vector3& Vector3::UnitZ()
{
    static Vector3 value(0.0f, 0.0f, 1.0f);
    return value;
}

bool Vector3::IsZero() const
{
    return x == 0.0f && y == 0.0f && z == 0.0f;
}

bool Vector3::IsOne() const
{
    return x == 1.0f && y == 1.0f && z == 1.0f;
}

float Vector3::Angle(const Vector3& v1, const Vector3& v2)
{
    float dx = v1.y * v2.z - v1.z * v2.y;
    float dy = v1.z * v2.x - v1.x * v2.z;
    float dz = v1.x * v2.y - v1.y * v2.x;

    return atan2f(sqrt(dx * dx + dy * dy + dz * dz) + Math::SmailFloat, Dot(v1, v2));
}

void Vector3::Add(const Vector3& v)
{
    x += v.x;
    y += v.y;
    z += v.z;
}

void Vector3::Add(const Vector3& v1, const Vector3& v2, Vector3* dst)
{
    assert(dst);

    dst->x = v1.x + v2.x;
    dst->y = v1.y + v2.y;
    dst->z = v1.z + v2.z;
}

void Vector3::Clamp(const Vector3& min, const Vector3& max)
{
    assert(!(min.x > max.x || min.y > max.y || min.z > max.z));

    // Clamp the x value.
    if (x < min.x)
        x = min.x;
    if (x > max.x)
        x = max.x;

    // Clamp the y value.
    if (y < min.y)
        y = min.y;
    if (y > max.y)
        y = max.y;

    // Clamp the z value.
    if (z < min.z)
        z = min.z;
    if (z > max.z)
        z = max.z;
}

void Vector3::Clamp(const Vector3& v, const Vector3& min, const Vector3& max, Vector3* dst)
{
    assert(dst);
    assert(!(min.x > max.x || min.y > max.y || min.z > max.z));

    // Clamp the x value.
    dst->x = v.x;
    if (dst->x < min.x)
        dst->x = min.x;
    if (dst->x > max.x)
        dst->x = max.x;

    // Clamp the y value.
    dst->y = v.y;
    if (dst->y < min.y)
        dst->y = min.y;
    if (dst->y > max.y)
        dst->y = max.y;

    // Clamp the z value.
    dst->z = v.z;
    if (dst->z < min.z)
        dst->z = min.z;
    if (dst->z > max.z)
        dst->z = max.z;
}

void Vector3::Cross(const Vector3& v)
{
    Cross(*this, v, this);
}

void Vector3::Cross(const Vector3& v1, const Vector3& v2, Vector3* dst)
{
    assert(dst);

    // NOTE: This code assumes Vector3 struct members are contiguous floats in memory.
    // We might want to revisit this (and other areas of code that make this assumption)
    // later to guarantee 100% safety/compatibility.
    MathUtil::CrossVector3(&v1.x, &v2.x, &dst->x);
}

float Vector3::Distance(const Vector3& v) const
{
    float dx = v.x - x;
    float dy = v.y - y;
    float dz = v.z - z;

    return sqrt(dx * dx + dy * dy + dz * dz);
}

float Vector3::DistanceSquared(const Vector3& v) const
{
    float dx = v.x - x;
    float dy = v.y - y;
    float dz = v.z - z;

    return (dx * dx + dy * dy + dz * dz);
}

float Vector3::Dot(const Vector3& v) const
{
    return (x * v.x + y * v.y + z * v.z);
}

float Vector3::Dot(const Vector3& v1, const Vector3& v2)
{
    return (v1.x * v2.x + v1.y * v2.y + v1.z * v2.z);
}

float Vector3::Length() const
{
    return sqrt(x * x + y * y + z * z);
}

float Vector3::LengthSquared() const
{
    return (x * x + y * y + z * z);
}

void Vector3::Negate()
{
    x = -x;
    y = -y;
    z = -z;
}

Vector3& Vector3::Normalize()
{
    Normalize(this);
    return *this;
}

void Vector3::Normalize(Vector3* dst) const
{
    assert(dst);

    if (dst != this)
    {
        dst->x = x;
        dst->y = y;
        dst->z = z;
    }

    float n = x * x + y * y + z * z;
    // Already normalized.
    if (n == 1.0f)
        return;

    n = sqrt(n);
    // Too close to zero.
    if (n < Math::Tolerance)
        return;

    n = 1.0f / n;
    dst->x *= n;
    dst->y *= n;
    dst->z *= n;
}

void Vector3::Scale(float scalar)
{
    x *= scalar;
    y *= scalar;
    z *= scalar;
}

void Vector3::Set(float x, float y, float z)
{
    this->x = x;
    this->y = y;
    this->z = z;
}

void Vector3::Set(const float* array)
{
    assert(array);

    x = array[0];
    y = array[1];
    z = array[2];
}

void Vector3::Set(const Vector3& v)
{
    this->x = v.x;
    this->y = v.y;
    this->z = v.z;
}

void Vector3::Set(const Vector3& p1, const Vector3& p2)
{
    x = p2.x - p1.x;
    y = p2.y - p1.y;
    z = p2.z - p1.z;
}

void Vector3::Subtract(const Vector3& v)
{
    x -= v.x;
    y -= v.y;
    z -= v.z;
}

void Vector3::Subtract(const Vector3& v1, const Vector3& v2, Vector3* dst)
{
    assert(dst);

    dst->x = v1.x - v2.x;
    dst->y = v1.y - v2.y;
    dst->z = v1.z - v2.z;
}

void Vector3::Smooth(const Vector3& target, float elapsedTime, float responseTime)
{
    if (elapsedTime > 0)
    {
        *this += (target - *this) * (elapsedTime / (elapsedTime + responseTime));
    }
}

NS_FK_END
