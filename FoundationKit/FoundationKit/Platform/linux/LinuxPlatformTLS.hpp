/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.
 
  losemymind.libo@gmail.com

****************************************************************************/
#ifndef FOUNDATIONKIT_LINUXPLATFORMTLS_HPP
#define FOUNDATIONKIT_LINUXPLATFORMTLS_HPP

#include "FoundationKit/GenericPlatformMacros.hpp"
#if (PLATFORM_LINUX)

#include "FoundationKit/Base/types.hpp"
#include <pthread.h>
NS_FK_BEGIN

#define TLS_THREAD_CACHE 0

/**
 * Linux implementation of the TLS OS functions
 **/
class LinuxPlatformTLS
{
public:
    /**
     * Return false if this is an invalid TLS slot
     * @param SlotIndex the TLS index to check
     * @return true if this looks like a valid slot
     */
    static FORCEINLINE bool IsValidTlsSlot(uint32 SlotIndex)
    {
        return SlotIndex != 0xFFFFFFFF;
    } 

	/**
	 * Returns the currently executing thread's id
	 */
	static FORCEINLINE uint32 GetCurrentThreadId(void)
	{
        // note: cannot use pthread_self() without updating the rest of API to opaque (or at least 64-bit) thread handles
#if defined(_GNU_SOURCE)

    #if TLS_THREAD_CACHE
        // syscall() is relatively heavy and shows up in the profiler, Cache thread id in TLS.
        static __thread uint32 ThreadIdTLS = 0;
        if (ThreadIdTLS == 0)
        {
    #else
        uint32 ThreadIdTLS;
        {
    #endif // TLS_THREAD_CACHE
            pid_t ThreadId = static_cast<pid_t>(syscall(SYS_gettid));
            static_assert(sizeof(pid_t) <= sizeof(uint32), "pid_t is larger than uint32, reconsider implementation of GetCurrentThreadId()");
            ThreadIdTLS = static_cast<uint32>(ThreadId);
            ASSERTED(ThreadIdTLS != 0, TEXT("ThreadId is 0 - reconsider implementation of GetCurrentThreadId() (syscall changed?)"));
        }
        return ThreadIdTLS;

#else
        // better than nothing...
        static_assert(sizeof(uint32) == sizeof(pthread_t), "pthread_t cannot be converted to uint32 one to one - different number of bits. Review FLinuxTLS::GetCurrentThreadId() implementation.");
        return static_cast<uint32>(pthread_self());
#endif
	}

	/**
	 * Allocates a thread local store slot
	 */
	static FORCEINLINE uint32 AllocTlsSlot(void)
	{
        // allocate a per-thread mem slot
        pthread_key_t Key = 0;
        if (pthread_key_create(&Key, nullptr) != 0)
        {
            return static_cast<uint32>(INDEX_NONE); // matches the Windows TlsAlloc() retval.
        }

        // pthreads can return an arbitrary key, yet we reserve INDEX_NONE as an invalid one. Handle this very unlikely case
        // by allocating another one first (so we get another value) and releasing existing key.
        if (static_cast<uint32>(Key) == static_cast<uint32>(INDEX_NONE))
        {
            pthread_key_t NewKey = 0;
            int SecondKeyAllocResult = pthread_key_create(&NewKey, nullptr);
            // discard the previous one
            pthread_key_delete((pthread_key_t)Key);
            if (SecondKeyAllocResult != 0)
            {
                // could not alloc the second key, treat this as an error
                return static_cast<uint32>(INDEX_NONE); // matches the Windows TlsAlloc() retval.
            }
            // check that we indeed got something different
            ASSERTED(NewKey != static_cast<uint32>(INDEX_NONE), TEXT("Could not allocate a usable TLS slot id."));
            Key = NewKey;
        }
        return Key;
	}

	/**
	 * Sets a value in the specified TLS slot
	 *
	 * @param SlotIndex the TLS index to store it in
	 * @param Value the value to store in the slot
	 */
	static FORCEINLINE void SetTlsValue(uint32 SlotIndex,void* Value)
	{
		pthread_setspecific((pthread_key_t)SlotIndex, Value);
	}

	/**
	 * Reads the value stored at the specified TLS slot
	 *
	 * @return the value stored in the slot
	 */
	static FORCEINLINE void* GetTlsValue(uint32 SlotIndex)
	{
		return pthread_getspecific((pthread_key_t)SlotIndex);
	}

	/**
	 * Frees a previously allocated TLS slot
	 *
	 * @param SlotIndex the TLS index to store it in
	 */
	static FORCEINLINE void FreeTlsSlot(uint32 SlotIndex)
	{
		pthread_key_delete((pthread_key_t)SlotIndex);
	}
};

typedef LinuxPlatformTLS PlatformTLS;

NS_FK_END

#endif //OF #if (PLATFORM_LINUX)


#endif // END OF FOUNDATIONKIT_LINUXPLATFORMTLS_HPP
