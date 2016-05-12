
#include "FoundationKit/Foundation/Logger.h"
#include "FoundationKit/experimental/memory.h"

USING_NS_FK;

void justForLoadSym(){}

#if TARGET_PLATFORM == PLATFORM_WIN32

typedef int(__cdecl *_PVFV)();
#define INIRETSUCCESS 0
#define PVAPI int __cdecl

static PVAPI on_process_init()
{
    return INIRETSUCCESS;
}
static PVAPI on_process_term()
{
    return INIRETSUCCESS;
}

#pragma section(".CRT$XCU",read)
__declspec(allocate(".CRT$XCU")) _PVFV p_process_init = on_process_init;

#pragma section(".CRT$XTU",long,read)
__declspec(allocate(".CRT$XTU"))_PVFV p_process_term = on_process_term;


/*

#elif (TARGET_PLATFORM == PLATFORM_ANDROID) || (TARGET_PLATFORM == PLATFORM_LINUX)

// call this before main function.
static void __attribute__((constructor)) installMemoryHooks(void)
{

}

// call this after main function.
static  __attribute__((destructor)) void uninstallMemoryHooks()
{
    
}

#elif (TARGET_PLATFORM == PLATFORM_IOS) || (TARGET_PLATFORM == PLATFORM_MAC)

// call this before main function.
static void __attribute__((constructor)) installMemoryHooks(void)
{

}

// call this after main function.
static  __attribute__((destructor)) void uninstallMemoryHooks()
{

}

static void * (*real_malloc)  (malloc_zone_t *, size_t);
static void * (*real_calloc)  (malloc_zone_t *, size_t, size_t);
static void * (*real_valloc)  (malloc_zone_t *, size_t);
static void * (*real_realloc) (malloc_zone_t *, void *, size_t);
static void   (*real_free)    (malloc_zone_t *, void *);

malloc_zone_t* default_zone = malloc_default_zone();
if (!default_zone) return false;
vm_protect(mach_task_self(), (uintptr_t)pMallocZone, sizeof(malloc_zone_t), 0, VM_PROT_READ | VM_PROT_WRITE);//remove the write protection
real_malloc  = default_zone->malloc;
real_calloc  = default_zone->calloc;
real_valloc  = default_zone->valloc;
real_realloc = default_zone->realloc;
real_free = default_zone->free;
vm_protect(mach_task_self(), (uintptr_t)pMallocZone, sizeof(malloc_zone_t), 0, VM_PROT_READ);//put the write protection back
*/
#endif





