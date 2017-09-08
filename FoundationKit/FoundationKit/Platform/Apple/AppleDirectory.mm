
#include "FoundationKit/GenericPlatformMacros.hpp"
#if (TARGET_PLATFORM == PLATFORM_IOS || TARGET_PLATFORM == PLATFORM_MAC)

#include <dirent.h> // for DIR
#import <Foundation/Foundation.h>
#include <ftw.h>
#include "FoundationKit/Platform/Directory.hpp"
#include "FoundationKit/Foundation/StringUtils.hpp"

NS_FK_BEGIN
bool Directory::CreateDirectory(const std::string& path)
{
    if (Exists(path))
        return true;
    
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
    
}

static int unlink_cb(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf)
{
    auto ret = remove(fpath);
    if (ret)
    {
        FKLog("Fail to remove: %s ", fpath);
    }
    return ret;
}

bool Directory::RemoveDirectory(const std::string& path)
{
    if (path.size() > 0 && path[path.size() - 1] != '/')
    {
        FKLog("Fail to remove directory, path must termniate with '/': %s", path.c_str());
        return false;
    }
    
    if (nftw(path.c_str(), unlink_cb, 64, FTW_DEPTH | FTW_PHYS))
        return false;
    else
        return true;
}

bool Directory::Move(const std::string& sourceDirName, const std::string& destDirName)
{
    if (Exists(destDirName))
        return false;

    if (!Exists(sourceDirName))
        return false;

    return (rename(sourceDirName.c_str(), destDirName.c_str()) == 0);
}

bool Directory::Exists(const std::string& path)
{
    struct stat sts;
    if (stat(path.c_str(), &sts) == 0 && S_ISDIR(sts.st_mode))
        return true;
    return false;
}

NS_FK_END

#endif // OF #if (TARGET_PLATFORM == PLATFORM_IOS || TARGET_PLATFORM == PLATFORM_MAC)




