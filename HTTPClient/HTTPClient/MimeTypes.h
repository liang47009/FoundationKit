/****************************************************************************
Copyright (c) 2015 libo.

losemymind.libo@gmail.com

****************************************************************************/
#ifndef LOSEMYMIND_MIMETYPES_H
#define LOSEMYMIND_MIMETYPES_H

#pragma once
#include <string>

#include "FoundationKit/GenericPlatformMacros.h"

NS_FK_BEGIN
namespace network{
namespace MimeTypes
{
    std::string getMimeType(std::string fileName);
};
} // namespace network
NS_FK_END
#endif // LOSEMYMIND_MIMETYPES_H

