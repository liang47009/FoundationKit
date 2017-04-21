#ifdef ANDROID

#include "FoundationKit/Platform/Platform.hpp"


NS_FK_BEGIN
bool Platform::IsDebuggerPresent()
{
    return false;
}

NS_FK_END

#endif //#ifdef ANDROID