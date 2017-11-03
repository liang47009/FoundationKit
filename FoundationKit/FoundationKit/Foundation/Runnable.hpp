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
#include "FoundationKit/Base/types.hpp"

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
    * Initializes the runnable object.
    *
    * This method is called in the context of the thread object that aggregates this, not the
    * thread that passes this runnable to a new thread.
    *
    * @return True if initialization was successful, false otherwise
    * @see Run, Stop, Exit
    */
    virtual bool Init()
    {
        return true;
    }

   /**
    * Runs the runnable object.
    *
    * This is where all per object thread work is done. This is only called if the initialization was successful.
    *
    * @return The exit code of the runnable object
    * @see Init, Stop, Exit
    */
    virtual uint32 Run() = 0;

   /**
    * Stops the runnable object.
    *
    * This is called if a thread is requested to terminate early.
    * @see Init, Run, Exit
    */
    virtual void Stop() { }

   /**
    * Exits the runnable object.
    *
    * Called in the context of the aggregating thread to perform any cleanup.
    * @see Init, Run, Stop
    */
    virtual void Exit() { }
};

NS_FK_END


#endif // FOUNDATIONKIT_RUNNABLE_HPP





