
#include "FoundationKit/GenericPlatformMacros.hpp"
#include <string>
#include <vector>

NS_FK_BEGIN

class PlatformMisc
{
public:
    static std::vector<std::string>   GetCommandLineArgs();
};

NS_FK_END