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
#ifndef FOUNDATIONKIT_MATHUTIL_HPP
#define FOUNDATIONKIT_MATHUTIL_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include "FoundationKit/GenericPlatformMacros.hpp"

NS_FK_BEGIN

#define MATRIX_SIZE ( sizeof(float) * 16)

/**
 * Defines a math utility class.
 *
 * This is primarily used for optimized internal math operations.
 */
class MathUtil
{
    friend class Matrix;
    friend class Vector3;

public:

    /**
     * Updates the given scalar towards the given target using a smoothing function.
     * The given response time determines the amount of smoothing (lag). A longer
     * response time yields a smoother result and more lag. To force the scalar to
     * follow the target closely, provide a response time that is very small relative
     * to the given elapsed time.
     *
     * @param x the scalar to update.
     * @param target target value.
     * @param elapsedTime elapsed time between calls.
     * @param responseTime response time (in the same units as elapsedTime).
     */
    static void Smooth(float* x, float target, float elapsedTime, float responseTime);

    /**
     * Updates the given scalar towards the given target using a smoothing function.
     * The given rise and fall times determine the amount of smoothing (lag). Longer
     * rise and fall times yield a smoother result and more lag. To force the scalar to
     * follow the target closely, provide rise and fall times that are very small relative
     * to the given elapsed time.
     *
     * @param x the scalar to update.
     * @param target target value.
     * @param elapsedTime elapsed time between calls.
     * @param riseTime response time for rising slope (in the same units as elapsedTime).
     * @param fallTime response time for falling slope (in the same units as elapsedTime).
     */
    static void Smooth(float* x, float target, float elapsedTime, float riseTime, float fallTime);

private:

    inline static void AddMatrix(const float* m, float scalar, float* dst);

    inline static void AddMatrix(const float* m1, const float* m2, float* dst);

    inline static void SubtractMatrix(const float* m1, const float* m2, float* dst);

    inline static void MultiplyMatrix(const float* m, float scalar, float* dst);

    inline static void MultiplyMatrix(const float* m1, const float* m2, float* dst);

    inline static void NegateMatrix(const float* m, float* dst);

    inline static void TransposeMatrix(const float* m, float* dst);

    inline static void TransformVector4(const float* m, float x, float y, float z, float w, float* dst);

    inline static void TransformVector4(const float* m, const float* v, float* dst);

    inline static void CrossVector3(const float* v1, const float* v2, float* dst);

    MathUtil();
};

NS_FK_END

#if (TARGET_PLATFORM == PLATFORM_IOS)
#if defined (__arm64__)
#define USE_NEON64
#define INCLUDE_NEON64
#elif defined (__ARM_NEON__)
#define USE_NEON32
#define INCLUDE_NEON32
#endif
#elif (TARGET_PLATFORM == PLATFORM_ANDROID)
#if defined (__arm64__) || defined (__aarch64__)
#define USE_NEON64
#define INCLUDE_NEON64
#elif defined (__ARM_NEON__)
#define INCLUDE_NEON32
#endif
#endif

#if defined(INCLUDE_NEON32) && defined(USE_NEON32)
#include "MathUtilNeon.inl"
#else
#include "MathUtil.inl"
#endif


#endif // FOUNDATIONKIT_MATHUTIL_HPP


