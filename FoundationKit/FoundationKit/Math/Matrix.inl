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

#ifndef FOUNDATIONKIT_MATRIX_INL
#define FOUNDATIONKIT_MATRIX_INL

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

NS_FK_BEGIN

inline const Matrix Matrix::operator+(const Matrix& mat) const
{
    Matrix result(*this);
    result.Add(mat);
    return result;
}

inline Matrix& Matrix::operator+=(const Matrix& mat)
{
    Add(mat);
    return *this;
}

inline const Matrix Matrix::operator-(const Matrix& mat) const
{
    Matrix result(*this);
    result.Subtract(mat);
    return result;
}

inline Matrix& Matrix::operator-=(const Matrix& mat)
{
    Subtract(mat);
    return *this;
}

inline const Matrix Matrix::operator-() const
{
    Matrix mat(*this);
	mat.Negate();
    return mat;
}

inline const Matrix Matrix::operator*(const Matrix& mat) const
{
    Matrix result(*this);
    result.Multiply(mat);
    return result;
}

inline Matrix& Matrix::operator*=(const Matrix& mat)
{
    Multiply(mat);
    return *this;
}

inline Vector3& operator*=(Vector3& v, const Matrix& mat)
{
	mat.TransformVector(&v);
    return v;
}

inline const Vector3 operator*(const Matrix& mat, const Vector3& v)
{
    Vector3 x;
	mat.TransformVector(v, &x);
    return x;
}

inline Vector4& operator*=(Vector4& v, const Matrix& mat)
{
	mat.TransformVector(&v);
    return v;
}

inline const Vector4 operator*(const Matrix& mat, const Vector4& v)
{
    Vector4 x;
	mat.TransformVector(v, &x);
    return x;
}

NS_FK_END
#endif // FOUNDATIONKIT_MATRIX_INL

