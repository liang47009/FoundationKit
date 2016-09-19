#ifndef LOSEMYMIND_ERROR_CODE_HPP
#define LOSEMYMIND_ERROR_CODE_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <system_error>
#include "FoundationKit/GenericPlatformMacros.h"
#include "FoundationKit/Base/noncopyable.hpp"

NS_FK_BEGIN

template <typename Exception>
inline void throw_exception(const Exception& e)
{
    throw e;
}

static void throw_error_if(const std::error_code& err, const char* location = nullptr)
{
    if (err)
    {
        if (location != nullptr)
        {
            std::string what_msg = location;
            what_msg += ": ";
            what_msg += err.message();
            std::system_error e(err, what_msg);
            throw_exception(e);
        }
        else
        {
            std::system_error e(err, err.message());
            throw_exception(e);
        } 
    }
}

NS_FK_END

namespace std
{
    enum class misc_errors
    {
        /// Already open.
        already_open = 1,

        /// End of file or stream.
        eof,

        /// Element not found.
        not_found,

        /// The descriptor cannot fit into the select system call's fd_set.
        fd_set_failure
    };

    inline std::error_code make_error_code(misc_errors _Errno)
    {	// make an error_code
        return (std::error_code((int)_Errno, std::generic_category()));
    }

}



#endif // LOSEMYMIND_ERROR_CODE_HPP
