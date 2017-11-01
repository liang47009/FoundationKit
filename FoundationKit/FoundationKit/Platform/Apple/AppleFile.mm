#include "FoundationKit/GenericPlatformMacros.hpp"
#if (PLATFORM_IOS || PLATFORM_MAC)

#include <stdlib.h>
#include "FoundationKit/Platform/File.hpp"
#include "FoundationKit/Foundation/StringUtils.hpp"
NS_FK_BEGIN

bool File::Move(const std::string& sourceFileName, const std::string& destFileName)
{
    if (IsExists(destFileName))
        return false;

    if (!IsExists(sourceFileName))
        return false;

    return (rename(sourceFileName.c_str(), destFileName.c_str()) == 0);
}

bool File::Delete(const std::string& path)
{
    return (remove(path.c_str()) == 0);
}

bool File::IsExists(const std::string& path)
{
    struct stat sts;
    if (stat(path.c_str(), &sts) == 0 && S_ISREG(sts.st_mode))
        return true;
    return false;
}

bool File::Rename(const std::string& sourceFileName, const std::string& destFileName)
{
    return (rename(sourceFileName.c_str(), destFileName.c_str()) == 0);
}

NS_FK_END

#endif //OF #if (PLATFORM_IOS || PLATFORM_MAC)

