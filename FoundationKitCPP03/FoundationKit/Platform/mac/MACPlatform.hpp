/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.
 
  losemymind.libo@gmail.com

****************************************************************************/
#ifndef FOUNDATIONKIT_MACPLATFORM_HPP
#define FOUNDATIONKIT_MACPLATFORM_HPP
#if defined(macintosh) || defined(__APPLE__) || defined(__APPLE_CC__)
// TARGET_OS_IPHONE inlcudes TARGET_OS_IOS TARGET_OS_TV and TARGET_OS_WATCH. see TargetConditionals.h
#include <TargetConditionals.h>
#include <AvailabilityMacros.h>
#endif
#if (TARGET_OS_MAC)
/**
 * Macro for marking up deprecated code, functions and types.
 *
 * Features that are marked as deprecated are scheduled to be removed from the code base
 * in a future release. If you are using a deprecated feature in your code, you should
 * replace it before upgrading to the next release. See the Upgrade Notes in the release
 * notes for the release in which the feature was marked deprecated.
 *
 * Sample usage (note the slightly different syntax for classes and structures):
 *
 *		DEPRECATED(4.6, "Message")
 *		void Function();
 *
 *		struct DEPRECATED(4.6, "Message") MODULE_API MyStruct
 *		{
 *			// StructImplementation
 *		};
 *		class DEPRECATED(4.6, "Message") MODULE_API MyClass
 *		{
 *			// ClassImplementation
 *		};
 *
 * @param VERSION The release number in which the feature was marked deprecated.
 * @param MESSAGE A message containing upgrade notes.
 */
#define DEPRECATED(VERSION, MESSAGE) __attribute__((deprecated(MESSAGE " Please update your code to the new API before upgrading to the next release, otherwise your project will no longer compile.")))
// Symbol export and import definitions
#define DLL_EXPORT
#define DLL_IMPORT
// Alignment.
#define ATTRIBUTE_PACK(n) __attribute__((packed,aligned(n)))
#define ATTRIBUTE_ALIGN(n) __attribute__((aligned(n)))
#define ATTRIBUTE_UNUSED __attribute__((unused)) //__attribute__((__unused__)) ?
#define ATTRIBUTE_USED __attribute__((used))
#define FORCEINLINE inline __attribute__ ((always_inline))  /* Force code to be inline */
#define FORCENOINLINE __attribute__((noinline))	            /* Force code to NOT be inline */
#define THREAD_LOCAL __thread
#define FILEPATH_MAX 1024
#endif // END OF TARGET_OS_MAC
#endif // END OF FOUNDATIONKIT_MACPLATFORM_HPP