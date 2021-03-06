#include "FoundationKit/GenericPlatformMacros.hpp"
#if TARGET_PLATFORM == PLATFORM_ANDROID

#include <sstream>
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

Environment::stringvec Environment::GetCommandLineArgs()
{
    stringvec commandArgs;// = StringUtils::Split(GSavedCommandLine, ' ');
    FILE *fp = nullptr;
    if ((fp = fopen("/proc/self/cmdline", "r")) == NULL)
    {
        FKLog("Cannot open /proc/self/cmdline file!");
        return commandArgs;
    }
    char line_buf[256] = { 0 };
    while (fgets(line_buf, sizeof(line_buf), fp) != NULL)
    {
        commandArgs.push_back(line_buf);
    }
    return commandArgs;
}

NS_FK_END

#endif //#if TARGET_PLATFORM == PLATFORM_ANDROID



