/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.

  losemymind.libo@gmail.com

****************************************************************************/
#include "FoundationKit/GenericPlatformMacros.hpp"
#if (PLATFORM_ANDROID)

#include "FoundationKit/Platform/Path.hpp"
#include "FoundationKit/Foundation/StringUtils.hpp"
NS_FK_BEGIN

std::string Path::GetTempPath()
{
    return TemporaryPath;
}

std::string Path::GetDocumentsPath()
{
    return DocumentsPath;
}
void Path::SetDocumentsPath(const std::string& path)
{
    DocumentsPath = path;
}
void Path::SetTempPath(const std::string& path)
{
    TemporaryPath = path;
}
std::string Path::GetApplicationPath()
{
    return "assets/";
}
NS_FK_END

#endif // #if (PLATFORM_ANDROID)
