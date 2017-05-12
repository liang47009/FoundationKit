

#include <sstream>
#include <stdlib.h>
#include <mach/machine.h>
#include <sys/types.h>
#include <sys/sysctl.h>
#include <sys/utsname.h>
#include "FoundationKit/Platform/Environment.hpp"
#include "FoundationKit/Foundation/Exception.hpp"
#include "FoundationKit/Foundation/StringUtils.hpp"

NS_FK_BEGIN
static std::string GSavedCommandLine = "";
Environment::stringvec Environment::GetEnvironmentVariables()
{
    return stringvec();
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

void Environment::SetEnvironmentVariable(const std::string& variable, const std::string& value)
{
    std::string var = variable;
    var.append("=");
    var.append(value);
    if (putenv((char*)var.c_str()))
    {
        std::string msg = "cannot set environment variable: ";
        msg.append(variable);
        throw SystemException(msg);
    }
}

Environment::stringvec Environment::GetCommandLineArgs()
{
    stringvec commandArgs = StringUtils::split(GSavedCommandLine, ' ');
    return commandArgs;
}

NS_FK_END




