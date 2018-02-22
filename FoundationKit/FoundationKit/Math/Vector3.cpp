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
#include "FoundationKit/Math/MathContent.hpp"
#include "FoundationKit/Math/MathUtil.hpp"
#include "FoundationKit/Math/Vector3.hpp"


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
    : X(0.0f), Y(0.0f), Z(0.0f)
{
}

Vector3::Vector3(float x, float y, float z)
    : X(x), Y(y), Z(z)
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
    return X == 0.0f && Y == 0.0f && Z == 0.0f;
}

bool Vector3::IsOne() const
{
    return X == 1.0f && Y == 1.0f && Z == 1.0f;
}

float Vector3::Angle(const Vector3& v1, const Vector3& v2)
{
    float dx = v1.Y * v2.Z - v1.Z * v2.Y;
    float dy = v1.Z * v2.X - v1.X * v2.Z;
    float dz = v1.X * v2.Y - v1.Y * v2.X;

    return atan2f(sqrt(dx * dx + dy * dy + dz * dz) + Math::SmailFloat, Dot(v1, v2));
}

void Vector3::Add(const Vector3& v)
{
    X += v.X;
    Y += v.Y;
    Z += v.Z;
}

void Vector3::Add(const Vector3& v1, const Vector3& v2, Vector3* dst)
{
    assert(dst);

    dst->X = v1.X + v2.X;
    dst->Y = v1.Y + v2.Y;
    dst->Z = v1.Z + v2.Z;
}

void Vector3::Clamp(const Vector3& min, const Vector3& max)
{
    assert(!(min.X > max.X || min.Y > max.Y || min.Z > max.Z));

    // Clamp the X value.
    if (X < min.X)
        X = min.X;
    if (X > max.X)
        X = max.X;

    // Clamp the Y value.
    if (Y < min.Y)
        Y = min.Y;
    if (Y > max.Y)
        Y = max.Y;

    // Clamp the Z value.
    if (Z < min.Z)
        Z = min.Z;
    if (Z > max.Z)
        Z = max.Z;
}

void Vector3::Clamp(const Vector3& v, const Vector3& min, const Vector3& max, Vector3* dst)
{
    assert(dst);
    assert(!(min.X > max.X || min.Y > max.Y || min.Z > max.Z));

    // Clamp the X value.
    dst->X = v.X;
    if (dst->X < min.X)
        dst->X = min.X;
    if (dst->X > max.X)
        dst->X = max.X;

    // Clamp the Y value.
    dst->Y = v.Y;
    if (dst->Y < min.Y)
        dst->Y = min.Y;
    if (dst->Y > max.Y)
        dst->Y = max.Y;

    // Clamp the Z value.
    dst->Z = v.Z;
    if (dst->Z < min.Z)
        dst->Z = min.Z;
    if (dst->Z > max.Z)
        dst->Z = max.Z;
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
    MathUtil::CrossVector3(&v1.X, &v2.X, &dst->X);
}

float Vector3::Distance(const Vector3& v) const
{
    float dx = v.X - X;
    float dy = v.Y - Y;
    float dz = v.Z - Z;

    return sqrt(dx * dx + dy * dy + dz * dz);
}

float Vector3::DistanceSquared(const Vector3& v) const
{
    float dx = v.X - X;
    float dy = v.Y - Y;
    float dz = v.Z - Z;

    return (dx * dx + dy * dy + dz * dz);
}

float Vector3::Dot(const Vector3& v) const
{
    return (X * v.X + Y * v.Y + Z * v.Z);
}

float Vector3::Dot(const Vector3& v1, const Vector3& v2)
{
    return (v1.X * v2.X + v1.Y * v2.Y + v1.Z * v2.Z);
}

float Vector3::Length() const
{
    return sqrt(X * X + Y * Y + Z * Z);
}

float Vector3::LengthSquared() const
{
    return (X * X + Y * Y + Z * Z);
}

void Vector3::Negate()
{
    X = -X;
    Y = -Y;
    Z = -Z;
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
        dst->X = X;
        dst->Y = Y;
        dst->Z = Z;
    }

    float n = X * X + Y * Y + Z * Z;
    // Already normalized.
    if (n == 1.0f)
        return;

    n = sqrt(n);
    // Too close to zero.
    if (n < Math::Tolerance)
        return;

    n = 1.0f / n;
    dst->X *= n;
    dst->Y *= n;
    dst->Z *= n;
}

void Vector3::Scale(float scalar)
{
    X *= scalar;
    Y *= scalar;
    Z *= scalar;
}

void Vector3::Set(float x, float y, float z)
{
    this->X = x;
    this->Y = y;
    this->Z = z;
}

void Vector3::Set(const float* array)
{
    assert(array);

    X = array[0];
    Y = array[1];
    Z = array[2];
}

void Vector3::Set(const Vector3& v)
{
    this->X = v.X;
    this->Y = v.Y;
    this->Z = v.Z;
}

void Vector3::Set(const Vector3& p1, const Vector3& p2)
{
    X = p2.X - p1.X;
    Y = p2.Y - p1.Y;
    Z = p2.Z - p1.Z;
}

void Vector3::Subtract(const Vector3& v)
{
    X -= v.X;
    Y -= v.Y;
    Z -= v.Z;
}

void Vector3::Subtract(const Vector3& v1, const Vector3& v2, Vector3* dst)
{
    assert(dst);

    dst->X = v1.X - v2.X;
    dst->Y = v1.Y - v2.Y;
    dst->Z = v1.Z - v2.Z;
}

void Vector3::Smooth(const Vector3& target, float elapsedTime, float responseTime)
{
    if (elapsedTime > 0)
    {
        *this += (target - *this) * (elapsedTime / (elapsedTime + responseTime));
    }
}

NS_FK_END
