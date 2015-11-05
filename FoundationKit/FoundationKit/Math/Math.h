/****************************************************************************
  Copyright (c) 2014-2015 libo.
 
  losemymind.libo@gmail.com

****************************************************************************/
#pragma  once

#include <vector>
#include <limits>
#include <cmath>
#include "FoundationKit/FoundationKitDefines.h"

NS_FK_BEGIN


#define MATH_PIOVER2                1.57079632679489661923f
#define MATH_PIOVER4                0.785398163397448309616f
#define MATH_EPSILON                0.000001f

/// Provides constants and static methods for trigonometric, 
/// logarithmic and other common mathematical functions.
namespace Math
{

    /// A few useful constants
    const int     MaxInt    = (std::numeric_limits<int>::max)();
    const double  MaxDouble = (std::numeric_limits<double>::max)();
    const double  MinDouble = (std::numeric_limits<double>::min)();
    const float   MaxFloat  = (std::numeric_limits<float>::max)();
    const float   MinFloat  = (std::numeric_limits<float>::min)();

    const float   SmailFloat = 1.0e-37f;
    const float   Tolerance   = 2e-37f;

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

    /// The infamous 3.14159265358979... value.
    const float    PI        = 3.14159f;
    const double   TwoPi     = PI * 2;
    const double   HalfPi    = PI / 2;
    const double   QuarterPi = PI / 4;



    /// Returns the absolute value of val.
    template< typename T >
	inline typename std::enable_if< std::is_arithmetic<T>::value, T >::type 
    abs(const T& val)
    {return std::abs(val);}

    /// Returns the arc-cosine of val - the angle in radians whose cosine is val.
    template< typename T >
	inline typename std::enable_if< std::is_arithmetic<T>::value, T >::type 
    acos(const T& val)
    {return std::acos(val);}

    /** 
     * Compares two values if they are similar.
     * Due to floating point imprecision it is not 
     * recommended to compare floats using the equal
     * operator. eg. 1.0 == 10.0 / 10.0 might not return true. 
     */
    template< typename T1, typename T2 >
    bool approximately(const T1 a,const T2 b,
        typename std::enable_if< std::is_arithmetic<T1>::value >::type* = 0,
        typename std::enable_if< std::is_arithmetic<T2>::value >::type* = 0) 
    { return roughlyEqual(a, b, 0.1f);  }

    /// Returns the arc-sine of val - the angle in radians whose sine is val.
    template< typename T >
	inline typename std::enable_if< std::is_arithmetic<T>::value, T >::type 
    asin(const T val)
    { return std::asin(val); }

    /// Returns the arc-tangent of val - the angle in radians whose tangent is val.
    template< typename T >
	inline typename std::enable_if< std::is_arithmetic<T>::value, T >::type 
    atan(const T val)
    { return std::atan(val); }

    /// Returns the angle in radians whose Tan is y/x.
    template< typename T >
	inline typename std::enable_if< std::is_arithmetic<T>::value, T >::type 
    atan2(const T y, const T x)
    { return std::atan2(y, x); }

    /// Returns the smallest integer greater to or equal to val.
    template< typename T >
	inline typename std::enable_if< std::is_arithmetic<T>::value, T >::type 
    ceil(const T val)
    { return std::ceil(val); }

    /// Returns the smallest integer greater to or equal to val.
    template< typename T >
    int ceilToInt(const T val,
        typename std::enable_if< std::is_arithmetic<T>::value >::type* = 0)
    {return static_cast<int>(std::ceil(val)); }

	/// Returns the largest integer smaller to or equal to val.
	template< typename T >
	inline typename std::enable_if< std::is_arithmetic<T>::value, T >::type 
    floor(const T& val)
	{
		return std::floor(val);
	}

	/// Returns the largest integer smaller to or equal to val.
	template< typename T >
	int floorToInt(const T& val,
		typename std::enable_if< std::is_arithmetic<T>::value >::type* = 0)
	{
		return static_cast<int>(std::floor(val));
	}

    /// Clamps val value between a minimum and maximum value.
    template< typename T>
	inline typename std::enable_if< std::is_arithmetic<T>::value, T >::type 
    clamp(const T& val, const T& minVal, const T& maxVal)
    {
        if (maxVal <= minVal)
            return minVal;
        return val < minVal ? minVal : val > maxVal ? maxVal : val;
    }

    /// Clamps value between 0 and 1 and returns value.
    template< typename T>
	inline typename std::enable_if< std::is_arithmetic<T>::value, T >::type 
    clamp01(const T& val)
    { return val < 0 ? 0 : val > 1 ? 1 : val; }

    /// Returns the cosine of angle f in radians.
    template< typename T >
	inline typename std::enable_if< std::is_arithmetic<T>::value, T >::type 
    cos(const T& val)
    { return std::cos(val); }

    /// Returns e raised to the specified power.
    template< typename T >
	inline typename std::enable_if< std::is_arithmetic<T>::value, T >::type 
    exp(const T& power)
    { return std::exp(power); }


    /// Interpolates between from and to by t. t is clamped between 0 and 1.
    template< typename T, typename U, typename V>
    inline float Lerp(const T& from, const U& to, const V& t,
        typename std::enable_if< std::is_arithmetic<T>::value >::type* = 0,
        typename std::enable_if< std::is_arithmetic<U>::value >::type* = 0,
        typename std::enable_if< std::is_arithmetic<V>::value >::type* = 0) 
    { 
        return t >= 1 ? to : t < 0 ? from : from + (to - from) * t; 
    }

