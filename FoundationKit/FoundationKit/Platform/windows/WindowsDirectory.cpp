#include "FoundationKit/GenericPlatformMacros.hpp"
#if (TARGET_PLATFORM == PLATFORM_WINDOWS)
#include <Windows.h>
#include "FoundationKit/Platform/Directory.hpp"
#include "FoundationKit/Foundation/StringUtils.hpp"

NS_FK_BEGIN
bool Directory::CreateDirectory(const std::string& path)
{
    if (Exists(path))
        return true;
    std::wstring wpath = StringUtils::string2UTF8wstring(path);
    // Split the path
    size_t start = 0;
    size_t found = wpath.find_first_of(L"/\\", start);
    std::wstring subpath;
    std::vector<std::wstring> dirs;
    if (found != std::wstring::npos)
    {
        while (true)
        {
            subpath = wpath.substr(start, found - start + 1);
            if (!subpath.empty())
                dirs.push_back(subpath);
            start = found + 1;
            found = wpath.find_first_of(L"/\\", start);
            if (found == std::wstring::npos)
            {
                if (start < wpath.length())
                {
                    dirs.push_back(wpath.substr(start));
                }
                break;
            }
        }
    }

    if ((GetFileAttributes(wpath.c_str())) == INVALID_FILE_ATTRIBUTES)
    {
        subpath = L"";
        for (unsigned int i = 0; i < dirs.size(); ++i)
        {
            subpath += dirs[i];

            std::string utf8Path = StringUtils::wstring2UTF8string(subpath);
            if (!Exists(utf8Path))
            {
                BOOL ret = ::CreateDirectory(subpath.c_str(), NULL);
                if (!ret && ERROR_ALREADY_EXISTS != GetLastError())
                {
                    FKLog("Fail create directory %s !Error code is 0x%x", utf8Path.c_str(), GetLastError());
                    return false;
                }
            }
        }
    }
    return true;
}

bool Directory::RemoveDirectory(const std::string& path)
{
    std::wstring wpath = StringUtils::string2UTF8wstring(path);
    std::wstring files = wpath + L"*.*";
    WIN32_FIND_DATA wfd;
    HANDLE  search = FindFirstFileExW(files.c_str(), FindExInfoStandard, &wfd, FindExSearchNameMatch, NULL, 0);
    bool ret = true;
    if (search != INVALID_HANDLE_VALUE)
    {
        BOOL find = true;
        while (find)
        {
            //. ..
            if (wfd.cFileName[0] != '.')
            {
                std::wstring temp = wpath + wfd.cFileName;
                if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
                {
                    temp += '/';
                    ret = ret && RemoveDirectory(StringUtils::wstring2UTF8string(temp));
                }
                else
                {
                    SetFileAttributesW(temp.c_str(), FILE_ATTRIBUTE_NORMAL);
                    ret = ret && DeleteFileW(temp.c_str());
                }
            }
            find = FindNextFileW(search, &wfd);
        }
        FindClose(search);
    }
    if (ret && ::RemoveDirectoryW(wpath.c_str()))
    {
        return true;
    }
    return false;
}

bool Directory::Move(const std::string& sourceDirName, const std::string& destDirName)
{
    if (Exists(destDirName))
        return false;

    if (!Exists(sourceDirName))
        return false;

    if (::MoveFileA(sourceDirName.c_str(), destDirName.c_str()) == FALSE)
    {
        FKLog("Fail to Move file %s to %s !Error code is 0x%x", sourceDirName.c_str(), destDirName.c_str(), GetLastError());
        return false;
    }
    return true;
}

bool Directory::Exists(const std::string& path)
{
    std::wstring utf16Str = StringUtils::string2UTF8wstring(path);
    unsigned long fAttrib = GetFileAttributesW(reinterpret_cast<LPCWSTR>(utf16Str.c_str()));
    if (fAttrib != INVALID_FILE_ATTRIBUTES && (fAttrib & FILE_ATTRIBUTE_DIRECTORY))
    {
        return true;
    }
    return false;
}

NS_FK_END

#endif // OF #if (TARGET_PLATFORM == PLATFORM_WINDOWS)

