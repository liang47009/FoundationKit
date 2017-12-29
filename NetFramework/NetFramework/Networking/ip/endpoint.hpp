/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.
 
  losemymind.libo@gmail.com

****************************************************************************/
#ifndef FOUNDATIONKIT_NETWORKING_ENDPOINT_HPP
#define FOUNDATIONKIT_NETWORKING_ENDPOINT_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <string>
#include "Networking/net_error_code.hpp"
#include "Networking/socket_types.hpp"
#include "Networking/winsock_init.hpp"
#include "Networking/ip/address.hpp"
#include "Networking/detail/config.hpp"

NS_FK_BEGIN
namespace network{
namespace ip{

// Helper class for implementating an IP endpoint.
class endpoint
{
public:
    // Default constructor.
    NETWORK_API endpoint();

    // Construct an endpoint using a family and port number.
    NETWORK_API endpoint(int family, unsigned short port_num);

    // Construct an endpoint using an address and port number.
    NETWORK_API endpoint(const ip::address& addr, unsigned short port_num);

    // Copy constructor.
    endpoint(const endpoint& other)
        : data_(other.data_)
    {
    }

    // Assign from another endpoint.
    endpoint& operator=(const endpoint& other)
    {
        data_ = other.data_;
        return *this;
    }

    // Get the underlying endpoint in the native type.
    socket_addr_type* data()
    {
        return &data_.base;
    }

    // Get the underlying endpoint in the native type.
    const socket_addr_type* data() const
    {
        return &data_.base;
    }

    // Get the underlying size of the endpoint in the native type.
    std::size_t size() const
    {
        if (is_v4())
            return sizeof(sockaddr_in4_type);
        else
            return sizeof(sockaddr_in6_type);
    }

    // Set the underlying size of the endpoint in the native type.
    NETWORK_API void resize(std::size_t new_size);

    // Get the capacity of the endpoint in the native type.
    std::size_t capacity() const
    {
        return sizeof(data_);
    }

    // Get the port associated with the endpoint.
    NETWORK_API unsigned short port() const;

    // Set the port associated with the endpoint.
    NETWORK_API void port(unsigned short port_num);

    // Get the IP address associated with the endpoint.
    NETWORK_API ip::address address() const;

    // Set the IP address associated with the endpoint.
    NETWORK_API void address(const ip::address& addr);

    // Compare two endpoints for equality.
    NETWORK_API friend bool operator==(const endpoint& e1, const endpoint& e2);

    // Compare endpoints for ordering.
    NETWORK_API friend bool operator<(const endpoint& e1, const endpoint& e2);

    // Determine whether the endpoint is IPv4.
    bool is_v4() const
    {
        return data_.base.sa_family == NET_OS_DEF(AF_INET);
    }

    // Convert to a string.
    NETWORK_API std::string to_string() const;


private:
    // The underlying IP socket address.
    union data_union
    {
        socket_addr_type base;
        sockaddr_in4_type v4;
        sockaddr_in6_type v6;
    } data_;
};

} // namespace ip
} // namespace network
NS_FK_END

# include "Networking/ip/endpoint.ipp"

#endif // END OF FOUNDATIONKIT_NETWORKING_ENDPOINT_HPP
