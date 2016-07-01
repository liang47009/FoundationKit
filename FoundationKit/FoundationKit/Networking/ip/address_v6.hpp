#ifndef LOSEMYMIND_ADDRESS_V6_HPP
#define LOSEMYMIND_ADDRESS_V6_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <string>
#include <iosfwd>
#include <array>
#include "FoundationKit/GenericPlatformMacros.h"
#include "FoundationKit/Base/Types.h"
#include "FoundationKit/Base/error_code.hpp"
#include "FoundationKit/Networking/socket_types.hpp"
#include "FoundationKit/Networking/winsock_init.hpp"
#include "FoundationKit/Networking/ip/address_v4.hpp"

NS_FK_BEGIN
namespace network{
namespace ip
{
template <typename> class basic_address_iterator;

/// Implements IP version 6 style addresses.
/**
* The asio::ip::address_v6 class provides the ability to use and
* manipulate IP version 6 addresses.
*
* @par Thread Safety
* @e Distinct @e objects: Safe.@n
* @e Shared @e objects: Unsafe.
*/
class address_v6
{
public:
    /// The type used to represent an address as an array of bytes.
    /**
    * @note This type is defined in terms of the C++0x template @c std::array
    * when it is available. Otherwise, it uses @c boost:array.
    */
    typedef std::array<unsigned char, 16> bytes_type;

    /// Default constructor.
    address_v6();

    /// Construct an address from raw bytes and scope ID.
    explicit address_v6(const bytes_type& bytes, unsigned long scope_id = 0);

    /// Copy constructor.
    address_v6(const address_v6& other);

    /// Move constructor.
    address_v6(address_v6&& other);

    /// Assign from another address.
    address_v6& operator=(const address_v6& other);

    /// Move-assign from another address.
    address_v6& operator=(address_v6&& other);

    /// The scope ID of the address.
    /**
    * Returns the scope ID associated with the IPv6 address.
    */
    unsigned long scope_id() const
    {
        return scope_id_;
    }

    /// The scope ID of the address.
    /**
    * Modifies the scope ID associated with the IPv6 address.
    */
    void scope_id(unsigned long id)
    {
        scope_id_ = id;
    }

    /// Get the address in bytes, in network byte order.
    bytes_type to_bytes() const;

    /// Get the address as a string.
    std::string to_string() const;

    /// (Deprecated: Use other overload.) Get the address as a string.
    std::string to_string(std::error_code& ec) const;

    /// (Deprecated: Use make_address_v6().) Create an IPv6 address from an IP
    /// address string.
    static address_v6 from_string(const char* str);

    /// (Deprecated: Use make_address_v6().) Create an IPv6 address from an IP
    /// address string.
    static address_v6 from_string(
        const char* str, std::error_code& ec);

    /// (Deprecated: Use make_address_v6().) Create an IPv6 address from an IP
    /// address string.
    static address_v6 from_string(const std::string& str);

    /// (Deprecated: Use make_address_v6().) Create an IPv6 address from an IP
    /// address string.
    static address_v6 from_string(const std::string& str, std::error_code& ec);

    /// (Deprecated: Use make_address_v4().) Converts an IPv4-mapped or
    /// IPv4-compatible address to an IPv4 address.
    address_v4 to_v4() const;

    /// Determine whether the address is a loopback address.
    bool is_loopback() const;

    /// Determine whether the address is unspecified.
    bool is_unspecified() const;

    /// Determine whether the address is link local.
    bool is_link_local() const;

    /// Determine whether the address is site local.
    bool is_site_local() const;

    /// Determine whether the address is a mapped IPv4 address.
    bool is_v4_mapped() const;

    /// (Deprecated: No replacement.) Determine whether the address is an
    /// IPv4-compatible address.
    bool is_v4_compatible() const;

    /// Determine whether the address is a multicast address.
    bool is_multicast() const;

    /// Determine whether the address is a global multicast address.
    bool is_multicast_global() const;

