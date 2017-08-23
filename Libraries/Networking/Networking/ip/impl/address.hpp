#ifndef LOSEMYMIND_IMPL_ADDRESS_HPP
#define LOSEMYMIND_IMPL_ADDRESS_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include "FoundationKit/GenericPlatformMacros.hpp"
#include "FoundationKit/Base/error_code.hpp"

NS_FK_BEGIN
namespace network{
namespace ip{

inline address address::from_string(const char* str)
{
    return ip::make_address(str);
}

inline address address::from_string( const char* str, std::error_code& ec)
{
    return ip::make_address(str, ec);
}

inline address address::from_string(const std::string& str)
{
    return ip::make_address(str);
}

inline address address::from_string(  const std::string& str, std::error_code& ec)
{
    return ip::make_address(str, ec);
}

template <typename Elem, typename Traits>
std::basic_ostream<Elem, Traits>& operator<<( std::basic_ostream<Elem, Traits>& os, const address& addr)
{
  return os << addr.to_string().c_str();
}

} // namespace ip
} // namespace network
NS_FK_END

#endif // LOSEMYMIND_IMPL_ADDRESS_HPP



