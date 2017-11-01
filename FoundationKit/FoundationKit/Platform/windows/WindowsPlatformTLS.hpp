/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.
 
  losemymind.libo@gmail.com

****************************************************************************/
#ifndef FOUNDATIONKIT_WINDOWSPLATFORMTLS_HPP
#define FOUNDATIONKIT_WINDOWSPLATFORMTLS_HPP

#include "FoundationKit/GenericPlatformMacros.hpp"
#if PLATFORM_WINDOWS

#include "FoundationKit/Base/types.hpp"

NS_FK_BEGIN

/**
 * Windows implementation of the TLS OS functions.
 */
class WindowsPlatformTLS
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
	 * Returns the currently executing thread's identifier.
	 *
	 * @return The thread identifier.
	 */
	static FORCEINLINE uint32 GetCurrentThreadId(void)
	{
		return ::GetCurrentThreadId();
	}

	/**
	 * Allocates a thread local store slot.
	 *
	 * @return The index of the allocated slot.
	 */
	static FORCEINLINE uint32 AllocTlsSlot(void)
	{
		static_assert(static_cast<uint32>(-1) == TLS_OUT_OF_INDEXES, "TLS_OUT_OF_INDEXES is different from INDEX_NONE, change FWindowsPlatformTLS::AllocTlsSlot() implementation.");
		return ::TlsAlloc();
	}

	/**
	 * Sets a value in the specified TLS slot.
	 *
	 * @param SlotIndex the TLS index to store it in.
	 * @param Value the value to store in the slot.
	 */
	static FORCEINLINE void SetTlsValue(uint32 SlotIndex,void* Value)
	{
		::TlsSetValue(SlotIndex,Value);
	}

	/**
	 * Reads the value stored at the specified TLS slot.
	 *
	 * @param SlotIndex The index of the slot to read.
	 * @return The value stored in the slot.
	 */
	static FORCEINLINE void* GetTlsValue(uint32 SlotIndex)
	{
		return ::TlsGetValue(SlotIndex);
	}

	/**
	 * Frees a previously allocated TLS slot
	 *
	 * @param SlotIndex the TLS index to store it in
	 */
	static FORCEINLINE void FreeTlsSlot(uint32 SlotIndex)
	{
		::TlsFree(SlotIndex);
	}
};


typedef WindowsPlatformTLS PlatformTLS;

NS_FK_END

#endif // OF #if PLATFORM_WINDOWS

#endif // FOUNDATIONKIT_WINDOWSPLATFORMTLS_HPP
