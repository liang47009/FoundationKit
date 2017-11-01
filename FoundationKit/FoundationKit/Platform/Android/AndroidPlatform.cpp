
#include "FoundationKit/GenericPlatformMacros.hpp"
#if PLATFORM_ANDROID

#include <malloc.h>
#include <errno.h>
#include "FoundationKit/Platform/Platform.hpp"

NS_FK_BEGIN
bool Platform::IsDebuggerPresent()
{
    return false;
}

std::string Platform::ExecuteSystemCommand(const std::string& command)
{
    std::string result = "";
    FILE* pipe = popen(command.c_str(), "r");
    if (!pipe)
    {
        FKLog("****** popen() failed!");
    }
    try {
        char buffer[256+1] = { 0 };
        while (!feof(pipe))
        {
            if (fgets(buffer, 256, pipe) != NULL)
                result += buffer;
        }
    }
    catch (...)
    {
        pclose(pipe);
        FKLog("****** Cannot execute command:%s with errno:%d", command.c_str(), errno);
    }
    pclose(pipe);
    return result;
}


NS_FK_END

#endif //#if PLATFORM_ANDROID



