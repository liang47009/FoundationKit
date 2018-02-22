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
#include <cmath>
#include "FoundationKit/Math/Matrix.hpp"
#include "FoundationKit/Math/Quaternion.hpp"

NS_FK_BEGIN

Quaternion::Quaternion()
    : X(0.0f), Y(0.0f), Z(0.0f), W(1.0f)
{
}

Quaternion::Quaternion(float x, float y, float z, float w)
    : X(x), Y(y), Z(z), W(w)
{
}

Quaternion::Quaternion(float* array)
{
    Set(array);
}

Quaternion::Quaternion(const Matrix& m)
{
    Set(m);
}

Quaternion::Quaternion(const Vector3& axis, float angle)
{
    Set(axis, angle);
}

Quaternion::Quaternion(const Quaternion& copy)
{
    Set(copy);
}

Quaternion::~Quaternion()
{
}

const Quaternion& Quaternion::Identity()
{
    static Quaternion value(0.0f, 0.0f, 0.0f, 1.0f);
    return value;
}

const Quaternion& Quaternion::Zero()
{
    static Quaternion value(0.0f, 0.0f, 0.0f, 0.0f);
    return value;
}

bool Quaternion::IsIdentity() const
{
    return X == 0.0f && Y == 0.0f && Z == 0.0f && W == 1.0f;
}

bool Quaternion::IsZero() const
{
    return X == 0.0f && Y == 0.0f && Z == 0.0f && W == 0.0f;
}

void Quaternion::CreateFromRotationMatrix(const Matrix& m, Quaternion* dst)
{
    m.GetRotation(dst);
}

void Quaternion::CreateFromAxisAngle(const Vector3& axis, float angle, Quaternion* dst)
{
    assert(dst);

    float halfAngle = angle * 0.5f;
    float sinHalfAngle = sinf(halfAngle);

    Vector3 normal(axis);
    normal.Normalize();
    dst->X = normal.X * sinHalfAngle;
    dst->Y = normal.Y * sinHalfAngle;
    dst->Z = normal.Z * sinHalfAngle;
    dst->W = cosf(halfAngle);
}

void Quaternion::Conjugate()
{
    Conjugate(this);
}

void Quaternion::Conjugate(Quaternion* dst) const
{
    assert(dst);

    dst->X = -X;
    dst->Y = -Y;
    dst->Z = -Z;
    dst->W =  W;
}

bool Quaternion::Inverse()
{
    return Inverse(this);
}

bool Quaternion::Inverse(Quaternion* dst) const
{
    assert(dst);

    float n = X * X + Y * Y + Z * Z + W * W;
    if (n == 1.0f)
    {
        dst->X = -X;
        dst->Y = -Y;
        dst->Z = -Z;
        dst->W = W;

        return true;
    }

    // Too close to zero.
    if (n < 0.000001f)
        return false;

    n = 1.0f / n;
    dst->X = -X * n;
    dst->Y = -Y * n;
    dst->Z = -Z * n;
    dst->W = W * n;

    return true;
}

void Quaternion::Multiply(const Quaternion& q)
{
    Multiply(*this, q, this);
}

void Quaternion::Multiply(const Quaternion& q1, const Quaternion& q2, Quaternion* dst)
{
    assert(dst);

    float X = q1.W * q2.X + q1.X * q2.W + q1.Y * q2.Z - q1.Z * q2.Y;
    float Y = q1.W * q2.Y - q1.X * q2.Z + q1.Y * q2.W + q1.Z * q2.X;
    float Z = q1.W * q2.Z + q1.X * q2.Y - q1.Y * q2.X + q1.Z * q2.W;
    float W = q1.W * q2.W - q1.X * q2.X - q1.Y * q2.Y - q1.Z * q2.Z;

    dst->X = X;
    dst->Y = Y;
    dst->Z = Z;
    dst->W = W;
}

void Quaternion::Normalize()
{
    Normalize(this);
}

void Quaternion::Normalize(Quaternion* dst) const
{
    assert(dst);

    if (this != dst)
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
    if (n < 0.000001f)
        return;

    n = 1.0f / n;
    dst->X *= n;
    dst->Y *= n;
    dst->Z *= n;
    dst->W *= n;
}

