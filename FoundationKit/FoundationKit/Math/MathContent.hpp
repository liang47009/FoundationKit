/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.
 
  losemymind.libo@gmail.com

****************************************************************************/
#ifndef FOUNDATIONKIT_MATHCONTENT_HPP
#define FOUNDATIONKIT_MATHCONTENT_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <limits>
#include <cmath>
#include "FoundationKit/GenericPlatformMacros.hpp"
#include "FoundationKit/Base/types.hpp"

#define MATH_PIOVER2                1.57079632679489661923f
#define MATH_PIOVER4                0.785398163397448309616f
#define MATH_EPSILON                0.000001f

NS_FK_BEGIN

namespace Math
{
    /// A few useful constants
    const int32   MaxInt    = (std::numeric_limits<int32>::max)();
    const uint32  MaxUInt   = (std::numeric_limits<uint32>::max)();
    const int64   MaxInt64  = (std::numeric_limits<int64>::max)();
    const uint64  MaxUInt64 = (std::numeric_limits<uint64>::max)();
    const double  MaxDouble = (std::numeric_limits<double>::max)();
    const double  MinDouble = (std::numeric_limits<double>::min)();
    const float   MaxFloat  = (std::numeric_limits<float>::max)();
    const float   MinFloat  = (std::numeric_limits<float>::min)();

    const float   SmailFloat = 1.0e-37f;
    const float   Tolerance  = 2e-37f;

    /// Represents the log base ten of e(0.4342945).
    const float Log10E = 0.4342945f;

    /// Represents the log base two of e(1.442695).
    const float Log2E = 1.442695f;

    /// A tiny floating point value.
    const float Epsilon = 1.4013e-045f;

    /// Represents the mathematical constant e(2.71828175).
    const float ExponentialE = 2.71828f;

    /// The golden ratio. Oooooh!
    const float GoldenRatio = 1.61803f;

    /// Grad-to-degrees conversion constant.
    const float Grad2Deg = 0.9f;

    /// Degrees-to-grad conversion constant.
    const float Deg2Grad = 1.1111111f;

    /// Radians-to-degrees conversion constant.
    const float Rad2Deg = 57.29577951f;

    /// Degrees-to-radians conversion constant.
    const float Deg2Rad = 0.0174532925f;

    /// Radians-to-grad conversion constant.
    const float Rad2Grad = 63.6619772f;

    /// Grad-to-radians conversion constant.
    const float Grad2Rad = 0.015708f;

    /// A representation of positive infinity.
    const float Infinity = MaxFloat;// 1.0f / 0.0f;

    /// A representation of negative infinity.
    const float NegativeInfinity = MinFloat;// -1.0f / 0.0f;

    /// The infamous 3.14159265358979323846... value.
    const float    PI        = 3.141592f;
    const double   TwoPi     = PI * 2;
    const double   HalfPi    = PI / 2;
    const double   QuarterPi = PI / 4;
}

NS_FK_END

#endif // FOUNDATIONKIT_MATHCONTENT_HPP
