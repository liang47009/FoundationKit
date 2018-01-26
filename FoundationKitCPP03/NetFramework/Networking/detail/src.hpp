/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.
 
  losemymind.libo@gmail.com

****************************************************************************/
#ifndef FOUNDATIONKIT_NETWORKING_SRC_HPP
#define FOUNDATIONKIT_NETWORKING_SRC_HPP

#define NETWORK_SOURCE
#include "Networking/detail/config.hpp"

#if defined(NETWORK_HEADER_ONLY)
# error Do not compile Networking library source with NETWORK_HEADER_ONLY defined
#endif

#include "Networking/ip/address.ipp"
#include "Networking/ip/address_v4.ipp"
#include "Networking/ip/address_v6.ipp"
#include "Networking/ip/endpoint.ipp"
#include "Networking/winsock_init.ipp"

#endif // END OF FOUNDATIONKIT_NETWORKING_SRC_HPP


