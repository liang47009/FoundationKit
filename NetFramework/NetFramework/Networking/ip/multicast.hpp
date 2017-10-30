/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.
 
  losemymind.libo@gmail.com

****************************************************************************/
#ifndef FOUNDATIONKIT_NETWORKING_MULTICAST_HPP
#define FOUNDATIONKIT_NETWORKING_MULTICAST_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <cstddef>
#include "Networking/detail/socket_option.hpp"

NS_FK_BEGIN

namespace network {
namespace ip {
namespace multicast {

/// Socket option to join a multicast group on a specified interface.
/**
 * Implements the IPPROTO_IP/IP_ADD_MEMBERSHIP socket option.
 *
 * @par Examples
 * Setting the option to join a multicast group:
 * @code
 * network::ip::udp::socket socket(io_service); 
 * ...
 * network::ip::address multicast_address =
 *   network::ip::address::from_string("225.0.0.1");
 * network::ip::multicast::join_group option(multicast_address);
 * socket.set_option(option);
 * @endcode
 *
 * @par Concepts:
 * SettableSocketOption.
 */
typedef network::socket_option::multicast_request<
  NET_OS_DEF(IPPROTO_IP),
  NET_OS_DEF(IP_ADD_MEMBERSHIP),
  NET_OS_DEF(IPPROTO_IPV6),
  NET_OS_DEF(IPV6_JOIN_GROUP)> join_group;

/// Socket option to leave a multicast group on a specified interface.
/**
 * Implements the IPPROTO_IP/IP_DROP_MEMBERSHIP socket option.
 *
 * @par Examples
 * Setting the option to leave a multicast group:
 * @code
 * network::ip::udp::socket socket(io_service); 
 * ...
 * network::ip::address multicast_address =
 *   network::ip::address::from_string("225.0.0.1");
 * network::ip::multicast::leave_group option(multicast_address);
 * socket.set_option(option);
 * @endcode
 *
 * @par Concepts:
 * SettableSocketOption.
 */
typedef network::socket_option::multicast_request<
  NET_OS_DEF(IPPROTO_IP),
  NET_OS_DEF(IP_DROP_MEMBERSHIP),
  NET_OS_DEF(IPPROTO_IPV6),
  NET_OS_DEF(IPV6_LEAVE_GROUP)> leave_group;

/// Socket option for local interface to use for outgoing multicast packets.
/**
 * Implements the IPPROTO_IP/IP_MULTICAST_IF socket option.
 *
 * @par Examples
 * Setting the option:
 * @code
 * network::ip::udp::socket socket(io_service); 
 * ...
 * network::ip::address_v4 local_interface =
 *   network::ip::address_v4::from_string("1.2.3.4");
 * network::ip::multicast::outbound_interface option(local_interface);
 * socket.set_option(option);
 * @endcode
 *
 * @par Concepts:
 * SettableSocketOption.
 */
typedef network::socket_option::network_interface<
  NET_OS_DEF(IPPROTO_IP),
  NET_OS_DEF(IP_MULTICAST_IF),
  NET_OS_DEF(IPPROTO_IPV6),
  NET_OS_DEF(IPV6_MULTICAST_IF)> outbound_interface;


/// Socket option for time-to-live associated with outgoing multicast packets.
/**
 * Implements the IPPROTO_IP/IP_MULTICAST_TTL socket option.
 *
 * @par Examples
 * Setting the option:
 * @code
 * network::ip::udp::socket socket(io_service); 
 * ...
 * network::ip::multicast::hops option(4);
 * socket.set_option(option);
 * @endcode
 *
 * @par
 * Getting the current option value:
 * @code
 * network::ip::udp::socket socket(io_service); 
 * ...
 * network::ip::multicast::hops option;
 * socket.get_option(option);
 * int ttl = option.value();
 * @endcode
 *
 * @par Concepts:
 * GettableSocketOption, SettableSocketOption.
 */
typedef network::socket_option::multicast_hops<
  NET_OS_DEF(IPPROTO_IP),
  NET_OS_DEF(IP_MULTICAST_TTL),
  NET_OS_DEF(IPPROTO_IPV6),
  NET_OS_DEF(IPV6_MULTICAST_HOPS)> hops;


/// Socket option determining whether outgoing multicast packets will be
/// received on the same socket if it is a member of the multicast group.
/**
 * Implements the IPPROTO_IP/IP_MULTICAST_LOOP socket option.
 *
 * @par Examples
 * Setting the option:
 * @code
 * network::ip::udp::socket socket(io_service); 
 * ...
 * network::ip::multicast::enable_loopback option(true);
 * socket.set_option(option);
 * @endcode
 *
 * @par
 * Getting the current option value:
 * @code
 * network::ip::udp::socket socket(io_service); 
 * ...
 * network::ip::multicast::enable_loopback option;
 * socket.get_option(option);
 * bool is_set = option.value();
 * @endcode
 *
 * @par Concepts:
 * GettableSocketOption, SettableSocketOption.
 */
typedef network::socket_option::multicast_enable_loopback<
  NET_OS_DEF(IPPROTO_IP),
  NET_OS_DEF(IP_MULTICAST_LOOP),
  NET_OS_DEF(IPPROTO_IPV6),
  NET_OS_DEF(IPV6_MULTICAST_LOOP)> enable_loopback;


} // namespace multicast
} // namespace ip
} // namespace network
NS_FK_END

#endif // END OF FOUNDATIONKIT_NETWORKING_MULTICAST_HPP
