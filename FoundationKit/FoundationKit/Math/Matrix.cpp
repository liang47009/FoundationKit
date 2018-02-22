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
#include "FoundationKit/Math/Matrix.hpp"

NS_FK_BEGIN

static const float MATRIX_IDENTITY[16] =
{
    1.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f
};

Matrix::Matrix()
{
    *this = Matrix::Identity();
}

Matrix::Matrix(float m11, float m12, float m13, float m14, float m21, float m22, float m23, float m24,
               float m31, float m32, float m33, float m34, float m41, float m42, float m43, float m44)
{
    Set(m11, m12, m13, m14, m21, m22, m23, m24, m31, m32, m33, m34, m41, m42, m43, m44);
}

Matrix::Matrix(const float* mat)
{
    Set(mat);
}

Matrix::Matrix(const Matrix& copy)
{
    memcpy(m, copy.m, MATRIX_SIZE);
}

Matrix::~Matrix()
{
}

const Matrix& Matrix::Identity()
{
    static Matrix m(
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1 );
    return m;
}

const Matrix& Matrix::Zero()
{
    static Matrix m(
        0, 0, 0, 0,
        0, 0, 0, 0,
        0, 0, 0, 0,
        0, 0, 0, 0 );
    return m;
}

void Matrix::CreateLookAt(const Vector3& eyePosition, const Vector3& targetPosition, const Vector3& up, Matrix* dst)
{
    CreateLookAt(eyePosition.X, eyePosition.Y, eyePosition.Z, targetPosition.X, targetPosition.Y, targetPosition.Z,
                 up.X, up.Y, up.Z, dst);
}

void Matrix::CreateLookAt(float eyePositionX, float eyePositionY, float eyePositionZ,
                          float targetPositionX, float targetPositionY, float targetPositionZ,
                          float upX, float upY, float upZ, Matrix* dst)
{
    assert(dst);

    Vector3 eye(eyePositionX, eyePositionY, eyePositionZ);
    Vector3 target(targetPositionX, targetPositionY, targetPositionZ);
    Vector3 up(upX, upY, upZ);
    up.Normalize();

    Vector3 zaxis;
    Vector3::Subtract(eye, target, &zaxis);
    zaxis.Normalize();

    Vector3 xaxis;
    Vector3::Cross(up, zaxis, &xaxis);
    xaxis.Normalize();

    Vector3 yaxis;
    Vector3::Cross(zaxis, xaxis, &yaxis);
    yaxis.Normalize();

    dst->m[0]  = xaxis.X;
    dst->m[1]  = yaxis.X;
    dst->m[2]  = zaxis.X;
    dst->m[3]  = 0.0f;

    dst->m[4]  = xaxis.Y;
    dst->m[5]  = yaxis.Y;
    dst->m[6]  = zaxis.Y;
    dst->m[7]  = 0.0f;

    dst->m[8]  = xaxis.Z;
    dst->m[9]  = yaxis.Z;
    dst->m[10] = zaxis.Z;
    dst->m[11] = 0.0f;

    dst->m[12] = -Vector3::Dot(xaxis, eye);
    dst->m[13] = -Vector3::Dot(yaxis, eye);
    dst->m[14] = -Vector3::Dot(zaxis, eye);
    dst->m[15] = 1.0f;
}

void Matrix::CreatePerspective(float fieldOfView, float aspectRatio,
                                     float zNearPlane, float zFarPlane, Matrix* dst)
{
    assert(dst);
    assert(zFarPlane != zNearPlane);

    float f_n = 1.0f / (zFarPlane - zNearPlane);
    float theta = fieldOfView *Math::Deg2Rad * 0.5f;
    if (fabs(fmod(theta, MATH_PIOVER2)) < MATH_EPSILON)
    {
        //GP_ERROR("Invalid field of view value (%d) causes attempted calculation tan(%d), which is undefined.", fieldOfView, theta);
        return;
    }
    float divisor = tan(theta);
    assert(divisor);
    float factor = 1.0f / divisor;

    memset(dst, 0, MATRIX_SIZE);

    assert(aspectRatio);
    dst->m[0]  = (1.0f / aspectRatio) * factor;
    dst->m[5]  = factor;
    dst->m[10] = (-(zFarPlane + zNearPlane)) * f_n;
    dst->m[11] = -1.0f;
    dst->m[14] = -2.0f * zFarPlane * zNearPlane * f_n;
}

