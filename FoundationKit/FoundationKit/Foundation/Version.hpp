/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.
 
  losemymind.libo@gmail.com

****************************************************************************/
#ifndef FOUNDATIONKIT_VERSION_HPP
#define FOUNDATIONKIT_VERSION_HPP

#pragma once
#include <string>
#include <sstream>
#include "FoundationKit/GenericPlatformMacros.hpp"
#include "FoundationKit/Base/lexical_cast.hpp"
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

    Version(const std::string& version);

    Version(int major, int minor);

    Version(int major, int minor, int build);

    Version(int major, int minor, int build, int revision);

    Version Clone();

    int CompareTo(Version other);

    bool Equals(Version other);

    int GetHashCode();
   
    std::string ToString();

    std::string ToString(int fieldCount);

    static Version Parse(const std::string& input);

    static bool TryParse(const std::string& input, Version& result);

    inline bool operator !=(Version& other)
    {
        return !(*this == other);
    }

    inline bool operator <(Version& other)
    {
        return this->CompareTo(other) < 0;
    }

    inline bool operator <=(Version& other)
    {
        return this->CompareTo(other) <= 0;
    }

    inline bool operator ==(Version& other)
    {
        return this->Equals(other);
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
    bool Init(int major, int minor, int build, int revision);

    /**
     * @brief Converts strings to basic numeric types.
     * @usage tonumber<type>( string );
     */
    template< class _Type, class _Val >
    static _Type ToNumber(_Val& val)
    {
        return lexical_cast<_Type>(val);
    }
};

NS_FK_END

#endif // FOUNDATIONKIT_VERSION_HPP
