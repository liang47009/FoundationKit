

#include <string>
#include "FoundationKit/GenericPlatformMacros.h"

NS_FK_BEGIN

class Environment
{
public:
    static std::string GetEnvironmentVariable(const std::string& variable);
    static bool        HasEnvironmentVariable(const std::string& variable);
    static void        SetEnvironmentVariable(const std::string& variable, const std::string& value);
    static std::string GetMachineName();
    static std::string GetOSVersion();
};


NS_FK_END


