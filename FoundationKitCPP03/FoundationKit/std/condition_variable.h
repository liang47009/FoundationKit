/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.
 
  losemymind.libo@gmail.com

****************************************************************************/
#ifndef FOUNDATIONKIT_CONDITION_VARIABLE_H
#define FOUNDATIONKIT_CONDITION_VARIABLE_H

#include "stddefine.h"
#include "mutex.h"
namespace std
{

class condition_variable 
{
public:

    condition_variable::condition_variable() : mWaitersCount(0)
    {
        mEvents[_CONDITION_EVENT_ONE] = CreateEvent(NULL, FALSE, FALSE, NULL);
        mEvents[_CONDITION_EVENT_ALL] = CreateEvent(NULL, TRUE, FALSE, NULL);
        InitializeCriticalSection(&mWaitersCountLock);
    }

    ~condition_variable()
    {
        CloseHandle(mEvents[_CONDITION_EVENT_ONE]);
        CloseHandle(mEvents[_CONDITION_EVENT_ALL]);
        DeleteCriticalSection(&mWaitersCountLock);
    }

    void wait(unique_lock<mutex>& _Lck)
    {	        // Increment number of waiters
        EnterCriticalSection(&mWaitersCountLock);
        ++ mWaitersCount;
        LeaveCriticalSection(&mWaitersCountLock);

        // Release the mutex while waiting for the condition (will decrease
        // the number of waiters when done)...
        _Lck.unlock();
        _wait();
        _Lck.lock();
    }

    template<class _Predicate>
    void wait(unique_lock<mutex>& _Lck, _Predicate _Pred)
    {	// wait for signal and test predicate
        while (!_Pred())
            wait(_Lck);
    }


    void notify_one()
    {
        // Are there any waiters?
        EnterCriticalSection(&mWaitersCountLock);
        bool haveWaiters = (mWaitersCount > 0);
        LeaveCriticalSection(&mWaitersCountLock);

        // If we have any waiting threads, send them a signal
        if(haveWaiters)
            SetEvent(mEvents[_CONDITION_EVENT_ONE]);
    }
    void notify_all()
    {
        // Are there any waiters?
        EnterCriticalSection(&mWaitersCountLock);
        bool haveWaiters = (mWaitersCount > 0);
        LeaveCriticalSection(&mWaitersCountLock);

        // If we have any waiting threads, send them a signal
        if(haveWaiters)
            SetEvent(mEvents[_CONDITION_EVENT_ALL]);
    }

private:
    void _wait()
    {
        // Wait for either event to become signaled due to notify_one() or
        // notify_all() being called
        int result = WaitForMultipleObjectsEx(2, mEvents, FALSE, INFINITE, false);

        // Check if we are the last waiter
        EnterCriticalSection(&mWaitersCountLock);
        -- mWaitersCount;
        bool lastWaiter = (result == (WAIT_OBJECT_0 + _CONDITION_EVENT_ALL)) &&
            (mWaitersCount == 0);
        LeaveCriticalSection(&mWaitersCountLock);

        // If we are the last waiter to be notified to stop waiting, reset the event
        if(lastWaiter)
            ResetEvent(mEvents[_CONDITION_EVENT_ALL]);
    }

private:
    condition_variable(const condition_variable&); 
    condition_variable& operator=(const condition_variable&);
    HANDLE mEvents[2];                  ///< Signal and broadcast event HANDLEs.
    unsigned int mWaitersCount;         ///< Count of the number of waiters.
    CRITICAL_SECTION mWaitersCountLock; ///< Serialize access to mWaitersCount.
    static const int _CONDITION_EVENT_ONE = 0;
    static const int _CONDITION_EVENT_ALL = 1;
};
} //namespace std
#endif // FOUNDATIONKIT_CONDITION_VARIABLE_H