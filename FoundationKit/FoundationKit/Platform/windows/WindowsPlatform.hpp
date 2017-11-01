/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.
 
  losemymind.libo@gmail.com

****************************************************************************/
#ifndef FOUNDATIONKIT_WINDOWSPLATFORM_HPP
#define FOUNDATIONKIT_WINDOWSPLATFORM_HPP
#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32) || defined(WIN64) || defined(_WIN64)

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
#define DEPRECATED(VERSION, MESSAGE) __declspec(deprecated(MESSAGE " Please update your code to the new API before upgrading to the next release, otherwise your project will no longer compile."))
// Symbol export and import definitions
#define DLL_EXPORT __declspec(dllexport)
#define DLL_IMPORT __declspec(dllimport)
// Alignment.
#define ATTRIBUTE_PACK(n)
#define ATTRIBUTE_ALIGN(n) 
#define MS_ALIGN(n) __declspec(align(n))
#define ATTRIBUTE_UNUSED
#define ATTRIBUTE_USED 
#define FORCEINLINE __forceinline				  /* Force code to be inline */
#define FORCENOINLINE __declspec(noinline)        /* Force code to NOT be inline */	
#define THREAD_LOCAL __declspec(thread)
#define FILEPATH_MAX MAX_PATH

#endif // END OF defined(_WIN32) || defined(__WIN32__) || defined(WIN32) || defined(WIN64) || defined(_WIN64)
#endif // END OF FOUNDATIONKIT_WINDOWSPLATFORM_HPP