void Matrix::CreateOrthographic(float width, float height, float zNearPlane, float zFarPlane, Matrix* dst)
{
    float halfWidth = width / 2.0f;
    float halfHeight = height / 2.0f;
    CreateOrthographicOffCenter(-halfWidth, halfWidth, -halfHeight, halfHeight, zNearPlane, zFarPlane, dst);
}

void Matrix::CreateOrthographicOffCenter(float left, float right, float bottom, float top,
                                         float zNearPlane, float zFarPlane, Matrix* dst)
{
    assert(dst);
    assert(right     != left);
    assert(top       != bottom);
    assert(zFarPlane != zNearPlane);

    memset(dst, 0, MATRIX_SIZE);
    dst->m[0]  = 2 / (right - left);
    dst->m[5]  = 2 / (top - bottom);
    dst->m[12] = (left + right) / (left - right);
    dst->m[10] = 1 / (zNearPlane - zFarPlane);
    dst->m[13] = (top + bottom) / (bottom - top);
    dst->m[14] = zNearPlane / (zNearPlane - zFarPlane);
    dst->m[15] = 1;
}
    
void Matrix::CreateBillboard(const Vector3& objectPosition, const Vector3& cameraPosition,
                             const Vector3& cameraUpVector, Matrix* dst)
{
    CreateBillboardHelper(objectPosition, cameraPosition, cameraUpVector, NULL, dst);
}

void Matrix::CreateBillboard(const Vector3& objectPosition, const Vector3& cameraPosition,
                             const Vector3& cameraUpVector, const Vector3& cameraForwardVector,
                             Matrix* dst)
{
    CreateBillboardHelper(objectPosition, cameraPosition, cameraUpVector, &cameraForwardVector, dst);
}

void Matrix::CreateBillboardHelper(const Vector3& objectPosition, const Vector3& cameraPosition,
                                   const Vector3& cameraUpVector, const Vector3* cameraForwardVector,
                                   Matrix* dst)
{
    Vector3 delta(objectPosition, cameraPosition);
    bool isSufficientDelta = delta.LengthSquared() > MATH_EPSILON;

    dst->SetIdentity();
    dst->m[3]  = objectPosition.X;
    dst->m[7]  = objectPosition.Y;
    dst->m[11] = objectPosition.Z;

    // As per the contracts for the 2 variants of createBillboard, we need
    // either a safe default or a sufficient distance between object and camera.
    if (cameraForwardVector || isSufficientDelta)
    {
        Vector3 target = isSufficientDelta ? cameraPosition : (objectPosition - *cameraForwardVector);

        // A billboard is the inverse of a lookAt rotation
        Matrix lookAt;
        CreateLookAt(objectPosition, target, cameraUpVector, &lookAt);
        dst->m[0]  = lookAt.m[0];
        dst->m[1]  = lookAt.m[4];
        dst->m[2]  = lookAt.m[8];
        dst->m[4]  = lookAt.m[1];
        dst->m[5]  = lookAt.m[5];
        dst->m[6]  = lookAt.m[9];
        dst->m[8]  = lookAt.m[2];
        dst->m[9]  = lookAt.m[6];
        dst->m[10] = lookAt.m[10];
    }
}

/**
void Matrix::createReflection(const Plane& plane, Matrix* dst)
{
    Vector3 normal(plane.getNormal());
    float k = -2.0f * plane.getDistance();

    dst->SetIdentity();

    dst->m[0]  -= 2.0f * normal.X * normal.X;
    dst->m[5]  -= 2.0f * normal.Y * normal.Y;
    dst->m[10] -= 2.0f * normal.Z * normal.Z;
    dst->m[1]   = dst->m[4] = -2.0f * normal.X * normal.Y;
    dst->m[2]   = dst->m[8] = -2.0f * normal.X * normal.Z;
    dst->m[6]   = dst->m[9] = -2.0f * normal.Y * normal.Z;
    
    dst->m[3]  = k * normal.X;
    dst->m[7]  = k * normal.Y;
    dst->m[11] = k * normal.Z;
}
*/
void Matrix::CreateScale(const Vector3& scale, Matrix* dst)
{
    assert(dst);
    memcpy(dst, MATRIX_IDENTITY, MATRIX_SIZE);
    dst->m[0]  = scale.X;
    dst->m[5]  = scale.Y;
    dst->m[10] = scale.Z;
}

