/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.
 
  losemymind.libo@gmail.com

****************************************************************************/
#ifndef FOUNDATIONKIT_NETWORKING_WINSOCK_INIT_IPP
#define FOUNDATIONKIT_NETWORKING_WINSOCK_INIT_IPP

#include "socket_types.hpp"
#include "winsock_init.hpp"

#if PLATFORM_WINDOWS
NS_FK_BEGIN

void winsock_init_base::startup(data& d,
    unsigned char major, unsigned char minor)
{
  if (::InterlockedIncrement(&d.init_count_) == 1)
  {
    WSADATA wsa_data;
    long result = ::WSAStartup(MAKEWORD(major, minor), &wsa_data);
    ::InterlockedExchange(&d.result_, result);
  }
}

void winsock_init_base::cleanup(data& d)
{
  if (::InterlockedDecrement(&d.init_count_) == 0)
  {
    ::WSACleanup();
  }
}

void winsock_init_base::throw_on_error(data& d)
{
  long result = ::InterlockedExchangeAdd(&d.result_, 0);
  if (result != 0)
  {

  }
}


NS_FK_END

#endif //PLATFORM_WINDOWS

#endif // END OF FOUNDATIONKIT_NETWORKING_WINSOCK_INIT_IPP