    /// Determine whether the address is a link-local multicast address.
    bool is_multicast_link_local() const;

    /// Determine whether the address is a node-local multicast address.
    bool is_multicast_node_local() const;

    /// Determine whether the address is a org-local multicast address.
    bool is_multicast_org_local() const;

    /// Determine whether the address is a site-local multicast address.
    bool is_multicast_site_local() const;

    /// Compare two addresses for equality.
    friend bool operator==(const address_v6& a1, const address_v6& a2);

    /// Compare two addresses for inequality.
    friend bool operator!=(const address_v6& a1, const address_v6& a2)
    {
        return !(a1 == a2);
    }

    /// Compare addresses for ordering.
    friend bool operator<(const address_v6& a1, const address_v6& a2);

    /// Compare addresses for ordering.
    friend bool operator>(const address_v6& a1, const address_v6& a2)
    {
        return a2 < a1;
    }

    /// Compare addresses for ordering.
    friend bool operator<=(const address_v6& a1, const address_v6& a2)
    {
        return !(a2 < a1);
    }

    /// Compare addresses for ordering.
    friend bool operator>=(const address_v6& a1, const address_v6& a2)
    {
        return !(a1 < a2);
    }

    /// Obtain an address object that represents any address.
    static address_v6 any()
    {
        return address_v6();
    }

    /// Obtain an address object that represents the loopback address.
    static address_v6 loopback();

    /// (Deprecated: Use make_address_v6().) Create an IPv4-mapped IPv6 address.
    static address_v6 v4_mapped(const address_v4& addr);

    /// (Deprecated: No replacement.) Create an IPv4-compatible IPv6 address.
    static address_v6 v4_compatible(const address_v4& addr);

private:
    friend class basic_address_iterator<address_v6>;

    // The underlying IPv6 address.
    in6_addr_type addr_;

    // The scope ID associated with the address.
    unsigned long scope_id_;
};

/// Create an IPv6 address from raw bytes and scope ID.
/**
* @relates address_v6
*/
inline address_v6 make_address_v6(const address_v6::bytes_type& bytes,unsigned long scope_id = 0)
{
    return address_v6(bytes, scope_id);
}

/// Create an IPv6 address from an IP address string.
/**
* @relates address_v6
*/
address_v6 make_address_v6(const char* str);

/// Create an IPv6 address from an IP address string.
/**
* @relates address_v6
*/
address_v6 make_address_v6(const char* str, std::error_code& ec);

/// Createan IPv6 address from an IP address string.
/**
* @relates address_v6
*/
address_v6 make_address_v6(const std::string& str);

/// Create an IPv6 address from an IP address string.
/**
* @relates address_v6
*/
address_v6 make_address_v6( const std::string& str, std::error_code& ec);

/// Tag type used for distinguishing overloads that deal in IPv4-mapped IPv6
/// addresses.
enum v4_mapped_t { v4_mapped };

/// Create an IPv4 address from a IPv4-mapped IPv6 address.
/**
* @relates address_v4
*/
address_v4 make_address_v4(v4_mapped_t, const address_v6& v6_addr);

/// Create an IPv4-mapped IPv6 address from an IPv4 address.
/**
* @relates address_v6
*/
address_v6 make_address_v6( v4_mapped_t, const address_v4& v4_addr);

/// Output an address as a string.
/**
* Used to output a human-readable string for a specified address.
*
* @param os The output stream to which the string will be written.
*
* @param addr The address to be written.
*
* @return The output stream.
*
* @relates asio::ip::address_v6
*/
template <typename Elem, typename Traits>
std::basic_ostream<Elem, Traits>& operator<<(std::basic_ostream<Elem, Traits>& os, const address_v6& addr);

} // namespace ip
} // namespace network

NS_FK_END

#include "FoundationKit/Networking/ip/impl/address_v6.hpp"
#include "FoundationKit/Networking/ip/impl/address_v6.ipp"

#endif // LOSEMYMIND_ADDRESS_V6_HPP

