/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.
 
  losemymind.libo@gmail.com

****************************************************************************/
#ifndef FOUNDATIONKIT_THREAD_H
#define FOUNDATIONKIT_THREAD_H
#include <process.h>
#include <ostream>
#include "stdheader.h"

namespace std
{

class thread;
/// Information to pass to the new thread (what to run).
//struct _thread_start_info {
//    void (*mFunction)(void *); ///< Pointer to the function to be executed.
//    void * mArg;               ///< Function argument for the thread function.
//    thread * mThread;          ///< Pointer to the thread object.
//};

struct thread_argument_base
{
    virtual void call()=0;
    virtual~thread_argument_base(){};
    void * mArg;               ///< Function argument for the thread function.
    thread * mThread;          ///< Pointer to the thread object.
};

template<class _Fn>
struct thread_argument : thread_argument_base
{
    thread_argument(){}
    virtual void call()
    {
        fun(mArg);
    }

    _Fn fun;
};

/// Thread class.
class thread {
public:
    typedef HANDLE native_handle_type;
    class id;

    /// Default constructor.
    /// Construct a @c thread object without an associated thread of execution
    /// (i.e. non-joinable).
    thread()
        : mHandle(0)
        , mNotAThread(true)
        , mWin32ThreadID(0)
    {}

    /// Thread starting constructor.
    /// Construct a @c thread object with a new thread of execution.
    /// @param[in] aFunction A function pointer to a function of type:
    ///          <tt>void fun(void * arg)</tt>
    /// @param[in] aArg Argument to the thread function.
    /// @note This constructor is not fully compatible with the standard C++
    /// thread class. It is more similar to the pthread_create() (POSIX) and
    /// CreateThread() (Windows) functions.
    thread(void (*aFunction)(void *), void * aArg)
    {
        make_thread<std::function<void(void*)> >(std::function<void(void*)>(aFunction), aArg);

    }

    template<class _Fn>
    thread(const _Fn& func,void * aArg)
    {
        make_thread<_Fn>(func, aArg);
    }

    /// Destructor.
    /// @note If the thread is joinable upon destruction, @c std::terminate()
    /// will be called, which terminates the process. It is always wise to do
    /// @c join() before deleting a thread object.
    ~thread()
    {
        if(joinable())
            terminate();
    }

    /// Wait for the thread to finish (join execution flows).
    /// After calling @c join(), the thread object is no longer associated with
    /// a thread of execution (i.e. it is not joinable, and you may not join
    /// with it nor detach from it).
    void join()
    {
        if(joinable())
        {
            WaitForSingleObjectEx(mHandle, INFINITE, false);
            CloseHandle(mHandle);
        }
    }

    /// Check if the thread is joinable.
    /// A thread object is joinable if it has an associated thread of execution.
    bool joinable() const
    {
        mDataMutex.lock();
        bool result = !mNotAThread;
        mDataMutex.unlock();
        return result;
    }

    /// Detach from the thread.
    /// After calling @c detach(), the thread object is no longer assicated with
    /// a thread of execution (i.e. it is not joinable). The thread continues
    /// execution without the calling thread blocking, and when the thread
    /// ends execution, any owned resources are released.
    void detach()
    {
        mDataMutex.lock();
        if(!mNotAThread)
        {
            CloseHandle(mHandle);
            mNotAThread = true;
        }
        mDataMutex.unlock();
    }

    /// Return the thread ID of a thread object.
    id get_id() const;

    /// Get the native handle for this thread.
    /// @note Under Windows, this is a @c HANDLE, and under POSIX systems, this
    /// is a @c pthread_t.
    inline native_handle_type native_handle()
    {
        return mHandle;
    }

