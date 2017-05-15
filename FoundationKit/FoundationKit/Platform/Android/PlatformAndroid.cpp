#ifdef ANDROID
#include <malloc.h>
#include "FoundationKit/Platform/Platform.hpp"


NS_FK_BEGIN
bool Platform::IsDebuggerPresent()
{
    return false;
}

size_t Platform::MallocUsableSize(void* ptr)
{
    return malloc_usable_size(ptr);
}

NS_FK_END

#endif //#ifdef ANDROID