void Matrix::CreateScale(float xScale, float yScale, float zScale, Matrix* dst)
{
    assert(dst);
    memcpy(dst, MATRIX_IDENTITY, MATRIX_SIZE);
    dst->m[0]  = xScale;
    dst->m[5]  = yScale;
    dst->m[10] = zScale;
}


void Matrix::CreateRotation(const Quaternion& q, Matrix* dst)
{
    assert(dst);

    float x2   = q.X + q.X;
    float y2   = q.Y + q.Y;
    float z2   = q.Z + q.Z;

    float xx2  = q.X * x2;
    float yy2  = q.Y * y2;
    float zz2  = q.Z * z2;
    float xy2  = q.X * y2;
    float xz2  = q.X * z2;
    float yz2  = q.Y * z2;
    float wx2  = q.W * x2;
    float wy2  = q.W * y2;
    float wz2  = q.W * z2;

    dst->m[0]  = 1.0f - yy2 - zz2;
    dst->m[1]  = xy2 + wz2;
    dst->m[2]  = xz2 - wy2;
    dst->m[3]  = 0.0f;

    dst->m[4]  = xy2 - wz2;
    dst->m[5]  = 1.0f - xx2 - zz2;
    dst->m[6]  = yz2 + wx2;
    dst->m[7]  = 0.0f;

    dst->m[8]  = xz2 + wy2;
    dst->m[9]  = yz2 - wx2;
    dst->m[10] = 1.0f - xx2 - yy2;
    dst->m[11] = 0.0f;

    dst->m[12] = 0.0f;
    dst->m[13] = 0.0f;
    dst->m[14] = 0.0f;
    dst->m[15] = 1.0f;
}

void Matrix::CreateRotation(const Vector3& axis, float angle, Matrix* dst)
{
    assert(dst);

    float X = axis.X;
    float Y = axis.Y;
    float Z = axis.Z;

    // Make sure the input axis is normalized.
    float n = X*X + Y*Y + Z*Z;
    if (n != 1.0f)
    {
        // Not normalized.
        n = sqrt(n);
        // Prevent divide too close to zero.
        if (n > 0.000001f)
        {
            n  = 1.0f / n;
            X *= n;
            Y *= n;
            Z *= n;
        }
    }

    float c   = cos(angle);
    float s   = sin(angle);
    float t   = 1.0f - c;
    float tx  = t * X;
    float ty  = t * Y;
    float tz  = t * Z;
    float txy = tx * Y;
    float txz = tx * Z;
    float tyz = ty * Z;
    float sx  = s * X;
    float sy  = s * Y;
    float sz  = s * Z;

    dst->m[0] = c + tx*X;
    dst->m[1] = txy + sz;
    dst->m[2] = txz - sy;
    dst->m[3] = 0.0f;

    dst->m[4] = txy - sz;
    dst->m[5] = c + ty*Y;
    dst->m[6] = tyz + sx;
    dst->m[7] = 0.0f;

    dst->m[8]  = txz + sy;
    dst->m[9]  = tyz - sx;
    dst->m[10] = c + tz*Z;
    dst->m[11] = 0.0f;

    dst->m[12] = 0.0f;
    dst->m[13] = 0.0f;
    dst->m[14] = 0.0f;
    dst->m[15] = 1.0f;
}

void Matrix::CreateRotationX(float angle, Matrix* dst)
{
    assert(dst);

    memcpy(dst, MATRIX_IDENTITY, MATRIX_SIZE);

    float c = cos(angle);
    float s = sin(angle);

    dst->m[5]  = c;
    dst->m[6]  = s;
    dst->m[9]  = -s;
    dst->m[10] = c;
}

void Matrix::CreateRotationY(float angle, Matrix* dst)
{
    assert(dst);

    memcpy(dst, MATRIX_IDENTITY, MATRIX_SIZE);

    float c = cos(angle);
    float s = sin(angle);

    dst->m[0]  = c;
    dst->m[2]  = -s;
    dst->m[8]  = s;
    dst->m[10] = c;
}

