/****************************************************************************
Copyright (c) 2017 libo All rights reserved.

losemymind.libo@gmail.com

****************************************************************************/
#include "FoundationKit/GenericPlatformMacros.hpp"
#if (PLATFORM_WINDOWS)
#include "FoundationKit/Platform/File.hpp"
#include "FoundationKit/Foundation/StringUtils.hpp"
NS_FK_BEGIN

bool File::Move(const std::string& sourceFileName, const std::string& destFileName)
{
    if (MoveFileA(sourceFileName.c_str(), destFileName.c_str()) == 0)
    {
        FKLog("Fail MoveFile file from [%s] to [%s] !Error code is 0x%x", sourceFileName.c_str(), destFileName.c_str(), GetLastError());
        return false;
    }
    return true;
}

bool File::Delete(const std::string& path)
{
    if (DeleteFileA(path.c_str()) == 0)
    {
        FKLog("Fail Delete file %s !Error code is 0x%x", path.c_str(), GetLastError());
        return false;
    }
    return true;
}

bool File::IsExists(const std::string& path)
{
    std::wstring utf16Str = StringUtils::string2UTF8wstring(path);
    unsigned long fAttrib = GetFileAttributesW(reinterpret_cast<LPCWSTR>(utf16Str.c_str()));
    if (fAttrib != INVALID_FILE_ATTRIBUTES)
    {
        return true;
    }
    return false;
}

bool File::Rename(const std::string& sourceFileName, const std::string& destFileName)
{
    if (IsExists(destFileName))
    {
        if (File::Delete(destFileName) && File::Move(sourceFileName, destFileName))
        {
            return true;
        }
    }
    return false;
}

NS_FK_END

#endif // OF #if (PLATFORM_WINDOWS)

