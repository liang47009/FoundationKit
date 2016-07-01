#ifndef LOSEMYMIND_DETAIL_ENDPOINT_HPP
#define LOSEMYMIND_DETAIL_ENDPOINT_HPP
#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <string>
#include "FoundationKit/Networking/socket_types.hpp"
#include "FoundationKit/Networking/winsock_init.hpp"
#include "FoundationKit/Networking/ip/address.hpp"
#include "FoundationKit/Base/error_code.hpp"
NS_FK_BEGIN
namespace network{
namespace ip{

// Helper class for implementating an IP endpoint.
class endpoint
{
public:
    // Default constructor.
    endpoint();

    // Construct an endpoint using a family and port number.
    endpoint(int family, unsigned short port_num);

    // Construct an endpoint using an address and port number.
    endpoint(const ip::address& addr, unsigned short port_num);

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
    void resize(std::size_t new_size);

    // Get the capacity of the endpoint in the native type.
    std::size_t capacity() const
    {
        return sizeof(data_);
    }

    // Get the port associated with the endpoint.
    unsigned short port() const;

    // Set the port associated with the endpoint.
    void port(unsigned short port_num);

    // Get the IP address associated with the endpoint.
    ip::address address() const;

    // Set the IP address associated with the endpoint.
    void address(const ip::address& addr);

    // Compare two endpoints for equality.
    friend bool operator==(const endpoint& e1, const endpoint& e2);

    // Compare endpoints for ordering.
    friend bool operator<(const endpoint& e1, const endpoint& e2);

    // Determine whether the endpoint is IPv4.
    bool is_v4() const
    {
        return data_.base.sa_family == NET_OS_DEF(AF_INET);
    }

    // Convert to a string.
    std::string to_string() const;


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

# include "FoundationKit/Networking/ip/detail/impl/endpoint.ipp"

#endif // LOSEMYMIND_DETAIL_ENDPOINT_HPP
