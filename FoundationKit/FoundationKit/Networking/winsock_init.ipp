#ifndef LOSEMYMIND_WINSOCK_INIT_IPP
#define LOSEMYMIND_WINSOCK_INIT_IPP
# pragma once

#include "socket_types.hpp"
#include "winsock_init.hpp"

#ifdef TARGET_PLATFORM == PLATFORM_WIN32
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

#endif //TARGET_PLATFORM == PLATFORM_WIN32

#endif //LOSEMYMIND_WINSOCK_INIT_IPP
