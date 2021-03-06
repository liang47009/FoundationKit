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

FoundationKit::Version::Version(const Version& other)
    : Major(other.Major)
    , Minor(other.Minor)
    , Build(other.Build)
    , Revision(other.Revision)
    , MajorRevision(other.MajorRevision)
    , MinorRevision(other.MinorRevision)
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
    TryParse(version, *this);
}

Version::Version(int major, int minor)
    : Major(0)
    , Minor(0)
    , Build(-1)
    , Revision(-1)
    , MajorRevision(-1)
    , MinorRevision(-1)
{
    Init(major, minor, -1, -1);
}

Version::Version(int major, int minor, int build)
    : Major(0)
    , Minor(0)
    , Build(-1)
    , Revision(-1)
    , MajorRevision(-1)
    , MinorRevision(-1)
{
    Init(major, minor, build, -1);
}

Version::Version(int major, int minor, int build, int revision)
    : Major(0)
    , Minor(0)
    , Build(-1)
    , Revision(-1)
    , MajorRevision(-1)
    , MinorRevision(-1)
{
    Init(major, minor, build, revision);
}



Version& Version::operator=(const Version& other)
{
    this->Major         = other.Major;
    this->Minor         = other.Minor;
    this->Build         = other.Build;
    this->Revision      = other.Revision;
    this->MajorRevision = other.MajorRevision;
    this->MinorRevision = other.MinorRevision;
    return (*this);
}


bool Version::Init(int major, int minor, int build, int revision)
{
    if (major < 0)
        throw std::invalid_argument("ArgumentOutOfRange_Version");
    if (minor < 0)
        throw std::invalid_argument("ArgumentOutOfRange_Version");
    //if (build < 0)
    //    throw std::invalid_argument("ArgumentOutOfRange_Version");
    //if (revision < 0)
    //    throw std::invalid_argument("ArgumentOutOfRange_Version");
    Major    = major;
    Minor    = minor;
    Build    = build;
    Revision = revision;
    MajorRevision = (Revision >> 16);
    MinorRevision = (Revision & 0xffff);
    return true;
}

FoundationKit::Version Version::Clone()
{
    return Version(this->Major, this->Minor, this->Build, this->Revision);
}

int Version::CompareTo(const Version& other)
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

bool Version::Equals(const Version& other)
{
    if (this->Major    != other.Major ||
        this->Minor    != other.Minor ||
        this->Build    != other.Build ||
        this->Revision != other.Revision)
            return false;
    return true;
}

int Version::GetHashCode()
{
    int accumulator = 0;
    accumulator |= (this->Major    & 0x0000000F) << 28;
    accumulator |= (this->Minor    & 0x000000FF) << 20;
    accumulator |= (this->Build    & 0x000000FF) << 12;
    accumulator |= (this->Revision & 0x00000FFF) << 28;
    return accumulator;
}

std::string Version::ToString()
{
    if (this->Build == -1)return ToString(2);
    if (this->Revision == -1) return ToString(3);
    return ToString(4);
}

std::string Version::ToString(int fieldCount)
{
    std::string result;
    switch (fieldCount)
    {
    case 0:
        break;
    case 1:
        result.append(StringUtils::Tostring(this->Major));
        break;
    case 2:
        result.append(StringUtils::Tostring(this->Major));
        result.append(".");
        result.append(StringUtils::Tostring(this->Minor));
        break;
    case 3:
        if (this->Build == -1)
            throw std::invalid_argument("ArgumentOutOfRange_Bounds_Lower_upper");
        else
            result += StringUtils::Tostring(this->Major)
            + "."
            + StringUtils::Tostring(this->Minor)
            + "."
            + StringUtils::Tostring(this->Build);
        break;
    case 4:
        if (this->Revision == -1)
            throw std::invalid_argument("ArgumentOutOfRange_Bounds_Lower_upper");
        else
            result += StringUtils::Tostring(this->Major)
            + "."
            + StringUtils::Tostring(this->Minor)
            + "."
            + StringUtils::Tostring(this->Build)
            + "."
            + StringUtils::Tostring(this->Revision);
        break;
    default:
        break;
    }
    return result;
}

FoundationKit::Version Version::Parse(const std::string& input)
{
    Version version;
    Version::TryParse(input, version);
    return version;
}

bool Version::TryParse(const std::string& input, Version& result)
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

        result.Major = ToNumber<int>(elems[0]);
        result.Minor = ToNumber<int>(elems[1]);
        if (result.Major == -1 || result.Minor == -1)
            break;

        parsedLength -= 2;

        if (parsedLength > 0)
        {
            result.Build = ToNumber<int>(elems[2]);
            if (result.Build == -1)
                break;
            parsedLength--;
            if (parsedLength > 0)
            {
                result.Revision = ToNumber<int>(elems[3]);
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