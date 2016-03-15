/****************************************************************************
  Copyright (c) 2015 libo All rights reserved.
 
  losemymind.libo@gmail.com

****************************************************************************/
#pragma once
#include "FoundationKit/FoundationKitDefines.h" // for NS_FK_BEGIN / NS_FK_END

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