void Quaternion::Set(float x, float y, float z, float w)
{
    this->X = x;
    this->Y = y;
    this->Z = z;
    this->W = w;
}

void Quaternion::Set(float* array)
{
    assert(array);

    X = array[0];
    Y = array[1];
    Z = array[2];
    W = array[3];
}

void Quaternion::Set(const Matrix& m)
{
    Quaternion::CreateFromRotationMatrix(m, this);
}

void Quaternion::Set(const Vector3& axis, float angle)
{
    Quaternion::CreateFromAxisAngle(axis, angle, this);
}

void Quaternion::Set(const Quaternion& q)
{
    this->X = q.X;
    this->Y = q.Y;
    this->Z = q.Z;
    this->W = q.W;
}

void Quaternion::SetIdentity()
{
    X = 0.0f;
    Y = 0.0f;
    Z = 0.0f;
    W = 1.0f;
}

float Quaternion::ToAxisAngle(Vector3* axis) const
{
    assert(axis);

    Quaternion q(X, Y, Z, W);
    q.Normalize();
    axis->X = q.X;
    axis->Y = q.Y;
    axis->Z = q.Z;
    axis->Normalize();

    return (2.0f * acos(q.W));
}

void Quaternion::Lerp(const Quaternion& q1, const Quaternion& q2, float t, Quaternion* dst)
{
    assert(dst);
    assert(!(t < 0.0f || t > 1.0f));

    if (t == 0.0f)
    {
        memcpy(dst, &q1, sizeof(float) * 4);
        return;
    }
    else if (t == 1.0f)
    {
        memcpy(dst, &q2, sizeof(float) * 4);
        return;
    }

    float t1 = 1.0f - t;

    dst->X = t1 * q1.X + t * q2.X;
    dst->Y = t1 * q1.Y + t * q2.Y;
    dst->Z = t1 * q1.Z + t * q2.Z;
    dst->W = t1 * q1.W + t * q2.W;
}

void Quaternion::Slerp(const Quaternion& q1, const Quaternion& q2, float t, Quaternion* dst)
{
    assert(dst);
    Slerp(q1.X, q1.Y, q1.Z, q1.W, q2.X, q2.Y, q2.Z, q2.W, t, &dst->X, &dst->Y, &dst->Z, &dst->W);
}

void Quaternion::Squad(const Quaternion& q1, const Quaternion& q2, const Quaternion& s1, const Quaternion& s2, float t, Quaternion* dst)
{
    assert(!(t < 0.0f || t > 1.0f));

    Quaternion dstQ(0.0f, 0.0f, 0.0f, 1.0f);
    Quaternion dstS(0.0f, 0.0f, 0.0f, 1.0f);

    SlerpForSquad(q1, q2, t, &dstQ);
    SlerpForSquad(s1, s2, t, &dstS);
    SlerpForSquad(dstQ, dstS, 2.0f * t * (1.0f - t), dst);
}

