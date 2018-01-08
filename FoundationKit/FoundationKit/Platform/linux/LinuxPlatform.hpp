/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.
 
  losemymind.libo@gmail.com

****************************************************************************/
#ifndef FOUNDATIONKIT_LINUXPLATFORM_HPP
#define FOUNDATIONKIT_LINUXPLATFORM_HPP
#if (defined(linux) || defined(__linux) || defined(__linux__) || defined(__GNU__) || defined(__GLIBC__)) && !defined(_CRAYC)

#define DEPRECATED(VERSION, MESSAGE) __attribute__((deprecated(MESSAGE " Please update your code to the new API before upgrading to the next release, otherwise your project will no longer compile.")))
// Symbol export and import definitions
#define DLL_EXPORT			__attribute__((visibility("default")))
#define DLL_IMPORT			__attribute__((visibility("default")))
// Alignment.
#define ATTRIBUTE_PACK(n) __attribute__((packed,aligned(n)))
#define ATTRIBUTE_ALIGN(n) __attribute__((aligned(n)))
#define ATTRIBUTE_UNUSED __attribute__((unused))//__attribute__((__unused__)) ?
#define ATTRIBUTE_USED __attribute__((used))
#define ATTRIBUTE_NORETURN __attribute__ ((noreturn))	   /* Indicate that the function never returns. */
#define FORCEINLINE inline __attribute__ ((always_inline)) /* Force code to be inline */
#define FORCENOINLINE __attribute__((noinline))	           /* Force code to NOT be inline */
#define THREAD_LOCAL __thread
#define FILEPATH_MAX PATH_MAX
#endif // PLATFORM_LINUX
#endif // END OF FOUNDATIONKIT_LINUXPLATFORM_HPP
