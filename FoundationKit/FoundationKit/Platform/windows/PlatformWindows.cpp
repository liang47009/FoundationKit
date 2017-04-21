/****************************************************************************
Copyright (c) 2015 libo.

losemymind.libo@gmail.com

****************************************************************************/
#include "FoundationKit/Platform/Platform.hpp"
#if (TARGET_PLATFORM == PLATFORM_WINDOWS)

NS_FK_BEGIN



bool Platform::IsDebuggerPresent()
{
    return ::IsDebuggerPresent() == TRUE;
}


NS_FK_END

#endif









