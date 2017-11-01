
#include "FoundationKit/GenericPlatformMacros.hpp"
#if (PLATFORM_IOS || PLATFORM_MAC)

#include <sstream>
#include <stdlib.h>
#include <mach/machine.h>
#include <sys/types.h>
#include <sys/sysctl.h>
#include <sys/utsname.h>
#include <unistd.h>
#include "FoundationKit/Platform/Environment.hpp"
#include "FoundationKit/Foundation/Exception.hpp"
#include "FoundationKit/Foundation/StringUtils.hpp"
#import <Foundation/Foundation.h>
extern char** environ;
NS_FK_BEGIN
Environment::stringvec Environment::GetEnvironmentVariables()
{
    // We can use:
    //[[NSProcessInfo processInfo]environment]
    stringvec Variables;
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
    return getenv(variable.c_str()) != 0;
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

Environment::stringvec Environment::GetCommandLineArgs()
{
    stringvec commandArgs;
    NSArray* arguments = [[NSProcessInfo processInfo] arguments];
    for (NSString *item in arguments)
    {
        commandArgs.push_back([item UTF8String]);
    }
    return commandArgs;
}

NS_FK_END

#endif //OF #if (PLATFORM_IOS || PLATFORM_MAC)




