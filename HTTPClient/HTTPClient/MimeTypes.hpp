/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.
 
  losemymind.libo@gmail.com

****************************************************************************/
#ifndef FOUNDATIONKIT_MIMETYPES_HPP
#define FOUNDATIONKIT_MIMETYPES_HPP

#pragma once
#include "FoundationKit/GenericPlatformMacros.hpp"
#include <string>

NS_FK_BEGIN
namespace network{
namespace MimeTypes
{
    std::string getMimeType(std::string fileName);
};
} // namespace network
NS_FK_END



#endif // FOUNDATIONKIT_MIMETYPES_HPP
