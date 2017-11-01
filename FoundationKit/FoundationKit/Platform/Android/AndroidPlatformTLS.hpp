/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.
 
  losemymind.libo@gmail.com

****************************************************************************/
#ifndef FOUNDATIONKIT_ANDROIDPLATFORMTLS_HPP
#define FOUNDATIONKIT_ANDROIDPLATFORMTLS_HPP

#include "FoundationKit/GenericPlatformMacros.hpp"
#if PLATFORM_ANDROID

/*=============================================================================================
	AndroidTLS.h: Android platform TLS (Thread local storage and thread ID) functions
==============================================================================================*/

#include "FoundationKit/Base/Types.hpp"
#include <pthread.h>
#include <fcntl.h> // for gettid
NS_FK_BEGIN

/**
 * Android implementation of the TLS OS functions
 */
class AndroidTLS
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
		static_assert(sizeof(pid_t) <= sizeof(uint32), "pid_t is larger than uint32, reconsider implementation of GetCurrentThreadId()");
		return static_cast<uint32>(gettid());
//		return pthread_self();
	}

	/**
	 * Allocates a thread local store slot
	 */
	static FORCEINLINE uint32 AllocTlsSlot(void)
	{
		// allocate a per-thread mem slot
		pthread_key_t Key = 0;
		if (pthread_key_create(&Key, NULL) != 0)
		{
			Key = 0xFFFFFFFF;  // matches the Windows TlsAlloc() retval //@todo android: should probably check for this below, or assert out instead
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

typedef AndroidTLS PlatformTLS;

NS_FK_END

#endif //#if PLATFORM_ANDROID

#endif // FOUNDATIONKIT_ANDROIDPLATFORMTLS_HPP
