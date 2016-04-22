
#include "FoundationKit/GenericPlatformMacros.h"

#if (TARGET_PLATFORM == PLATFORM_ANDROID || TARGET_PLATFORM == PLATFORM_LINUX)

#include "FoundationKit/Platform/FileUtils.h"
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <sys/stat.h>

NS_FK_BEGIN
void FileUtils::initRootPath()
{
    if (0 == _resourceRootPath.length())
    {
        _resourceRootPath = "assets/";
    }
}

bool FileUtils::isFileExist(const std::string& filename) const
{
    if (filename.empty())
        return false;

    std::string fullPath = filename;
    if (!isAbsolutePath(fullPath))
    {
        fullPath.insert(0, _resourceRootPath);
    }
    struct stat sts;
    return (stat(fullPath.c_str(), &sts) != -1) ? true : false;
}


std::string FileUtils::getWritablePath() const
{
    struct stat st;
    stat(_writablePath.c_str(), &st);
    if (!S_ISDIR(st.st_mode)) {
        mkdir(_writablePath.c_str(), 0744);
    }

    return _writablePath;
}


NS_FK_END


#endif

