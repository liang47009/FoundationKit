#include "FoundationKit/Foundation/Directory.hpp"
#include "FoundationKit/Foundation/StringUtils.hpp"
#include "FoundationKit/Base/scope_locale.hpp"
#include <dirent.h> // for DIR
#if TARGET_PLATFORM == PLATFORM_WINDOWS
#include <Windows.h>
#else
#include <sys/types.h>
#include <errno.h>
#include <dirent.h>
#endif

NS_FK_BEGIN


bool Directory::CreateDirectory(const std::string& path)
{
    if (IsExist(path))
        return true;
#if TARGET_PLATFORM == PLATFORM_WINDOWS
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
            if (!IsExist(utf8Path))
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
#else
    // Split the path
    size_t start = 0;
    size_t found = path.find_first_of("/\\", start);
    std::string subpath;
    std::vector<std::string> dirs;

    if (found != std::string::npos)
    {
        while (true)
        {
            subpath = path.substr(start, found - start + 1);
            if (!subpath.empty())
                dirs.push_back(subpath);
            start = found + 1;
            found = path.find_first_of("/\\", start);
            if (found == std::string::npos)
            {
                if (start < path.length())
                {
                    dirs.push_back(path.substr(start));
                }
                break;
            }
        }
    }

    // Create path recursively
    subpath = "";
    for (int i = 0; i < dirs.size(); ++i)
    {
        subpath += dirs[i];
        DIR *dir = opendir(subpath.c_str());
        if (!dir)
        {
            // directory doesn't exist, should create a new one
            int ret = mkdir(subpath.c_str(), S_IRWXU | S_IRWXG | S_IRWXO);
            if (ret != 0 && (errno != EEXIST))
            {
                // current directory can not be created, sub directories can not be created too
                // should return
                return false;
            }
        }
        else
        {
            // directory exists, should close opened dir
            closedir(dir);
        }
    }
    return true;
#endif

}

bool Directory::RemoveDirectory(const std::string& path)
{
#if TARGET_PLATFORM == PLATFORM_WINDOWS
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
#else
#endif
}

void Directory::Move(const std::string& sourceDirName, const std::string& destDirName)
{

}

bool Directory::IsExist(const std::string& path)
{

}


bool Directory::IsDirectory(const std::string& path)
{

}

void Directory::GetFiles(const std::string& path, std::vector<std::string>& files, SearchOption searchOption /*= SearchOption::TopDirectoryOnly*/)
{

}

void Directory::GetFiles(const std::string& path, bool includeChild, const EnumFileCallback& callback)
{

}

void Directory::GetDirectories(const std::string& path, std::vector<std::string>& dirs, SearchOption searchOption /*= SearchOption::TopDirectoryOnly*/)
{

}


NS_FK_END



