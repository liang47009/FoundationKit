/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.
 
  losemymind.libo@gmail.com

****************************************************************************/
#ifndef HTTPCLIENT_HTTPCLIENTVERSION_HPP
#define HTTPCLIENT_HTTPCLIENTVERSION_HPP
#pragma once

#define HTTPCLIENT_VERSION_MAJOR 1
#define HTTPCLIENT_VERSION_MINOR 0
#define HTTPCLIENT_VERSION_BUILD 0
#define HTTPCLIENT_VERSION_REVISION 0
#define HTTPCLIENT_VERSION_FULL "1.0.0.0"


// Version as a single hex number, e.g. 0x01000300 == 1.0.3
#define HTTPCLIENT_VERSION_HEX (   (HTTPCLIENT_VERSION_MAJOR << 24) | \
                                   (HTTPCLIENT_VERSION_MINOR << 16) | \
                                   (HTTPCLIENT_VERSION_BUILD <<  8))
#define HTTPCLIENT_VERSION_STRING "HTTPClient Version:" HTTPCLIENT_VERSION_FULL

#define BUILD_TIMESTAMP "2018-03-19T09:43:07Z";

#endif // END OF HTTPCLIENT_HTTPCLIENTVERSION_HPP
