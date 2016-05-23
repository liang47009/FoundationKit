

#include <sstream>
#include <stdlib.h>
#include <sys/system_properties.h>//for __system_property_get
#include "FoundationKit/Platform/Environment.h"
#include "FoundationKit/Foundation/Exception.h"

NS_FK_BEGIN

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


std::string Environment::GetMachineName()
{
    char propValue[PROP_VALUE_MAX + 1] = { 0 };
    __system_property_get("ro.product.model", propValue);
    return propValue;
}


std::string Environment::GetOSVersion()
{
    char propValue[PROP_VALUE_MAX + 1] = { 0 };
    __system_property_get("ro.build.version.release", propValue);
    return propValue;
}



NS_FK_END




