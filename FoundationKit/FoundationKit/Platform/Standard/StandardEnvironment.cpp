/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.

  losemymind.libo@gmail.com

****************************************************************************/
#include "FoundationKit/GenericPlatformMacros.hpp"
#if PLATFORM_ANDROID || PLATFORM_APPLE || PLATFORM_LINUX || PLATFORM_SWITCH

//#include <sstream>
#include <stdlib.h>
#include <unistd.h> // for environ
#include "FoundationKit/Platform/Environment.hpp"
#include "FoundationKit/Foundation/StringUtils.hpp"
extern char ** environ; // in <unistd.h>
NS_FK_BEGIN

Environment::stringvec Environment::GetEnvironmentVariables()
{
    stringvec  Variables;
    char** env = environ;
    while (*env)
    {
        Variables.push_back(*env);
        ++env;
    }
    return Variables;
}

std::string Environment::GetEnvironmentVariable(const std::string& variable)
{
    const char* val = getenv(variable.c_str());
    if (val)
        return std::string(val);
    else
        return "";//throw NotFoundException(variable);
}

bool Environment::HasEnvironmentVariable(const std::string& variable)
{
    return getenv(variable.c_str()) != nullptr;
}

bool Environment::SetEnvironmentVariable(const std::string& variable, const std::string& value)
{
    bool result = true;
    if (value.empty())
    {
        if (unsetenv(variable.c_str()) != 0)
        {
            result = false;
            std::string msg = "cannot set environment variable: ";
            msg.append(variable);
            ASSERTED(false, msg.c_str());
        }
    }
    else
    {
        std::string var = variable;
        var.append("=");
        var.append(value);
        if (putenv((char*)var.c_str()) != 0)
        {
            result = false;
            std::string msg = "cannot set environment variable: ";
            msg.append(variable);
            ASSERTED(false, msg.c_str());
        }
    }
    return result;
}

NS_FK_END

#endif //#if PLATFORM_ANDROID || PLATFORM_APPLE || PLATFORM_LINUX || PLATFORM_SWITCH