void Matrix::CreateRotationZ(float angle, Matrix* dst)
{
    assert(dst);

    memcpy(dst, MATRIX_IDENTITY, MATRIX_SIZE);

    float c = cos(angle);
    float s = sin(angle);

    dst->m[0] = c;
    dst->m[1] = s;
    dst->m[4] = -s;
    dst->m[5] = c;
}

void Matrix::CreateTranslation(const Vector3& translation, Matrix* dst)
{
    assert(dst);

    memcpy(dst, MATRIX_IDENTITY, MATRIX_SIZE);

    dst->m[12] = translation.X;
    dst->m[13] = translation.Y;
    dst->m[14] = translation.Z;
}

void Matrix::CreateTranslation(float xTranslation, float yTranslation, float zTranslation, Matrix* dst)
{
    assert(dst);

    memcpy(dst, MATRIX_IDENTITY, MATRIX_SIZE);

    dst->m[12] = xTranslation;
    dst->m[13] = yTranslation;
    dst->m[14] = zTranslation;
}

void Matrix::Add(float scalar)
{
    Add(scalar, this);
}

void Matrix::Add(float scalar, Matrix* dst)
{
    assert(dst);

    MathUtil::AddMatrix(m, scalar, dst->m);
}

void Matrix::Add(const Matrix& other)
{
    Add(*this, other, this);
}

void Matrix::Add(const Matrix& m1, const Matrix& m2, Matrix* dst)
{
    assert(dst);

    MathUtil::AddMatrix(m1.m, m2.m, dst->m);
}

bool Matrix::Decompose(Vector3* scale, Quaternion* rotation, Vector3* translation) const
{
    if (translation)
    {
        // Extract the translation.
        translation->X = m[12];
        translation->Y = m[13];
        translation->Z = m[14];
    }

    // Nothing left to do.
    if (scale == NULL && rotation == NULL)
        return true;

    // Extract the scale.
    // This is simply the length of each axis (row/column) in the matrix.
    Vector3 xaxis(m[0], m[1], m[2]);
    float scaleX = xaxis.Length();

    Vector3 yaxis(m[4], m[5], m[6]);
    float scaleY = yaxis.Length();

    Vector3 zaxis(m[8], m[9], m[10]);
    float scaleZ = zaxis.Length();

    // Determine if we have a negative scale (true if Determinant is less than zero).
    // In this case, we simply Negate a single axis of the scale.
    float det = Determinant();
    if (det < 0)
        scaleZ = -scaleZ;

    if (scale)
    {
        scale->X = scaleX;
        scale->Y = scaleY;
        scale->Z = scaleZ;
    }

    // Nothing left to do.
    if (rotation == NULL)
        return true;

    // Scale too close to zero, can't Decompose rotation.
    if (scaleX < Math::Tolerance || scaleY < Math::Tolerance || fabs(scaleZ) < Math::Tolerance)
        return false;

    float rn;

    // Factor the scale out of the matrix axes.
    rn       = 1.0f / scaleX;
    xaxis.X *= rn;
    xaxis.Y *= rn;
    xaxis.Z *= rn;

    rn       = 1.0f / scaleY;
    yaxis.X *= rn;
    yaxis.Y *= rn;
    yaxis.Z *= rn;

    rn       = 1.0f / scaleZ;
    zaxis.X *= rn;
    zaxis.Y *= rn;
    zaxis.Z *= rn;

    // Now calculate the rotation from the resulting matrix (axes).
    float trace = xaxis.X + yaxis.Y + zaxis.Z + 1.0f;

    if (trace > MATH_EPSILON)
    {
        float s     = 0.5f / sqrt(trace);
        rotation->W = 0.25f / s;
        rotation->X = (yaxis.Z - zaxis.Y) * s;
        rotation->Y = (zaxis.X - xaxis.Z) * s;
        rotation->Z = (xaxis.Y - yaxis.X) * s;
    }
    else
    {
        // Note: since xaxis, yaxis, and zaxis are normalized, 
        // we will never divide by zero in the code below.
        if (xaxis.X > yaxis.Y && xaxis.X > zaxis.Z)
        {
            float s     = 0.5f / sqrt(1.0f + xaxis.X - yaxis.Y - zaxis.Z);
            rotation->W = (yaxis.Z - zaxis.Y) * s;
            rotation->X = 0.25f / s;
            rotation->Y = (yaxis.X + xaxis.Y) * s;
            rotation->Z = (zaxis.X + xaxis.Z) * s;
        }
        else if (yaxis.Y > zaxis.Z)
        {
            float s     = 0.5f / sqrt(1.0f + yaxis.Y - xaxis.X - zaxis.Z);
            rotation->W = (zaxis.X - xaxis.Z) * s;
            rotation->X = (yaxis.X + xaxis.Y) * s;
            rotation->Y = 0.25f / s;
            rotation->Z = (zaxis.Y + yaxis.Z) * s;
        }
        else
        {
            float s     = 0.5f / sqrt(1.0f + zaxis.Z - xaxis.X - yaxis.Y );
            rotation->W = (xaxis.Y - yaxis.X ) * s;
            rotation->X = (zaxis.X + xaxis.Z ) * s;
            rotation->Y = (zaxis.Y + yaxis.Z ) * s;
            rotation->Z = 0.25f / s;
        }
    }

    return true;
}

