/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.
 
  losemymind.libo@gmail.com

****************************************************************************/
#ifndef FOUNDATIONKIT_ERROR_HPP
#define FOUNDATIONKIT_ERROR_HPP

#include "FoundationKit/GenericPlatformMacros.hpp"
#include <system_error>
#include <string>
#include "FoundationKit/Base/lexical_cast.hpp"

NS_FK_BEGIN

template <typename Exception>
inline void throw_exception(const Exception& e)
{
    throw e;
}

static inline void throw_error_if(const std::error_code& err, const char* file = "", int line = 0)
{
    if (err)
    {
        std::string what_msg = file;
        if (!what_msg.empty())
        {
            what_msg += "(";
            what_msg += lexical_cast<std::string>(line);
            what_msg += "):";
        }
        what_msg += err.message();
        std::system_error e(err, what_msg);
        throw_exception(e);
    }
}

NS_FK_END

#define THROW_IF(ERR) FK_NAME throw_error_if(ERR, __FILE__, __LINE__)

#endif // END OF FOUNDATIONKIT_ERROR_HPP