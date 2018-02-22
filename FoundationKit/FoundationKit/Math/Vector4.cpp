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
#include "FoundationKit/Math/Vector4.hpp"

NS_FK_BEGIN

Vector4 Vector4::ZERO  =  Vector4();
Vector4 Vector4::UNIT = Vector4(1.f, 1.f, 1.f, 1.f);
Vector4 Vector4::UNITX = Vector4(1.f, 0.f, 0.f, 0.f);
Vector4 Vector4::UNITY = Vector4(0.f, 1.f, 0.f, 0.f);
Vector4 Vector4::UNITZ = Vector4(0.f, 0.f, 1.f, 0.f);
Vector4 Vector4::UNITW = Vector4(0.f, 0.f, 0.f, 1.f);

Vector4::Vector4()
    : X(0.0f), Y(0.0f), Z(0.0f), W(0.0f)
{
}

Vector4::Vector4(float x, float y, float z, float w)
    : X(x), Y(y), Z(z), W(w)
{
}

Vector4::Vector4(const float* src)
{
    Set(src);
}

Vector4::Vector4(const Vector4& p1, const Vector4& p2)
{
    Set(p1, p2);
}

Vector4::Vector4(const Vector4& copy)
{
    Set(copy);
}

Vector4 Vector4::FromColor(unsigned int color)
{
    float components[4];
    int componentIndex = 0;
    for (int i = 3; i >= 0; --i)
    {
        int component = (color >> i*8) & 0x000000ff;

        components[componentIndex++] = static_cast<float>(component) / 255.0f;
    }

    Vector4 value(components);
    return value;
}

Vector4::~Vector4()
{
}

const Vector4& Vector4::Zero()
{
    static Vector4 value(0.0f, 0.0f, 0.0f, 0.0f);
    return value;
}

const Vector4& Vector4::One()
{
    static Vector4 value(1.0f, 1.0f, 1.0f, 1.0f);
    return value;
}

const Vector4& Vector4::UnitX()
{
    static Vector4 value(1.0f, 0.0f, 0.0f, 0.0f);
    return value;
}

const Vector4& Vector4::UnitY()
{
    static Vector4 value(0.0f, 1.0f, 0.0f, 0.0f);
    return value;
}

const Vector4& Vector4::UnitZ()
{
    static Vector4 value(0.0f, 0.0f, 1.0f, 0.0f);
    return value;
}

const Vector4& Vector4::UnitW()
{
    static Vector4 value(0.0f, 0.0f, 0.0f, 1.0f);
    return value;
}

bool Vector4::IsZero() const
{
    return X == 0.0f && Y == 0.0f && Z == 0.0f && W == 0.0f;
}

bool Vector4::IsOne() const
{
    return X == 1.0f && Y == 1.0f && Z == 1.0f && W == 1.0f;
}

float Vector4::Angle(const Vector4& v1, const Vector4& v2)
{
    float dx = v1.W * v2.X - v1.X * v2.W - v1.Y * v2.Z + v1.Z * v2.Y;
    float dy = v1.W * v2.Y - v1.Y * v2.W - v1.Z * v2.X + v1.X * v2.Z;
    float dz = v1.W * v2.Z - v1.Z * v2.W - v1.X * v2.Y + v1.Y * v2.X;

    return atan2f(sqrt(dx * dx + dy * dy + dz * dz) + Math::SmailFloat, Dot(v1, v2));
}

void Vector4::Add(const Vector4& v)
{
    X += v.X;
    Y += v.Y;
    Z += v.Z;
    W += v.W;
}

void Vector4::Add(const Vector4& v1, const Vector4& v2, Vector4* dst)
{
    assert(dst);

    dst->X = v1.X + v2.X;
    dst->Y = v1.Y + v2.Y;
    dst->Z = v1.Z + v2.Z;
    dst->W = v1.W + v2.W;
}

void Vector4::Clamp(const Vector4& min, const Vector4& max)
{
    assert(!(min.X > max.X || min.Y > max.Y || min.Z > max.Z || min.W > max.W));

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

    // Clamp the Z value.
    if (W < min.W)
        W = min.W;
    if (W > max.W)
        W = max.W;
}

void Vector4::Clamp(const Vector4& v, const Vector4& min, const Vector4& max, Vector4* dst)
{
    assert(dst);
    assert(!(min.X > max.X || min.Y > max.Y || min.Z > max.Z || min.W > max.W));

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

    // Clamp the W value.
    dst->W = v.W;
    if (dst->W < min.W)
        dst->W = min.W;
    if (dst->W > max.W)
        dst->W = max.W;
}

float Vector4::Distance(const Vector4& v) const
{
    float dx = v.X - X;
    float dy = v.Y - Y;
    float dz = v.Z - Z;
    float dw = v.W - W;

    return sqrt(dx * dx + dy * dy + dz * dz + dw * dw);
}

float Vector4::DistanceSquared(const Vector4& v) const
{
    float dx = v.X - X;
    float dy = v.Y - Y;
    float dz = v.Z - Z;
    float dw = v.W - W;

    return (dx * dx + dy * dy + dz * dz + dw * dw);
}

float Vector4::Dot(const Vector4& v) const
{
    return (X * v.X + Y * v.Y + Z * v.Z + W * v.W);
}

float Vector4::Dot(const Vector4& v1, const Vector4& v2)
{
    return (v1.X * v2.X + v1.Y * v2.Y + v1.Z * v2.Z + v1.W * v2.W);
}

float Vector4::Length() const
{
    return sqrt(X * X + Y * Y + Z * Z + W * W);
}


float Vector4::LengthSquared() const
{
    return (X * X + Y * Y + Z * Z + W * W);
}

void Vector4::Negate()
{
    X = -X;
    Y = -Y;
    Z = -Z;
    W = -W;
}

Vector4& Vector4::Normalize()
{
    Normalize(this);
    return *this;
}

void Vector4::Normalize(Vector4* dst) const
{
    assert(dst);

    if (dst != this)
    {
        dst->X = X;
        dst->Y = Y;
        dst->Z = Z;
        dst->W = W;
    }

    float n = X * X + Y * Y + Z * Z + W * W;
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
    dst->W *= n;
}

void Vector4::Scale(float scalar)
{
    X *= scalar;
    Y *= scalar;
    Z *= scalar;
    W *= scalar;
}

void Vector4::Set(float x, float y, float z, float w)
{
    this->X = x;
    this->Y = y;
    this->Z = z;
    this->W = w;
}

void Vector4::Set(const float* array)
{
    assert(array);

    X = array[0];
    Y = array[1];
    Z = array[2];
    W = array[3];
}

void Vector4::Set(const Vector4& v)
{
    this->X = v.X;
    this->Y = v.Y;
    this->Z = v.Z;
    this->W = v.W;
}

void Vector4::Set(const Vector4& p1, const Vector4& p2)
{
    X = p2.X - p1.X;
    Y = p2.Y - p1.Y;
    Z = p2.Z - p1.Z;
    W = p2.W - p1.W;
}

void Vector4::Subtract(const Vector4& v)
{
    X -= v.X;
    Y -= v.Y;
    Z -= v.Z;
    W -= v.W;
}

void Vector4::Subtract(const Vector4& v1, const Vector4& v2, Vector4* dst)
{
    assert(dst);

    dst->X = v1.X - v2.X;
    dst->Y = v1.Y - v2.Y;
    dst->Z = v1.Z - v2.Z;
    dst->W = v1.W - v2.W;
}
NS_FK_END