float Matrix::Determinant() const
{
    float a0 = m[0] * m[5] - m[1] * m[4];
    float a1 = m[0] * m[6] - m[2] * m[4];
    float a2 = m[0] * m[7] - m[3] * m[4];
    float a3 = m[1] * m[6] - m[2] * m[5];
    float a4 = m[1] * m[7] - m[3] * m[5];
    float a5 = m[2] * m[7] - m[3] * m[6];
    float b0 = m[8] * m[13] - m[9] * m[12];
    float b1 = m[8] * m[14] - m[10] * m[12];
    float b2 = m[8] * m[15] - m[11] * m[12];
    float b3 = m[9] * m[14] - m[10] * m[13];
    float b4 = m[9] * m[15] - m[11] * m[13];
    float b5 = m[10] * m[15] - m[11] * m[14];

    // Calculate the Determinant.
    return (a0 * b5 - a1 * b4 + a2 * b3 + a3 * b2 - a4 * b1 + a5 * b0);
}

void Matrix::GetScale(Vector3* scale) const
{
    Decompose(scale, NULL, NULL);
}

bool Matrix::GetRotation(Quaternion* rotation) const
{
    return Decompose(NULL, rotation, NULL);
}

void Matrix::GetTranslation(Vector3* translation) const
{
    Decompose(NULL, NULL, translation);
}

void Matrix::GetUpVector(Vector3* dst) const
{
    assert(dst);

    dst->X = m[4];
    dst->Y = m[5];
    dst->Z = m[6];
}

void Matrix::GetDownVector(Vector3* dst) const
{
    assert(dst);
    
    dst->X = -m[4];
    dst->Y = -m[5];
    dst->Z = -m[6];
}

void Matrix::GetLeftVector(Vector3* dst) const
{
    assert(dst);

    dst->X = -m[0];
    dst->Y = -m[1];
    dst->Z = -m[2];
}

void Matrix::GetRightVector(Vector3* dst) const
{
    assert(dst);

    dst->X = m[0];
    dst->Y = m[1];
    dst->Z = m[2];
}

void Matrix::GetForwardVector(Vector3* dst) const
{
    assert(dst);

    dst->X = -m[8];
    dst->Y = -m[9];
    dst->Z = -m[10];
}

void Matrix::GetBackVector(Vector3* dst) const
{
    assert(dst);

    dst->X = m[8];
    dst->Y = m[9];
    dst->Z = m[10];
}

bool Matrix::Invert()
{
    return Invert(this);
}

