#ifndef FoundationKit_thread_wp_H
#define FoundationKit_thread_wp_H

#if defined(WINAPI_FAMILY) && (WINAPI_FAMILY == WINAPI_FAMILY_PHONE_APP)

#include <windows.h>
#include <ostream>
#include <process.h>
#include <exception>
#include "thread.h"

namespace FoundationKit
{
	static VOID WINAPI Sleep(DWORD dwMilliseconds)
	{
		static HANDLE singletonEvent = nullptr;

		HANDLE sleepEvent = singletonEvent;

		// Demand create the event.
		if (!sleepEvent)
		{
			sleepEvent = CreateEventEx(nullptr, nullptr, CREATE_EVENT_MANUAL_RESET, EVENT_ALL_ACCESS);

			if (!sleepEvent)
				return;

			HANDLE previousEvent = InterlockedCompareExchangePointerRelease(&singletonEvent, sleepEvent, nullptr);

			if (previousEvent)
			{
				// Back out if multiple threads try to demand create at the same time.
				CloseHandle(sleepEvent);
				sleepEvent = previousEvent;
			}
		}

		// Emulate sleep by waiting with timeout on an event that is never signalled.
		WaitForSingleObjectEx(sleepEvent, dwMilliseconds, false);
	}


	class mutex {
	public:
		/// Constructor.
		mutex()
			: mAlreadyLocked(false)
		{
			InitializeCriticalSectionEx(&mHandle,0,0);
		}

		/// Destructor.
		~mutex()
		{
			DeleteCriticalSection(&mHandle);
		}

		/// Lock the mutex.
		/// The method will block the calling thread until a lock on the mutex can
		/// be obtained. The mutex remains locked until @c unlock() is called.
		/// @see lock_guard
		inline void lock()
		{
			EnterCriticalSection(&mHandle);
			while(mAlreadyLocked) Sleep(1000); // Simulate deadlock...
			mAlreadyLocked = true;
		}

		/// Try to lock the mutex.
		/// The method will try to lock the mutex. If it fails, the function will
		/// return immediately (non-blocking).
		/// @return @c true if the lock was acquired, or @c false if the lock could
		/// not be acquired.
		inline bool try_lock()
		{
			bool ret = (TryEnterCriticalSection(&mHandle) ? true : false);
			if(ret && mAlreadyLocked)
			{
				LeaveCriticalSection(&mHandle);
				ret = false;
			}
			return ret;
		}

		/// Unlock the mutex.
		/// If any threads are waiting for the lock on this mutex, one of them will
		/// be unblocked.
		inline void unlock()
		{
			mAlreadyLocked = false;
			LeaveCriticalSection(&mHandle);
		}

		DISABLE_ASSIGNMENT(mutex)
	private:
		CRITICAL_SECTION mHandle;
		bool mAlreadyLocked;
		friend class condition_variable;
	};


	/// Recursive mutex class.
	/// This is a mutual exclusion object for synchronizing access to shared
	/// memory areas for several threads. The mutex is recursive (i.e. a thread
	/// may lock the mutex several times, as long as it unlocks the mutex the same
	/// number of times).
	/// @see mutex
	class recursive_mutex {
	public:
		/// Constructor.
		recursive_mutex()
		{
			InitializeCriticalSectionEx(&mHandle,0,0);
		}

		/// Destructor.
		~recursive_mutex()
		{
			DeleteCriticalSection(&mHandle);
		}

		/// Lock the mutex.
		/// The method will block the calling thread until a lock on the mutex can
		/// be obtained. The mutex remains locked until @c unlock() is called.
		/// @see lock_guard
		inline void lock()
		{
			EnterCriticalSection(&mHandle);
		}

		/// Try to lock the mutex.
		/// The method will try to lock the mutex. If it fails, the function will
		/// return immediately (non-blocking).
		/// @return @c true if the lock was acquired, or @c false if the lock could
		/// not be acquired.
		inline bool try_lock()
		{
			return TryEnterCriticalSection(&mHandle) ? true : false;
		}

