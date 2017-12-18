/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.

  losemymind.libo@gmail.com

****************************************************************************/
/*=============================================================================================
	ApplePlatformTLS.hpp: Apple platform TLS (Thread local storage and thread ID) functions
==============================================================================================*/
#ifndef FOUNDATIONKIT_APPLEPLATFORMTLS_HPP
#define FOUNDATIONKIT_APPLEPLATFORMTLS_HPP

#include "FoundationKit/GenericPlatformMacros.hpp"
#if (PLATFORM_IOS || PLATFORM_MAC)

#include "FoundationKit/Base/types.hpp"
#include <pthread.h>
NS_FK_BEGIN

/**
 * Apple implementation of the TLS OS functions
 **/
class ApplePlatformTLS
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
		return (uint32)pthread_mach_thread_np(pthread_self());
	}

	/**
	 * Allocates a thread local store slot
	 */
	static FORCEINLINE uint32 AllocTlsSlot(void)
	{
		// allocate a per-thread mem slot
		pthread_key_t SlotKey = 0;
		if (pthread_key_create(&SlotKey, NULL) != 0)
		{
			SlotKey = 0xFFFFFFFF;  // matches the Windows TlsAlloc() retval.
		}
		return (uint32)SlotKey;
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

typedef ApplePlatformTLS PlatformTLS;

NS_FK_END

#endif //OF #if (PLATFORM_IOS || PLATFORM_MAC)

#endif // FOUNDATIONKIT_APPLEPLATFORMTLS_HPP