    /// Determine the number of threads which can possibly execute concurrently.
    /// This function is useful for determining the optimal number of threads to
    /// use for a task.
    /// @return The number of hardware thread contexts in the system.
    /// @note If this value is not defined, the function returns zero (0).
    static unsigned hardware_concurrency()
    {
        SYSTEM_INFO si;
        GetSystemInfo(&si);
        return (int) si.dwNumberOfProcessors;
    }
private:
    template<class _Fn>
    void make_thread(const _Fn& func, void* aArg)
    {
        // Serialize access to this thread structure
        lock_guard<mutex> guard(mDataMutex);
        // Fill out the thread startup information (passed to the thread wrapper,
        // which will eventually free it)
        thread_argument<_Fn> * tab = new thread_argument<_Fn>();;
        tab->fun = func;
        tab->mArg = aArg;
        tab->mThread = this;
        // The thread is now alive
        mNotAThread = false;
        // Create the thread
        mHandle = (HANDLE) _beginthreadex(0, 0, wrapper_function, (void *) tab, 0, &mWin32ThreadID);
        // Did we fail to create the thread?
        if(!mHandle)
        {
            mNotAThread = true;
            delete tab;
        }
    }

    static unsigned WINAPI wrapper_function(void * aArg)
    {
        // Get thread startup information
        thread_argument_base * tab = (thread_argument_base*) aArg;
        try
        {
            // Call the actual client thread function
            tab->call();
        }
        catch(...)
        {
            delete tab;
            // Uncaught exceptions will terminate the application (default behavior
            // according to C++11)
            terminate();
        }
        // The thread is no longer executing
        lock_guard<mutex> guard(tab->mThread->mDataMutex);
        tab->mThread->mNotAThread = true;
        // The thread is responsible for freeing the startup information
        delete tab;
        return 0;
    }
private:
    thread(const thread&); 
    thread& operator=(const thread&);
    native_handle_type mHandle;   ///< Thread handle.
    mutable mutex mDataMutex;     ///< Serializer for access to the thread private data.
    bool mNotAThread;             ///< True if this object is not a thread of execution.
    unsigned int mWin32ThreadID;  ///< Unique thread ID (filled out by _beginthreadex).
};


/// Thread ID.
/// The thread ID is a unique identifier for each thread.
/// @see thread::get_id()
class thread::id {
public:
    /// Default constructor.
    /// The default constructed ID is that of thread without a thread of
    /// execution.
    id() : mId(0) {};

    id(unsigned long int aId) : mId(aId) {};

    id(const id& aId) : mId(aId.mId) {};

    inline id & operator=(const id &aId)
    {
        mId = aId.mId;
        return *this;
    }

    inline friend bool operator==(const id &aId1, const id &aId2)
    {
        return (aId1.mId == aId2.mId);
    }

    inline friend bool operator!=(const id &aId1, const id &aId2)
    {
        return (aId1.mId != aId2.mId);
    }

    inline friend bool operator<=(const id &aId1, const id &aId2)
    {
        return (aId1.mId <= aId2.mId);
    }

    inline friend bool operator<(const id &aId1, const id &aId2)
    {
        return (aId1.mId < aId2.mId);
    }

    inline friend bool operator>=(const id &aId1, const id &aId2)
    {
        return (aId1.mId >= aId2.mId);
    }

    inline friend bool operator>(const id &aId1, const id &aId2)
    {
        return (aId1.mId > aId2.mId);
    }

    inline friend ostream& operator <<(ostream &os, const id &obj)
    {
        os << obj.mId;
        return os;
    }

private:
    unsigned long int mId;
};

inline thread::id thread::get_id() const
{
    if(!joinable())
        return id();
    return id((unsigned long int) mWin32ThreadID);
}

namespace this_thread {
    /// Return the thread ID of the calling thread.
    inline thread::id get_id()
    {
        return thread::id((unsigned long int) GetCurrentThreadId());
    }
    /// Yield execution to another thread.
    /// Offers the operating system the opportunity to schedule another thread
    /// that is ready to run on the current processor.
    inline void yield()
    {
        Sleep(0);
    }

    /// Blocks the calling thread for a period of time.
    /// @param[in] aTime Minimum time to put the thread to sleep.
    /// Example usage:
    /// @code
    /// // Sleep for 100 milliseconds
    /// this_thread::sleep_for(chrono::milliseconds(100));
    /// @endcode
    /// @note Supported duration types are: nanoseconds, microseconds,
    /// milliseconds, seconds, minutes and hours.
    template <class _Rep, class _Period> 
    void sleep_for(const chrono::duration<_Rep, _Period>& aTime)
    {
        Sleep(int(double(aTime.count()) * (1000.0 * _Period::_as_double()) + 0.5));
    }
}

} //namespace std
#endif // FOUNDATIONKIT_THREAD_H