		/// Unlock the mutex.
		/// If any threads are waiting for the lock on this mutex, one of them will
		/// be unblocked.
		inline void unlock()
		{
			LeaveCriticalSection(&mHandle);
		}

		DISABLE_ASSIGNMENT(recursive_mutex)

	private:
		CRITICAL_SECTION mHandle;
		friend class condition_variable;
	};



	/// Condition variable class.
	/// This is a signalling object for synchronizing the execution flow for
	/// several threads. Example usage:
	/// @code
	/// // Shared data and associated mutex and condition variable objects
	/// int count;
	/// mutex m;
	/// condition_variable cond;
	///
	/// // Wait for the counter to reach a certain number
	/// void wait_counter(int targetCount)
	/// {
	///   lock_guard<mutex> guard(m);
	///   while(count < targetCount)
	///     cond.wait(m);
	/// }
	///
	/// // Increment the counter, and notify waiting threads
	/// void increment()
	/// {
	///   lock_guard<mutex> guard(m);
	///   ++ count;
	///   cond.notify_all();
	/// }
	/// @endcode
#define CONDITION_EVENT_ONE 0
#define CONDITION_EVENT_ALL 1
	class condition_variable {
	public:
		condition_variable()
		{
			mEvents[CONDITION_EVENT_ONE] = CreateEvent(NULL, FALSE, FALSE, NULL);
			mEvents[CONDITION_EVENT_ALL] = CreateEvent(NULL, TRUE, FALSE, NULL);
			InitializeCriticalSectionEx(&mWaitersCountLock, 0, 0);
		}
		~condition_variable()
		{
			CloseHandle(mEvents[CONDITION_EVENT_ONE]);
			CloseHandle(mEvents[CONDITION_EVENT_ALL]);
			DeleteCriticalSection(&mWaitersCountLock);
		}


		/// Wait for the condition.
		/// The function will block the calling thread until the condition variable
		/// is woken by @c notify_one(), @c notify_all() or a spurious wake up.
		/// @param[in] aMutex A mutex that will be unlocked when the wait operation
		/// starts, an locked again as soon as the wait operation is finished.
		template <class _mutexT>
		inline void wait(_mutexT &aMutex)
		{
			// Increment number of waiters
			EnterCriticalSection(&mWaitersCountLock);
			++ mWaitersCount;
			LeaveCriticalSection(&mWaitersCountLock);

			// Release the mutex while waiting for the condition (will decrease
			// the number of waiters when done)...
			aMutex.unlock();
			_wait();
			aMutex.lock();
		}

		/// Notify one thread that is waiting for the condition.
		/// If at least one thread is blocked waiting for this condition variable,
		/// one will be woken up.
		/// @note Only threads that started waiting prior to this call will be
		/// woken up.
		void notify_one()
		{
			// Are there any waiters?
			EnterCriticalSection(&mWaitersCountLock);
			bool haveWaiters = (mWaitersCount > 0);
			LeaveCriticalSection(&mWaitersCountLock);

			// If we have any waiting threads, send them a signal
			if(haveWaiters)
				SetEvent(mEvents[CONDITION_EVENT_ONE]);
		}

		/// Notify all threads that are waiting for the condition.
		/// All threads that are blocked waiting for this condition variable will
		/// be woken up.
		/// @note Only threads that started waiting prior to this call will be
		/// woken up.

		void notify_all()
		{
			// Are there any waiters?
			EnterCriticalSection(&mWaitersCountLock);
			bool haveWaiters = (mWaitersCount > 0);
			LeaveCriticalSection(&mWaitersCountLock);

			// If we have any waiting threads, send them a signal
			if(haveWaiters)
				SetEvent(mEvents[CONDITION_EVENT_ALL]);
		}
		DISABLE_ASSIGNMENT(condition_variable)

