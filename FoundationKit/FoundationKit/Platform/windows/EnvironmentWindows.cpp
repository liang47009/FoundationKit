
#include <sstream>
#include "FoundationKit/Platform/Environment.hpp"
#include "FoundationKit/Foundation/Logger.hpp"

NS_FK_BEGIN

std::string Environment::GetEnvironmentVariable(const std::string& variable)
{
    DWORD len = GetEnvironmentVariableA(variable.c_str(), 0, 0);
    if (len == 0) return ""; //throw NotFoundException(variable);
    char* buffer = new char[len];
    GetEnvironmentVariableA(variable.c_str(), buffer, len);
    std::string result(buffer);
    delete[] buffer;
    return result;
}

bool Environment::HasEnvironmentVariable(const std::string& variable)
{
    DWORD len = GetEnvironmentVariableA(variable.c_str(), 0, 0);
    return len > 0;
}

void Environment::SetEnvironmentVariable(const std::string& variable, const std::string& value)
{
    if (SetEnvironmentVariableA(variable.c_str(), value.c_str()) == 0)
    {
        std::string msg = "cannot set environment variable: ";
        msg.append(variable);
        ASSERTED(false, msg.c_str());
    }
}
NS_FK_END

