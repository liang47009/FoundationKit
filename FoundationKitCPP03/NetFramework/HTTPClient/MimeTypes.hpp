/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.
 
  losemymind.libo@gmail.com

****************************************************************************/
#ifndef NETFRAMEWORK_MIMETYPES_HPP
#define NETFRAMEWORK_MIMETYPES_HPP
#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)
#include <string>
#include "FoundationKit/GenericPlatformMacros.hpp"
NS_FK_BEGIN
namespace MimeTypes
{
    std::string GetMimeType(std::string fileName);
};
NS_FK_END
//#include "HTTPClient/MimeTypes.ipp"

#endif // END OF NETFRAMEWORK_MIMETYPES_HPP