bool Matrix::Invert(Matrix* dst) const
{
    float a0 = m[0] * m[5] - m[1] * m[4];
    float a1 = m[0] * m[6] - m[2] * m[4];
    float a2 = m[0] * m[7] - m[3] * m[4];
    float a3 = m[1] * m[6] - m[2] * m[5];
    float a4 = m[1] * m[7] - m[3] * m[5];
    float a5 = m[2] * m[7] - m[3] * m[6];
    float b0 = m[8] * m[13] - m[9] * m[12];
    float b1 = m[8] * m[14] - m[10] * m[12];
    float b2 = m[8] * m[15] - m[11] * m[12];
    float b3 = m[9] * m[14] - m[10] * m[13];
    float b4 = m[9] * m[15] - m[11] * m[13];
    float b5 = m[10] * m[15] - m[11] * m[14];

    // Calculate the Determinant.
    float det = a0 * b5 - a1 * b4 + a2 * b3 + a3 * b2 - a4 * b1 + a5 * b0;

    // Close to zero, can't invert.
    if (fabs(det) <= Math::Tolerance)
        return false;

    // Support the case where m == dst.
    Matrix inverse;
    inverse.m[0]  = m[5] * b5 - m[6] * b4 + m[7] * b3;
    inverse.m[1]  = -m[1] * b5 + m[2] * b4 - m[3] * b3;
    inverse.m[2]  = m[13] * a5 - m[14] * a4 + m[15] * a3;
    inverse.m[3]  = -m[9] * a5 + m[10] * a4 - m[11] * a3;

    inverse.m[4]  = -m[4] * b5 + m[6] * b2 - m[7] * b1;
    inverse.m[5]  = m[0] * b5 - m[2] * b2 + m[3] * b1;
    inverse.m[6]  = -m[12] * a5 + m[14] * a2 - m[15] * a1;
    inverse.m[7]  = m[8] * a5 - m[10] * a2 + m[11] * a1;

    inverse.m[8]  = m[4] * b4 - m[5] * b2 + m[7] * b0;
    inverse.m[9]  = -m[0] * b4 + m[1] * b2 - m[3] * b0;
    inverse.m[10] = m[12] * a4 - m[13] * a2 + m[15] * a0;
    inverse.m[11] = -m[8] * a4 + m[9] * a2 - m[11] * a0;

    inverse.m[12] = -m[4] * b3 + m[5] * b1 - m[6] * b0;
    inverse.m[13] = m[0] * b3 - m[1] * b1 + m[2] * b0;
    inverse.m[14] = -m[12] * a3 + m[13] * a1 - m[14] * a0;
    inverse.m[15] = m[8] * a3 - m[9] * a1 + m[10] * a0;

    Multiply(inverse, 1.0f / det, dst);

    return true;
}

bool Matrix::IsIdentity() const
{
    return (memcmp(m, MATRIX_IDENTITY, MATRIX_SIZE) == 0);
}

void Matrix::Multiply(float scalar)
{
    Multiply(scalar, this);
}

void Matrix::Multiply(float scalar, Matrix* dst) const
{
    Multiply(*this, scalar, dst);
}

void Matrix::Multiply(const Matrix& mat, float scalar, Matrix* dst)
{
    assert(dst);

    MathUtil::MultiplyMatrix(mat.m, scalar, dst->m);
}

void Matrix::Multiply(const Matrix& mat)
{
    Multiply(*this, mat, this);
}

void Matrix::Multiply(const Matrix& m1, const Matrix& m2, Matrix* dst)
{
    assert(dst);

    MathUtil::MultiplyMatrix(m1.m, m2.m, dst->m);
}

void Matrix::Negate()
{
    Negate(this);
}

void Matrix::Negate(Matrix* dst) const
{
    assert(dst);

    MathUtil::NegateMatrix(m, dst->m);
}

void Matrix::Rotate(const Quaternion& q)
{
    Rotate(q, this);
}

void Matrix::Rotate(const Quaternion& q, Matrix* dst) const
{
    Matrix r;
    CreateRotation(q, &r);
    Multiply(*this, r, dst);
}

void Matrix::Rotate(const Vector3& axis, float angle)
{
    Rotate(axis, angle, this);
}

void Matrix::Rotate(const Vector3& axis, float angle, Matrix* dst) const
{
    Matrix r;
    CreateRotation(axis, angle, &r);
    Multiply(*this, r, dst);
}

void Matrix::RotateX(float angle)
{
    RotateX(angle, this);
}

void Matrix::RotateX(float angle, Matrix* dst) const
{
    Matrix r;
    CreateRotationX(angle, &r);
    Multiply(*this, r, dst);
}

void Matrix::RotateY(float angle)
{
    RotateY(angle, this);
}

void Matrix::RotateY(float angle, Matrix* dst) const
{
    Matrix r;
    CreateRotationY(angle, &r);
    Multiply(*this, r, dst);
}

void Matrix::RotateZ(float angle)
{
    RotateZ(angle, this);
}

