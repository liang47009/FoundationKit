

#include <sstream>
#include "FoundationKit/Platform/Environment.h"
#include "FoundationKit/Foundation/Exception.h"

NS_FK_BEGIN

std::string Environment::GetEnvironmentVariable(const std::string& variable)
{
    DWORD len = GetEnvironmentVariableA(variable.c_str(), 0, 0);
    //LOG_ASSERT(len != 0, "The environment variable:%s not found", variable.c_str());
    if (len == 0) throw NotFoundException(variable);
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
        throw SystemException(msg);
    }
}


std::string Environment::GetMachineName()
{
    OSVERSIONINFOEX vi;	// OSVERSIONINFOEX is supported starting at Windows 2000 
    vi.dwOSVersionInfoSize = sizeof(vi);
    if (GetVersionEx((OSVERSIONINFO*)&vi) == 0) throw SystemException("Cannot get OS version information");
    switch (vi.dwMajorVersion)
    {
    case 10:
        switch (vi.dwMinorVersion)
        {
        case 0:
            return vi.wProductType == VER_NT_WORKSTATION ? "Windows 10" : "Windows Server 2016";
        }
    case 6:
        switch (vi.dwMinorVersion)
        {
        case 0:
            return vi.wProductType == VER_NT_WORKSTATION ? "Windows Vista" : "Windows Server 2008";
        case 1:
            return vi.wProductType == VER_NT_WORKSTATION ? "Windows 7" : "Windows Server 2008 R2";
        case 2:
            return vi.wProductType == VER_NT_WORKSTATION ? "Windows 8" : "Windows Server 2012";
        case 3:
            return vi.wProductType == VER_NT_WORKSTATION ? "Windows 8.1" : "Windows Server 2012 R2";
        default:
            return "Unknown";
        }
    case 5:
        switch (vi.dwMinorVersion)
        {
        case 0:
            return "Windows 2000";
        case 1:
            return "Windows XP";
        case 2:
            return "Windows Server 2003/Windows Server 2003 R2";
        default:
            return "Unknown";
        }
    default:
        return "Unknown";
    }
}


std::string Environment::GetOSVersion()
{
    OSVERSIONINFO vi;
    vi.dwOSVersionInfoSize = sizeof(vi);
    if (GetVersionEx(&vi) == 0) throw SystemException("Cannot get OS version information");
    std::ostringstream str;
    str << vi.dwMajorVersion << "." << vi.dwMinorVersion << " (Build " << (vi.dwBuildNumber & 0xFFFF);
    if (vi.szCSDVersion[0]) str << ": " << vi.szCSDVersion;
    str << ")";
    return str.str();
}



NS_FK_END




