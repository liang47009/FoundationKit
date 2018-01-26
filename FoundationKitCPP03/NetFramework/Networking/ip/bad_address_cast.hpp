/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.
 
  losemymind.libo@gmail.com

****************************************************************************/
#ifndef FOUNDATIONKIT_NETWORKING_BAD_ADDRESS_CAST_HPP
#define FOUNDATIONKIT_NETWORKING_BAD_ADDRESS_CAST_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)
#include <typeinfo>
#include "FoundationKit/GenericPlatformMacros.hpp"
NS_FK_BEGIN
namespace network{
namespace ip{

/// Thrown to indicate a failed address conversion.
class bad_address_cast : public std::bad_cast
{
public:
  /// Default constructor.
  bad_address_cast() {}

  /// Destructor.
  virtual ~bad_address_cast() throw() {}

  /// Get the message associated with the exception.
  virtual const char* what() const throw()
  {
    return "bad address cast";
  }
};

} // namespace ip
} // namespace network
NS_FK_END

#endif // END OF FOUNDATIONKIT_NETWORKING_BAD_ADDRESS_CAST_HPP
