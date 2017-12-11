/****************************************************************************
 Copyright (c) 2015 libo All rights reserved.
 
 losemymind.libo@gmail.com
 
 ****************************************************************************/
#include "FoundationKit/Foundation/Math.hpp"
NS_FK_BEGIN

namespace Math
{


int NextPowerOfTwo( int val )
{
    val = val - 1;
    if (val < 0)
        return 0;
    val |= val >> 1;
    val |= val >> 2;
    val |= val >> 4;
    val |= val >> 8;
    val |= val >> 16;
    return val + 1;
}

int ClosestPowerOfTwo( int val )
{
    int b = NextPowerOfTwo(val);
    int c = b / 2;
    return val - c < b - val ? c : b;
}

double StandardDeviation( const std::vector<double>& vec )
{
    double sd = 0.0;
    double dAverage = Average<double>(vec);
    for (unsigned int i=0; i < vec.size(); ++i)
    {
        sd +=(vec[i]- dAverage)*(vec[i] - dAverage);
    }
    sd = sd / vec.size();
    return sqrt(sd);
}


std::default_random_engine& GetRandomEngine()
{
    static std::random_device          _randomDevice;
    static std::default_random_engine  _randomEngine(_randomDevice());
    return _randomEngine;
}

} // namespace MathEx
NS_FK_END