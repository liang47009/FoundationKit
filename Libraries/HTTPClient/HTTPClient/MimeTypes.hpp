/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.
 
  losemymind.libo@gmail.com

****************************************************************************/
#ifndef FOUNDATIONKIT_MIMETYPES_HPP
#define FOUNDATIONKIT_MIMETYPES_HPP

#pragma once
#include <string>
#include "FoundationKit/GenericPlatformMacros.hpp"
NS_FK_BEGIN
namespace MimeTypes
{
    std::string getMimeType(std::string fileName);
};
NS_FK_END
#endif // FOUNDATIONKIT_MIMETYPES_HPP
