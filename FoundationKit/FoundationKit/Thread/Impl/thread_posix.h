#ifndef FoundationKit_thread_posix_H
#define FoundationKit_thread_posix_H

#if defined(ANDROID) || defined(IOS)

#include <pthread.h>
#include <signal.h>
#include <sched.h>
#include <unistd.h>
#include <ostream>
#include <unistd.h>
#include <map>

namespace FoundationKit
{

class mutex {
public:
	/// Constructor.
	mutex()
	{
		pthread_mutex_init(&mHandle, NULL);
	}

	/// Destructor.
	~mutex()
	{
		pthread_mutex_destroy(&mHandle);
	}

	/// Lock the mutex.
	/// The method will block the calling thread until a lock on the mutex can
	/// be obtained. The mutex remains locked until @c unlock() is called.
	/// @see lock_guard
	inline void lock()
	{
		pthread_mutex_lock(&mHandle);
	}

	/// Try to lock the mutex.
	/// The method will try to lock the mutex. If it fails, the function will
	/// return immediately (non-blocking).
	/// @return @c true if the lock was acquired, or @c false if the lock could
	/// not be acquired.
	inline bool try_lock()
	{
		return (pthread_mutex_trylock(&mHandle) == 0) ? true : false;
	}

	/// Unlock the mutex.
	/// If any threads are waiting for the lock on this mutex, one of them will
	/// be unblocked.
	inline void unlock()
	{
		pthread_mutex_unlock(&mHandle);
	}

	DISABLE_ASSIGNMENT(mutex)

private:
	pthread_mutex_t mHandle;

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
		pthread_mutexattr_t attr;
		pthread_mutexattr_init(&attr);
		pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
		pthread_mutex_init(&mHandle, &attr);
	}

	/// Destructor.
	~recursive_mutex()
	{
		pthread_mutex_destroy(&mHandle);
	}

	/// Lock the mutex.
	/// The method will block the calling thread until a lock on the mutex can
	/// be obtained. The mutex remains locked until @c unlock() is called.
	/// @see lock_guard
	inline void lock()
	{
#if defined(_TTHREAD_WIN32_)
		EnterCriticalSection(&mHandle);
#else
		pthread_mutex_lock(&mHandle);
#endif
	}

	/// Try to lock the mutex.
	/// The method will try to lock the mutex. If it fails, the function will
	/// return immediately (non-blocking).
	/// @return @c true if the lock was acquired, or @c false if the lock could
	/// not be acquired.
	inline bool try_lock()
	{
#if defined(_TTHREAD_WIN32_)
		return TryEnterCriticalSection(&mHandle) ? true : false;
#else
		return (pthread_mutex_trylock(&mHandle) == 0) ? true : false;
#endif
	}

	/// Unlock the mutex.
	/// If any threads are waiting for the lock on this mutex, one of them will
	/// be unblocked.
	inline void unlock()
	{
#if defined(_TTHREAD_WIN32_)
		LeaveCriticalSection(&mHandle);
#else
		pthread_mutex_unlock(&mHandle);
#endif
	}

	_TTHREAD_DISABLE_ASSIGNMENT(recursive_mutex)

private:
#if defined(_TTHREAD_WIN32_)
	CRITICAL_SECTION mHandle;
#else
	pthread_mutex_t mHandle;
#endif

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
class condition_variable {
public:
	/// Constructor.
	condition_variable()
	{
		pthread_cond_init(&mHandle, NULL);
	}

	/// Destructor.
	~condition_variable()
	{
		pthread_cond_destroy(&mHandle);
	}


	/// Wait for the condition.
	/// The function will block the calling thread until the condition variable
	/// is woken by @c notify_one(), @c notify_all() or a spurious wake up.
	/// @param[in] aMutex A mutex that will be unlocked when the wait operation
	///   starts, an locked again as soon as the wait operation is finished.
	template <class _mutexT>
	inline void wait(_mutexT &aMutex)
	{
		pthread_cond_wait(&mHandle, &aMutex.mHandle);
	}

	/// Notify one thread that is waiting for the condition.
	/// If at least one thread is blocked waiting for this condition variable,
	/// one will be woken up.
	/// @note Only threads that started waiting prior to this call will be
	/// woken up.
	inline void notify_one()
	{
		pthread_cond_signal(&mHandle);
	}


	/// Notify all threads that are waiting for the condition.
	/// All threads that are blocked waiting for this condition variable will
	/// be woken up.
	/// @note Only threads that started waiting prior to this call will be
	/// woken up.
	inline void notify_all()
	{
		pthread_cond_broadcast(&mHandle);
	}

	DISABLE_ASSIGNMENT(condition_variable)

private:
	pthread_cond_t mHandle;
};


static id _pthread_t_to_ID(const pthread_t &aHandle)
{
	static mutex idMapLock;
	static std::map<pthread_t, unsigned long int> idMap;
	static unsigned long int idCount(1);

	lock_guard<mutex> guard(idMapLock);
	if(idMap.find(aHandle) == idMap.end())
		idMap[aHandle] = idCount ++;
	return thread::id(idMap[aHandle]);
}
/// Thread class.
class thread {
public:
	typedef pthread_t native_handle_type;

	class id;

	/// Default constructor.
	/// Construct a @c thread object without an associated thread of execution
	/// (i.e. non-joinable).
	thread()
		: mHandle(0)
		, mNotAThread(true)
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
		_thread_start_info * ti = new _thread_start_info;
		ti->mFunction = aFunction;
		ti->mArg = aArg;
		ti->mThread = this;

		// The thread is now alive
		mNotAThread = false;

		// Create the thread
		if(pthread_create(&mHandle, NULL, wrapper_function, (void *) ti) != 0)
			mHandle = 0;

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
			pthread_join(mHandle, NULL);
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
			pthread_detach(mHandle);
			mNotAThread = true;
		}
		mDataMutex.unlock();
	}

	/// Return the thread ID of a thread object.
	id get_id() const
	{
		if(!joinable())
			return id();
		return _pthread_t_to_ID(mHandle);
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

#if defined(_SC_NPROCESSORS_ONLN)
		return (int) sysconf(_SC_NPROCESSORS_ONLN);
#elif defined(_SC_NPROC_ONLN)
		return (int) sysconf(_SC_NPROC_ONLN);
#else
		// The standard requires this function to return zero if the number of
		// hardware cores could not be determined.
		return 0;
#endif
	}

	DISABLE_ASSIGNMENT(thread)

private:
	native_handle_type mHandle;   ///< Thread handle.
	mutable mutex mDataMutex;     ///< Serializer for access to the thread private data.
	bool mNotAThread;             ///< True if this object is not a thread of execution.

	static void * wrapper_function(void * aArg)
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
		 return _pthread_t_to_ID(pthread_self());
	}

	/// Yield execution to another thread.
	/// Offers the operating system the opportunity to schedule another thread
	/// that is ready to run on the current processor.
	inline void yield()
	{
		sched_yield();
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
		usleep(int(double(aTime.count()) * (1000000.0 * _Period::_as_double()) + 0.5));
	}
}

} // namespace FoundationKit

#endif // defined(ANDROID) || defined(IOS)

#endif // FoundationKit_thread_posix_H

