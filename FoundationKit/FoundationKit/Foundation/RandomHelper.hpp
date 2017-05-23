/****************************************************************************
 Copyright (c) 2015 libo All rights reserved.
 
 losemymind.libo@gmail.com
 
 ****************************************************************************/
#ifndef LOSEMYMIND_RANDOMHELPER_H
#define LOSEMYMIND_RANDOMHELPER_H

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <random>
#include <cmath>
#include "FoundationKit/GenericPlatformMacros.hpp"

NS_FK_BEGIN

class RandomHelper
{
public:

	/** 
	 * Random number distribution that produces floating-point values
	 *        according to a uniform distribution, which is described 
	 *        by the following probability density function:
	 *        p(x|a,b)=1/b-a, a ≤ x < b
	 * The random loating_point number in a range [a, b)
	 * @param  a and b are set on construction.
	 * @return A floating-point type, by default type is double.
	 */
	template< class _Ty >
	static inline _Ty Random(_Ty a, _Ty b,
		typename std::enable_if< std::is_floating_point<_Ty>::value >::type* = 0)
	{
		std::uniform_real_distribution<_Ty> randomResult(a, b);
		return randomResult(GetRandomEngine());
	}

	/** 
	 * Get random integral number in a range [a, b]
	 * @return An integer type, by default the type is int.
	 */
	template< class _Ty >
	static inline _Ty Random(_Ty a, _Ty b,
		typename std::enable_if< std::is_integral<_Ty>::value >::type* = 0)
	{
		std::uniform_int_distribution<> randomResult(a, b);
		return randomResult(GetRandomEngine());
	}

	/**
	 * @return A random floating-point number in the range [0, 1.0).
	 *         if want to get a floating-point number in the range[0.0, 1.0],
	 *         usage:
	 *         double ret = random<double>();
	 *         double ret = random<double>(0.0, std::nextafter(1, DBL_MAX));
	 */
	template< class _Ty >
	static inline _Ty Random(typename std::enable_if< std::is_floating_point<_Ty>::value >::type* = 0)
	{
		return RandomHelper::Random<_Ty>((_Ty)0, std::nextafter(1, DBL_MAX));
	}

	// return a random boolean.
	static inline bool   RandBoolean()
	{
		return Random<double>() > 0.5;
	}


	//returns a random double in the range -1 < n < 1
	static inline double RandClamped()
	{
		return Random<double>() - Random<double>();
	}


public:
	static std::default_random_engine GetRandomEngine()
	{
		static std::random_device          _randomDevice;
		static std::default_random_engine  _randomEngine(_randomDevice());
		return _randomEngine;
	}

	RandomHelper()  = delete;
	~RandomHelper() = default;
};

NS_FK_END
#endif // LOSEMYMIND_RANDOMHELPER_H






