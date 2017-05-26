#ifdef ANDROID
#include <malloc.h>
#include <errno.h>
#include "FoundationKit/Platform/Platform.hpp"
#include "FoundationKit/Foundation/Logger.hpp"

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



std::string Platform::ExecuteSystemCommand(const std::string& command)
{
    std::string result = "";
    FILE* pipe = popen(command.c_str(), "r");
    if (!pipe)
    {
        LOG_ERROR("****** popen() failed!");
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
        LOG_ERROR("****** Cannot execute command:%s with errno:%d", command.c_str(), errno);
    }
    pclose(pipe);
    return result;
}


NS_FK_END

#endif //#ifdef ANDROID



