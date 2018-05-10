/****************************************************************************
Copyright (c) 2017 libo All rights reserved.

losemymind.libo@gmail.com

****************************************************************************/
#include "FoundationKit/GenericPlatformMacros.hpp"
#if (PLATFORM_WINDOWS)
#include <sstream>
#include "FoundationKit/Platform/Environment.hpp"
#include "FoundationKit/Foundation/StringUtils.hpp"

NS_FK_BEGIN
#define VARIABLE_SIZE	1024
Environment::stringvec Environment::GetEnvironmentVariables()
{
    LPWCH lpvEnv = GetEnvironmentStringsW();
    LPWSTR lpszVariable = (LPWSTR)lpvEnv;
    stringvec  Variables;
    while (*lpszVariable)
    {
        Variables.push_back(StringUtils::wstring2string(lpszVariable));
        lpszVariable += lstrlen(lpszVariable) + 1;
    }
    FreeEnvironmentStringsW(lpvEnv);
    return Variables;
}


std::string Environment::GetEnvironmentVariable(const std::string& variable)
{
    std::wstring VariableName = StringUtils::string2wstring(variable);
    wchar_t VariableValue[VARIABLE_SIZE] = { 0 };
    DWORD VariableLength = ::GetEnvironmentVariableW(VariableName.c_str(), VariableValue, VARIABLE_SIZE);
    if (VariableLength == 0)
        return std::string(); //throw NotFoundException(VariableName);
    if (VariableLength < VARIABLE_SIZE)
        return StringUtils::wstring2string(VariableValue);
    wchar_t* BigVariableValue = new wchar_t[VariableLength + 1];
    memset(BigVariableValue, 0, VariableLength + 1);
    ::GetEnvironmentVariableW(VariableName.c_str(), BigVariableValue, VariableLength);
    std::wstring result = BigVariableValue;
    delete[] BigVariableValue;
    return StringUtils::wstring2string(result);
}

bool Environment::HasEnvironmentVariable(const std::string& variable)
{
    std::wstring VariableName = StringUtils::string2wstring(variable);
    DWORD VariableLength = ::GetEnvironmentVariableW(VariableName.c_str(), 0, 0);
    return VariableLength > 0;
}

bool Environment::SetEnvironmentVariable(const std::string& variable, const std::string& value)
{
    bool ChangeVariableResult = false;
    std::wstring VariableName = StringUtils::string2wstring(variable);
    if (value.empty())
    {
        ChangeVariableResult = (::SetEnvironmentVariableW(VariableName.c_str(), NULL) == TRUE);
    }
    else 
    {
        std::wstring VariableValue = StringUtils::string2wstring(value);
        ChangeVariableResult = (::SetEnvironmentVariableW(VariableName.c_str(), VariableValue.c_str()) == TRUE);
        if (!ChangeVariableResult)
        {
            std::string msg = "cannot set environment variable: ";
            msg.append(variable);
            ASSERTED(false, msg.c_str());
        }
    }
    return ChangeVariableResult;
}

NS_FK_END

#endif // OF #if (PLATFORM_WINDOWS)
