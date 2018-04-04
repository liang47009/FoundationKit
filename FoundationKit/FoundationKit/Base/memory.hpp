/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.
 
  losemymind.libo@gmail.com

****************************************************************************/
#ifndef FOUNDATIONKIT_MEMORY_HPP
#define FOUNDATIONKIT_MEMORY_HPP

/**
* Override new + delete operators (and array versions) in every module.
* This prevents the possibility of mismatched new/delete calls such as a new[] that
* uses allocator and a delete[] that uses the system allocator.
*/
#define REPLACEMENT_OPERATOR_NEW_AND_DELETE \
	void* operator new  ( size_t Size                        )              { return FMemory::Malloc( Size ); } \
	void* operator new[]( size_t Size                        )              { return FMemory::Malloc( Size ); } \
	void* operator new  ( size_t Size, const std::nothrow_t& )              { return FMemory::Malloc( Size ); } \
	void* operator new[]( size_t Size, const std::nothrow_t& )              { return FMemory::Malloc( Size ); } \
	void operator delete  ( void* Ptr )                                     { FMemory::Free( Ptr ); } \
	void operator delete[]( void* Ptr )                                     { FMemory::Free( Ptr ); } \
	void operator delete  ( void* Ptr, const std::nothrow_t& )              { FMemory::Free( Ptr ); } \
	void operator delete[]( void* Ptr, const std::nothrow_t& )              { FMemory::Free( Ptr ); } \
	void operator delete  ( void* Ptr, size_t Size )                        { FMemory::Free( Ptr ); } \
	void operator delete[]( void* Ptr, size_t Size )                        { FMemory::Free( Ptr ); } \
	void operator delete  ( void* Ptr, size_t Size, const std::nothrow_t& ) { FMemory::Free( Ptr ); } \
	void operator delete[]( void* Ptr, size_t Size, const std::nothrow_t& ) { FMemory::Free( Ptr ); }

#endif // END OF FOUNDATIONKIT_MEMORY_HPP