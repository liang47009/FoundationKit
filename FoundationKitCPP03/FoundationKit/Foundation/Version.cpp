/****************************************************************************
 Copyright (c) 2017 libo All rights reserved.

 losemymind.libo@gmail.com

****************************************************************************/
#include <vector>
#include <stdexcept>
#include "FoundationKit/Foundation/StringUtils.hpp"
#include "FoundationKit/Foundation/Version.hpp"
NS_FK_BEGIN

Version::Version()
    : Major(0)
    , Minor(0)
    , Build(-1)
    , Revision(-1)
    , MajorRevision(-1)
    , MinorRevision(-1)
{
    
}

Version::Version(const std::string& version)
    : Major(0)
    , Minor(0)
    , Build(-1)
    , Revision(-1)
    , MajorRevision(-1)
    , MinorRevision(-1)
{
    tryParse(version, *this);
}

Version::Version(int major, int minor)
    : Major(0)
    , Minor(0)
    , Build(-1)
    , Revision(-1)
    , MajorRevision(-1)
    , MinorRevision(-1)
{
    init(major, minor, 0, 0);
}

Version::Version(int major, int minor, int build)
    : Major(0)
    , Minor(0)
    , Build(-1)
    , Revision(-1)
    , MajorRevision(-1)
    , MinorRevision(-1)
{
    init(major, minor, build, 0);
}

Version::Version(int major, int minor, int build, int revision)
    : Major(0)
    , Minor(0)
    , Build(-1)
    , Revision(-1)
    , MajorRevision(-1)
    , MinorRevision(-1)
{
    init(major, minor, build, revision);
}

bool Version::init(int major, int minor, int build, int revision)
{
    if (major < 0)
        throw std::invalid_argument("ArgumentOutOfRange_Version");
    if (minor < 0)
        throw std::invalid_argument("ArgumentOutOfRange_Version");
    if (build < 0)
        throw std::invalid_argument("ArgumentOutOfRange_Version");
    if (revision < 0)
        throw std::invalid_argument("ArgumentOutOfRange_Version");
    Major    = major;
    Minor    = minor;
    Build    = build;
    Revision = revision;
    MajorRevision = (Revision >> 16);
    MinorRevision = (Revision & 0xffff);
    return true;
}

FoundationKit::Version Version::clone()
{
    return Version(this->Major, this->Minor, this->Build, this->Revision);
}

int Version::compareTo(Version other)
{
    if (this->Major != other.Major)
    {
        if (this->Major > other.Major)
            return 1;
        else
            return -1;
    }
    if (this->Minor != other.Minor)
    {
        if (this->Minor > other.Minor)
            return 1;
        else
            return -1;
    }
    if (this->Build != other.Build)
    {
        if (this->Build > other.Build)
            return 1;
        else
            return -1;
    }
    if (this->Revision != other.Revision)
    {
        if (this->Revision > other.Revision)
            return 1;
        else
            return -1;
    }
    return 0;
}

bool Version::equals(Version other)
{
    if (this->Major    != other.Major ||
        this->Minor    != other.Minor ||
        this->Build    != other.Build ||
        this->Revision != other.Revision)
            return false;
    return true;
}

int Version::getHashCode()
{
    int accumulator = 0;
    accumulator |= (this->Major    & 0x0000000F) << 28;
    accumulator |= (this->Minor    & 0x000000FF) << 20;
    accumulator |= (this->Build    & 0x000000FF) << 12;
    accumulator |= (this->Revision & 0x00000FFF) << 28;
    return accumulator;
}

std::string Version::toString()
{
    if (this->Build == -1)return toString(2);
    if (this->Revision == -1) return toString(3);
    return toString(4);
}

std::string Version::toString(int fieldCount)
{
    std::string result;
    switch (fieldCount)
    {
    case 0:
        break;
    case 1:
        result.append(StringUtils::to_string(this->Major));
        break;
    case 2:
        result.append(StringUtils::to_string(this->Major));
        result.append(".");
        result.append(StringUtils::to_string(this->Minor));
        break;
    case 3:
        if (this->Build == -1)
            throw std::invalid_argument("ArgumentOutOfRange_Bounds_Lower_upper");
        else
            result += StringUtils::to_string(this->Major)
            + "."
            + StringUtils::to_string(this->Minor)
            + "."
            + StringUtils::to_string(this->Build);
        break;
    case 4:
        if (this->Revision == -1)
            throw std::invalid_argument("ArgumentOutOfRange_Bounds_Lower_upper");
        else
            result += StringUtils::to_string(this->Major)
            + "."
            + StringUtils::to_string(this->Minor)
            + "."
            + StringUtils::to_string(this->Build)
            + "."
            + StringUtils::to_string(this->Revision);
        break;
    default:
        break;
    }
    return result;
}

FoundationKit::Version Version::parse(const std::string& input)
{
    Version version;
    Version::tryParse(input, version);
    return version;
}

bool Version::tryParse(const std::string& input, Version& result)
{
    bool successed = false;
    do 
    {
        std::vector<std::string> elems;
        std::stringstream ss(input);
        std::string item;
        while (std::getline(ss, item, '.'))
        {
            elems.push_back(item);
        }
        size_t parsedLength = elems.size();

        if (parsedLength < 2 || parsedLength > 4)
            break; //invalid_argument

        result.Major = tonumber<int>(elems[0]);
        result.Minor = tonumber<int>(elems[1]);
        if (result.Major == -1 || result.Minor == -1)
            break;

        parsedLength -= 2;

        if (parsedLength > 0)
        {
            result.Build = tonumber<int>(elems[2]);
            if (result.Build == -1)
                break;
            parsedLength--;
            if (parsedLength > 0)
            {
                result.Revision = tonumber<int>(elems[3]);
                if (result.Revision == -1)
                    break;
            }
        }
        result.MajorRevision = (result.Revision >> 16);
        result.MinorRevision = (result.Revision & 0xffff);
        successed = true;
    } while (false);

    return successed;
}

NS_FK_END