void Matrix::RotateZ(float angle, Matrix* dst) const
{
    Matrix r;
    CreateRotationZ(angle, &r);
    Multiply(*this, r, dst);
}

void Matrix::Scale(float value)
{
    Scale(value, this);
}

void Matrix::Scale(float value, Matrix* dst) const
{
    Scale(value, value, value, dst);
}

void Matrix::Scale(float xScale, float yScale, float zScale)
{
    Scale(xScale, yScale, zScale, this);
}

void Matrix::Scale(float xScale, float yScale, float zScale, Matrix* dst) const
{
    Matrix s;
    CreateScale(xScale, yScale, zScale, &s);
    Multiply(*this, s, dst);
}

void Matrix::Scale(const Vector3& s)
{
    Scale(s.X, s.Y, s.Z, this);
}

void Matrix::Scale(const Vector3& s, Matrix* dst) const
{
    Scale(s.X, s.Y, s.Z, dst);
}

void Matrix::Set(float m11, float m12, float m13, float m14, float m21, float m22, float m23, float m24,
                 float m31, float m32, float m33, float m34, float m41, float m42, float m43, float m44)
{
    m[0]  = m11;
    m[1]  = m21;
    m[2]  = m31;
    m[3]  = m41;
    m[4]  = m12;
    m[5]  = m22;
    m[6]  = m32;
    m[7]  = m42;
    m[8]  = m13;
    m[9]  = m23;
    m[10] = m33;
    m[11] = m43;
    m[12] = m14;
    m[13] = m24;
    m[14] = m34;
    m[15] = m44;
}

void Matrix::Set(const float* mat)
{
    assert(mat);
    memcpy(this->m, mat, MATRIX_SIZE);
}

void Matrix::Set(const Matrix& mat)
{
    memcpy(this->m, mat.m, MATRIX_SIZE);
}

void Matrix::SetIdentity()
{
    memcpy(m, MATRIX_IDENTITY, MATRIX_SIZE);
}

void Matrix::SetZero()
{
    memset(m, 0, MATRIX_SIZE);
}

void Matrix::Subtract(const Matrix& mat)
{
    Subtract(*this, mat, this);
}

void Matrix::Subtract(const Matrix& m1, const Matrix& m2, Matrix* dst)
{
    assert(dst);

    MathUtil::SubtractMatrix(m1.m, m2.m, dst->m);
}

void Matrix::TransformPoint(Vector3* point) const
{
    assert(point);
    TransformVector(point->X, point->Y, point->Z, 1.0f, point);
}

void Matrix::TransformPoint(const Vector3& point, Vector3* dst) const
{
    TransformVector(point.X, point.Y, point.Z, 1.0f, dst);
}

void Matrix::TransformVector(Vector3* vector) const
{
    assert(vector);
    TransformVector(vector->X, vector->Y, vector->Z, 0.0f, vector);
}

void Matrix::TransformVector(const Vector3& vector, Vector3* dst) const
{
    TransformVector(vector.X, vector.Y, vector.Z, 0.0f, dst);
}

void Matrix::TransformVector(float x, float y, float z, float w, Vector3* dst) const
{
    assert(dst);
    MathUtil::TransformVector4(m, x, y, z, w, (float*)dst);
}

void Matrix::TransformVector(Vector4* vector) const
{
    assert(vector);
    TransformVector(*vector, vector);
}

void Matrix::TransformVector(const Vector4& vector, Vector4* dst) const
{
    assert(dst);

    MathUtil::TransformVector4(m, (const float*) &vector, (float*)dst);
}

void Matrix::Translate(float x, float y, float z)
{
    Translate(x, y, z, this);
}

void Matrix::Translate(float x, float y, float z, Matrix* dst) const
{
    Matrix t;
    CreateTranslation(x, y, z, &t);
    Multiply(*this, t, dst);
}

void Matrix::Translate(const Vector3& t)
{
    Translate(t.X, t.Y, t.Z, this);
}

void Matrix::Translate(const Vector3& t, Matrix* dst) const
{
    Translate(t.X, t.Y, t.Z, dst);
}

void Matrix::Transpose()
{
    Transpose(this);
}

void Matrix::Transpose(Matrix* dst) const
{
    assert(dst);

    MathUtil::TransposeMatrix(m, dst->m);
}

NS_FK_END
