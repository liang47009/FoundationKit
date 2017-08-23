#ifndef LOSEMYMIND_IMPL_BASIC_ENDPOINT_HPP
#define LOSEMYMIND_IMPL_BASIC_ENDPOINT_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include "FoundationKit/GenericPlatformMacros.hpp"
#include "Networking/ip/basic_endpoint.hpp"

NS_FK_BEGIN
namespace network{
namespace ip
{

template <typename Elem, typename Traits, typename InternetProtocol>
std::basic_ostream<Elem, Traits>& operator<<(
    std::basic_ostream<Elem, Traits>& os,
    const basic_endpoint<InternetProtocol>& endpoint)
{
    ip::endpoint tmp_ep(endpoint.address(), endpoint.port());
    return os << tmp_ep.to_string().c_str();
}

} // namespace ip
} // namespace network
NS_FK_END

#endif // LOSEMYMIND_IMPL_BASIC_ENDPOINT_HPP





