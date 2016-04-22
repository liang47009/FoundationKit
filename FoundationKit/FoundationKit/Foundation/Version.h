/****************************************************************************
 Copyright (c) 2015 libo All rights reserved.

 losemymind.libo@gmail.com

****************************************************************************/
#pragma once
#include <string>
#include <sstream>
#include "FoundationKit/GenericPlatformMacros.h"
NS_FK_BEGIN

class Version
{
public:
    int Major;
    int Minor;
    int Build;
    int Revision;
    short MajorRevision;
    short MinorRevision;
public:
    Version();

    Version(std::string version);

    Version(int major, int minor);

    Version(int major, int minor, int build);

    Version(int major, int minor, int build, int revision);

    Version clone();

    int compareTo(Version other);

    bool equals(Version other);

    int getHashCode();
   
    std::string toString();

    std::string toString(int fieldCount);

    static Version parse(std::string input);

    static bool tryParse(std::string input, Version& result);

    inline bool operator !=(Version& other)
    {
        return !(*this == other);
    }

    inline bool operator <(Version& other)
    {
        return this->compareTo(other) < 0;
    }

    inline bool operator <=(Version& other)
    {
        return this->compareTo(other) <= 0;
    }

    inline bool operator ==(Version& other)
    {
        return this->equals(other);
    }

    inline bool operator >(Version& other)
    {
        return other < *this;
    }

    inline bool operator >=(Version& other)
    {
        return other <= *this;
    }
private:
    bool init(int major, int minor, int build, int revision);

    /**
    * @brief Converts strings to basic numeric types.
    * @usage tonumber<type>( string );
    */
    template< class _Type, class _Val >
    static _Type tonumber(_Val& val)
    {
        std::istringstream iss(val);
        _Type result;
        return (iss >> result) ? result : -1;
    }
};

NS_FK_END