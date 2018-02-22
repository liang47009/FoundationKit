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
#include "FoundationKit/Math/Vector2.hpp"

NS_FK_BEGIN

Vector2 Vector2::ZERO  = Vector2(0.f, 0.f);
Vector2 Vector2::UNIT  = Vector2(1.f, 1.f);
Vector2 Vector2::UNITX = Vector2(1.f, 0.f);
Vector2 Vector2::UNITY = Vector2(0.f, 1.f);

Vector2::Vector2()
    : X(0.0f), Y(0.0f)
{
}

Vector2::Vector2(float x, float y)
    : X(x), Y(y)
{
}

Vector2::Vector2(const float* array)
{
    Set(array);
}

Vector2::Vector2(const Vector2& p1, const Vector2& p2)
{
    Set(p1, p2);
}

Vector2::Vector2(const Vector2& copy)
{
    Set(copy);
}

Vector2::~Vector2()
{
}

const Vector2& Vector2::Zero()
{
    static Vector2 value(0.0f, 0.0f);
    return value;
}

const Vector2& Vector2::One()
{
    static Vector2 value(1.0f, 1.0f);
    return value;
}

const Vector2& Vector2::UnitX()
{
    static Vector2 value(1.0f, 0.0f);
    return value;
}

const Vector2& Vector2::UnitY()
{
    static Vector2 value(0.0f, 1.0f);
    return value;
}

bool Vector2::IsZero() const
{
    return X == 0.0f && Y == 0.0f;
}

bool Vector2::IsOne() const
{
    return X == 1.0f && Y == 1.0f;
}

float Vector2::Angle(const Vector2& v1, const Vector2& v2)
{
    float dz = v1.X * v2.Y - v1.Y * v2.X;
    return atan2f(fabsf(dz) + Math::SmailFloat, Dot(v1, v2));
}

void Vector2::Add(const Vector2& v)
{
    X += v.X;
    Y += v.Y;
}

void Vector2::Add(const Vector2& v1, const Vector2& v2, Vector2* dst)
{
    assert(dst);

    dst->X = v1.X + v2.X;
    dst->Y = v1.Y + v2.Y;
}

void Vector2::Clamp(const Vector2& min, const Vector2& max)
{
    assert(!(min.X > max.X || min.Y > max.Y ));

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
}

void Vector2::Clamp(const Vector2& v, const Vector2& min, const Vector2& max, Vector2* dst)
{
    assert(dst);
    assert(!(min.X > max.X || min.Y > max.Y ));

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
}

float Vector2::Distance(const Vector2& v) const
{
    float dx = v.X - X;
    float dy = v.Y - Y;

    return sqrt(dx * dx + dy * dy);
}

float Vector2::DistanceSquared(const Vector2& v) const
{
    float dx = v.X - X;
    float dy = v.Y - Y;
    return (dx * dx + dy * dy);
}

float Vector2::Dot(const Vector2& v) const
{
    return (X * v.X + Y * v.Y);
}

float Vector2::Dot(const Vector2& v1, const Vector2& v2)
{
    return (v1.X * v2.X + v1.Y * v2.Y);
}

float Vector2::Length() const
{
    return sqrt(X * X + Y * Y);
}

float Vector2::LengthSquared() const
{
    return (X * X + Y * Y);
}

void Vector2::Negate()
{
    X = -X;
    Y = -Y;
}

Vector2& Vector2::Normalize()
{
    Normalize(this);
    return *this;
}

void Vector2::Normalize(Vector2* dst) const
{
    assert(dst);

    if (dst != this)
    {
        dst->X = X;
        dst->Y = Y;
    }

    float n = X * X + Y * Y;
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
}

void Vector2::Scale(float scalar)
{
    X *= scalar;
    Y *= scalar;
}

void Vector2::Scale(const Vector2& scale)
{
    X *= scale.X;
    Y *= scale.Y;
}

void Vector2::Rotate(const Vector2& point, float angle)
{
    float sinAngle = sin(angle);
    float cosAngle = cos(angle);

    if (point.IsZero())
    {
        float tempX = X * cosAngle - Y * sinAngle;
        Y = Y * cosAngle + X * sinAngle;
        X = tempX;
    }
    else
    {
        float tempX = X - point.X;
        float tempY = Y - point.Y;

        X = tempX * cosAngle - tempY * sinAngle + point.X;
        Y = tempY * cosAngle + tempX * sinAngle + point.Y;
    }
}

void Vector2::Set(float x, float y)
{
    this->X = x;
    this->Y = y;
}

void Vector2::Set(const float* array)
{
    assert(array);

    X = array[0];
    Y = array[1];
}

void Vector2::Set(const Vector2& v)
{
    this->X = v.X;
    this->Y = v.Y;
}

void Vector2::Set(const Vector2& p1, const Vector2& p2)
{
     X = p2.X - p1.X;
     Y = p2.Y - p1.Y;
}

void Vector2::Subtract(const Vector2& v)
{
    X -= v.X;
    Y -= v.Y;
}

void Vector2::Subtract(const Vector2& v1, const Vector2& v2, Vector2* dst)
{
    assert(dst);

    dst->X = v1.X - v2.X;
    dst->Y = v1.Y - v2.Y;
}

void Vector2::Smooth(const Vector2& target, float elapsedTime, float responseTime)
{
    if (elapsedTime > 0)
    {
        *this += (target - *this) * (elapsedTime / (elapsedTime + responseTime));
    }
}

Vector2 Vector2::MoveTowards(Vector2 current, Vector2 target, float maxDistanceDelta)
{
    Vector2 a = target - current;
    float magnitude = a.Length();
    if (magnitude <= maxDistanceDelta || magnitude == 0.f)
    {
        return target;
    }
    return current + a / magnitude * maxDistanceDelta;
}




NS_FK_END
