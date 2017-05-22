#ifdef ANDROID
#include <malloc.h>
#include "FoundationKit/Platform/Platform.hpp"

NS_FK_BEGIN
bool Platform::IsDebuggerPresent()
{
    return false;
}

struct chunk {
    size_t psize, csize;
    struct chunk *next, *prev;
};

#if __ANDROID_API__ < 17
#define OVERHEAD (2*sizeof(size_t))
#define CHUNK_SIZE(c) ((c)->csize & -2)
#define MEM_TO_CHUNK(p) (struct chunk *)((char *)(p) - OVERHEAD)
#endif
size_t Platform::MallocUsableSize(void* ptr)
{
#if __ANDROID_API__ < 17
    return ptr ? CHUNK_SIZE(MEM_TO_CHUNK(ptr)) - OVERHEAD : 0;
#else
    return malloc_usable_size(ptr);
#endif
}

NS_FK_END

#endif //#ifdef ANDROID



