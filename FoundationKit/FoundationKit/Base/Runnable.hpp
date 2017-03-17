/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.
 
  losemymind.libo@gmail.com

****************************************************************************/
#ifndef FOUNDATIONKIT_RUNNABLE_HPP
#define FOUNDATIONKIT_RUNNABLE_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include "FoundationKit/GenericPlatformMacros.hpp" // for NS_FK_BEGIN / NS_FK_END

NS_FK_BEGIN

/** 
 * The Runnable interface with the run() method
 * must be implemented by classes that provide
 * an entry point for a thread.
 */
class Runnable
{
public:	
	Runnable(){};
	virtual ~Runnable(){};

	/** 
	 * Do whatever the thread needs to do. Must
	 * be overridden by subclasses.
	 */
	virtual void run() = 0;
};

NS_FK_END


#endif // FOUNDATIONKIT_RUNNABLE_HPP