void Quaternion::Slerp(float q1x, float q1y, float q1z, float q1w, float q2x, float q2y, float q2z, float q2w, float t, float* dstx, float* dsty, float* dstz, float* dstw)
{
    // Fast slerp implementation by kwhatmough:
    // It contains no division operations, no trig, no inverse trig
    // and no sqrt. Not only does this code tolerate small constraint
    // errors in the input quaternions, it actually corrects for them.
    assert(dstx && dsty && dstz && dstw);
    assert(!(t < 0.0f || t > 1.0f));

    if (t == 0.0f)
    {
        *dstx = q1x;
        *dsty = q1y;
        *dstz = q1z;
        *dstw = q1w;
        return;
    }
    else if (t == 1.0f)
    {
        *dstx = q2x;
        *dsty = q2y;
        *dstz = q2z;
        *dstw = q2w;
        return;
    }

    if (q1x == q2x && q1y == q2y && q1z == q2z && q1w == q2w)
    {
        *dstx = q1x;
        *dsty = q1y;
        *dstz = q1z;
        *dstw = q1w;
        return;
    }

    float halfY, alpha, beta;
    float u, f1, f2a, f2b;
    float ratio1, ratio2;
    float halfSecHalfTheta, versHalfTheta;
    float sqNotU, sqU;

    float cosTheta = q1w * q2w + q1x * q2x + q1y * q2y + q1z * q2z;

    // As usual in all slerp implementations, we fold theta.
    alpha = cosTheta >= 0 ? 1.0f : -1.0f;
    halfY = 1.0f + alpha * cosTheta;

    // Here we bisect the interval, so we need to fold t as well.
    f2b  = t - 0.5f;
    u    = f2b >= 0 ? f2b : -f2b;
    f2a  = u - f2b;
    f2b += u;
    u   += u;
    f1   = 1.0f - u;

    // One iteration of Newton to get 1-cos(theta / 2) to good accuracy.
    halfSecHalfTheta  = 1.09f - (0.476537f - 0.0903321f * halfY) * halfY;
    halfSecHalfTheta *= 1.5f - halfY * halfSecHalfTheta * halfSecHalfTheta;
    versHalfTheta     = 1.0f - halfY * halfSecHalfTheta;

    // Evaluate series expansions of the coefficients.
    sqNotU = f1 * f1;
    ratio2 = 0.0000440917108f * versHalfTheta;
    ratio1 = -0.00158730159f + (sqNotU - 16.0f) * ratio2;
    ratio1 = 0.0333333333f + ratio1 * (sqNotU - 9.0f) * versHalfTheta;
    ratio1 = -0.333333333f + ratio1 * (sqNotU - 4.0f) * versHalfTheta;
    ratio1 = 1.0f + ratio1 * (sqNotU - 1.0f) * versHalfTheta;

    sqU    = u * u;
    ratio2 = -0.00158730159f + (sqU - 16.0f) * ratio2;
    ratio2 = 0.0333333333f + ratio2 * (sqU - 9.0f) * versHalfTheta;
    ratio2 = -0.333333333f + ratio2 * (sqU - 4.0f) * versHalfTheta;
    ratio2 = 1.0f + ratio2 * (sqU - 1.0f) * versHalfTheta;

    // Perform the bisection and resolve the folding done earlier.
    f1    *= ratio1 * halfSecHalfTheta;
    f2a   *= ratio2;
    f2b   *= ratio2;
    alpha *= f1 + f2a;
    beta   = f1 + f2b;

    // Apply final coefficients to a and b as usual.
    float W = alpha * q1w + beta * q2w;
    float X = alpha * q1x + beta * q2x;
    float Y = alpha * q1y + beta * q2y;
    float Z = alpha * q1z + beta * q2z;

    // This final adjustment to the quaternion's length corrects for
    // any small constraint error in the inputs q1 and q2 But as you
    // can see, it comes at the cost of 9 additional multiplication
    // operations. If this error-correcting feature is not required,
    // the following code may be removed.
    f1    = 1.5f - 0.5f * (W * W + X * X + Y * Y + Z * Z);
    *dstw = W * f1;
    *dstx = X * f1;
    *dsty = Y * f1;
    *dstz = Z * f1;
}

void Quaternion::SlerpForSquad(const Quaternion& q1, const Quaternion& q2, float t, Quaternion* dst)
{
    assert(dst);

    // cos(omega) = q1 * q2;
    // slerp(q1, q2, t) = (q1*sin((1-t)*omega) + q2*sin(t*omega))/sin(omega);
    // q1 = +- q2, slerp(q1,q2,t) = q1.
    // This is a straight-forward implementation of the formula of slerp. It does not do any sign switching.
    float c = q1.X * q2.X + q1.Y * q2.Y + q1.Z * q2.Z + q1.W * q2.W;

    if (fabs(c) >= 1.0f)
    {
        dst->X = q1.X;
        dst->Y = q1.Y;
        dst->Z = q1.Z;
        dst->W = q1.W;
        return;
    }

    float omega = acos(c);
    float s = sqrt(1.0f - c * c);
    if (fabs(s) <= 0.00001f)
    {
        dst->X = q1.X;
        dst->Y = q1.Y;
        dst->Z = q1.Z;
        dst->W = q1.W;
        return;
    }

    float r1 = sin((1 - t) * omega) / s;
    float r2 = sin(t * omega) / s;
    dst->X   = (q1.X * r1 + q2.X * r2);
    dst->Y   = (q1.Y * r1 + q2.Y * r2);
    dst->Z   = (q1.Z * r1 + q2.Z * r2);
    dst->W   = (q1.W * r1 + q2.W * r2);
}

NS_FK_END