	private:
		void _wait()
		{
			// Wait for either event to become signaled due to notify_one() or
			// notify_all() being called
			int result = WaitForMultipleObjects(2, mEvents, FALSE, INFINITE);

			// Check if we are the last waiter
			EnterCriticalSection(&mWaitersCountLock);
			-- mWaitersCount;
			bool lastWaiter = (result == (WAIT_OBJECT_0 + CONDITION_EVENT_ALL)) &&
				(mWaitersCount == 0);
			LeaveCriticalSection(&mWaitersCountLock);

			// If we are the last waiter to be notified to stop waiting, reset the event
			if(lastWaiter)
				ResetEvent(mEvents[CONDITION_EVENT_ALL]);
		}

		HANDLE mEvents[2];                  ///< Signal and broadcast event HANDLEs.
		unsigned int mWaitersCount;         ///< Count of the number of waiters.
		CRITICAL_SECTION mWaitersCountLock; ///< Serialize access to mWaitersCount.

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
			// Serialize access to this thread structure
			lock_guard<mutex> guard(mDataMutex);

			// Fill out the thread startup information (passed to the thread wrapper,
			// which will eventually free it)
			thread_parameter * ti = new _thread_start_info;
			ti->_workfunction = aFunction;
			ti->_param = aArg;
			ti->_thread = this;

			// The thread is now alive
			mNotAThread = false;

			// Create the thread
			mHandle = (HANDLE) _beginthreadex(0, 0, wrapper_function, (void *) ti, 0, &mWin32ThreadID);

			// Did we fail to create the thread?
			if(!mHandle)
			{
				mNotAThread = true;
				delete ti;
			}
		}

		/// Destructor.
		/// @note If the thread is joinable upon destruction, @c std::terminate()
		/// will be called, which terminates the process. It is always wise to do
		/// @c join() before deleting a thread object.
		~thread()
		{
			if(joinable())
				std::terminate();
		}

		/// Wait for the thread to finish (join execution flows).
		/// After calling @c join(), the thread object is no longer associated with
		/// a thread of execution (i.e. it is not joinable, and you may not join
		/// with it nor detach from it).
		void join()
		{
			if(joinable())
			{
				WaitForSingleObject(mHandle, INFINITE);
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
		id get_id() const
		{
			if(!joinable())
				return id();
			return id((unsigned long int) mWin32ThreadID);
		}

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

		DISABLE_ASSIGNMENT(thread)

	private:
		native_handle_type mHandle;   ///< Thread handle.
		mutable mutex mDataMutex;     ///< Serializer for access to the thread private data.
		bool mNotAThread;             ///< True if this object is not a thread of execution.
		unsigned int mWin32ThreadID;  ///< Unique thread ID (filled out by _beginthreadex).

		// This is the internal thread wrapper function.
		static unsigned WINAPI wrapper_function(void * aArg)
		{
			// Get thread startup information
			thread_parameter * ti = (thread_parameter *) aArg;

			try
			{
				// Call the actual client thread function
				ti->_workfunction(ti->_param);
			}
			catch(...)
			{
				// Uncaught exceptions will terminate the application (default behavior
				// according to C++11)
				std::terminate();
			}

			// The thread is no longer executing
			lock_guard<mutex> guard(ti->_thread->mDataMutex);
			ti->_thread->mNotAThread = true;

			// The thread is responsible for freeing the startup information
			delete ti;

			return 0;
		}
	};


	/// The namespace @c this_thread provides methods for dealing with the
	/// calling thread.
	namespace this_thread {
		/// Return the thread ID of the calling thread.
		id get_id()
		{
			return id((unsigned long int) GetCurrentThreadId());
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
		template <class _Rep, class _Period> void sleep_for(const chrono::duration<_Rep, _Period>& aTime)
		{
			Sleep(int(double(aTime.count()) * (1000.0 * _Period::_as_double()) + 0.5));
		}
	}

} //namespace FoundationKit

#endif //#if defined(WINAPI_FAMILY) && (WINAPI_FAMILY == WINAPI_FAMILY_PHONE_APP)

#endif // FoundationKit_thread_wp_H