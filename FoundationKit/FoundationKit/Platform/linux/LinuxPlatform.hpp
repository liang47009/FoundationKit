/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.
 
  losemymind.libo@gmail.com

****************************************************************************/
#ifndef _SFI_59DB4CEF-F7CA-45A2-A385-A7567F69206A__LINUXPLATFORM_HPP
#define _SFI_59DB4CEF-F7CA-45A2-A385-A7567F69206A__LINUXPLATFORM_HPP

#define DEPRECATED(VERSION, MESSAGE) __attribute__((deprecated(MESSAGE " Please update your code to the new API before upgrading to the next release, otherwise your project will no longer compile.")))
// Symbol export and import definitions
#define DLL_EXPORT			__attribute__((visibility("default")))
#define DLL_IMPORT			__attribute__((visibility("default")))
// Alignment.
#define ATTRIBUTE_PACK(n) __attribute__((packed,aligned(n)))
#define ATTRIBUTE_ALIGN(n) __attribute__((aligned(n)))
#define ATTRIBUTE_UNUSED __attribute__((unused))//__attribute__((__unused__)) ?
#define ATTRIBUTE_USED __attribute__((used))
#define FORCEINLINE inline __attribute__ ((always_inline)) /* Force code to be inline */
#define FORCENOINLINE __attribute__((noinline))	           /* Force code to NOT be inline */
#define THREAD_LOCAL __thread
#define FILEPATH_MAX PATH_MAX

#endif // END OF _SFI_59DB4CEF-F7CA-45A2-A385-A7567F69206A__LINUXPLATFORM_HPP