    /// Returns the natural (base e) logarithm of val specified value.
    template< typename T >
	inline typename std::enable_if< std::is_arithmetic<T>::value, T >::type 
    log(const T& val)
    { return std::log(val); }

    /// Returns the base 10 logarithm of val specified value.
    template< typename T >
	inline typename std::enable_if< std::is_arithmetic<T>::value, T >::type 
    log10(const T& val0)
    { return std::log10(val0); }

    /// returns the maximum of two values
    template< typename T >
	inline typename std::enable_if< std::is_arithmetic<T>::value, T >::type 
    max(const T& v1, const T& v2)
    { return v1 > v2 ? v1 : v2; }

    /// returns the minimum of two values
    template <typename T>
	inline typename std::enable_if< std::is_arithmetic<T>::value, T >::type 
    min(const T& v1, const T& v2)
    { return v1 < v2 ? v1 : v2; }

    /// Returns f raised to power p.
    template< typename T , typename U >
    T Pow(const T& f, const U& p,
        typename std::enable_if< std::is_arithmetic<T>::value >::type* = 0,
        typename std::enable_if< std::is_arithmetic<U>::value >::type* = 0)
    { return std::pow(f, p); }

    /**
        * Compares two floating point values if they are similar.
        * @param[in] v1 First value.
        * @param[in] v2 Second value.
        * @param[in] threshold The threshold of similarity
        * @return True if the values are similar, otherwise false.
        */
    template< typename T, typename U, typename V>
    bool roughlyEqual(const T v1, const U v2, const V threshold = 0.01f) 
    { return abs(v1 - v2) <= threshold;  }

    /// Returns f rounded to the nearest integer.
//    template <typename T>
//    T Round(const T& val,
//        typename std::enable_if< std::is_arithmetic<T>::value >::type* = 0) 
//    { return std::round(val); }

    /// Returns the sign of val.
//    template <typename T>
//    T sign(const T& val,
//        typename std::enable_if< std::is_arithmetic<T>::value >::type* = 0)
//    { return std::sign(val); }

    /// Returns the sine of angle val in radians.
    template <typename T>
	inline typename std::enable_if< std::is_arithmetic<T>::value, T >::type 
    sin(const T& val)
    { return std::sin(val); }

    /// Returns square root of val.
    template <typename T>
	inline typename std::enable_if< std::is_arithmetic<T>::value, T >::type 
    sqrt(const T& val)
    { return std::sqrt(val); }

    /// Returns the tangent of angle val in radians.
    template <typename T>
	inline typename std::enable_if< std::is_arithmetic<T>::value, T >::type 
    tan(const T& val)
    { return std::tan(val); }

    template<typename T>
    double average(const std::vector<T>& vec,
        typename std::enable_if< std::is_arithmetic<T>::value >::type* = 0)
    {
        double dAverage = 0.0;
        for (auto& i : vec )
        {
            dAverage += static_cast<double>(i);
        }
        return dAverage / static_cast<double>(vec.size());
    }

    inline bool equal(const float& v1, const float& v2)
    {
        return (std::fabs(v1-v2) < 1E-12);
    }

    inline bool equal(const double& v1, const double& v2)
    {
        return (std::fabs(v1-v2) < 1E-12);
    }

    template< typename T >
    inline bool fuzzyEqual(const T& v1, const T& v2, const T& var)
    {
        return(v1 - var <= v2 && v2 <= v1 + var);
    }

    /// Return true if the parameter is equal to zero
    inline bool isZero(const double& val)
    {
        return ((-MinDouble < val) && (val < MinDouble));
    }

    /// Returns if the value is powered by two.
    inline bool isPowerOfTwo(const int& val) 
    {
        return (val > 0) && ((val & (val - 1)) == 0);
    }

    inline double sigmoid(const double& input, const double& response = 1.0)
    {
        return ( 1.0 / ( 1.0 + exp(-input / response)));
    }

    // return true is the third parameter is in the range described by the first two
    template< typename T, typename U, typename V>
    inline bool inRange(const T& start, const U& end, const V& val)
    {
        if (start < end)
        {
            return ((val > start) && (val < end));
        }
        else
        {
            return ((val < start) && (val > end));
        }
    }


    /** Converts radians to degrees.
     *  This method uses double precission internally,
     *  though it returns single float
     *  Factor = 180 / pi
     *  @param[in] radians The angle in radians.
     *  @return    The angle in degrees.
     */
    inline float radiansToDegrees(float radians)
    { 
        return static_cast<float>(radians * 57.295779513082320876798154814105);
    }

    /** Converts degrees to radians.
     *  This method uses double precission internally,
     *  though it returns single float
     *  Factor = pi / 180
     *  @param[in]  degrees  The angle in degrees.
     *  @return     The angle in radians.
     */
    inline float degreesToRadians(float degrees)
    { 
        return static_cast<float>(degrees * 0.017453292519943295769236907684886);
    }

    /// Get the next power of two after val value.
    int nextPowerOfTwo(int val);

    /// Returns the closest power of two to val value.
    int closestPowerOfTwo(int val);

    double standardDeviation(const std::vector<double>& vec);

} // namespace Math
NS_FK_END

















