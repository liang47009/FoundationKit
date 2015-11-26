#ifndef FoundationKit_Impl_thread_H
#define FoundationKit_Impl_thread_H

#include <iostream>

namespace FoundationKit
{

#define COMPILER_SUPPORT_CPP11 0
#define IS_CPP11_PARTIAL       0

// Do we have a fully featured C++11 compiler?
#if (__cplusplus > 199711L) || (defined(__STDCXX_VERSION__) && (__STDCXX_VERSION__ >= 201001L))
#undef  COMPILER_SUPPORT_CPP11
#define COMPILER_SUPPORT_CPP11 1
#endif

// ...at least partial C++11?
#if(COMPILER_SUPPORT_CPP11) || defined(__GXX_EXPERIMENTAL_CXX0X__) || defined(__GXX_EXPERIMENTAL_CPP0X__)
#undef  IS_CPP11_PARTIAL
#define IS_CPP11_PARTIAL 1
#endif

// Macro for disabling assignments of objects.
#if(IS_CPP11_PARTIAL)
#define DISABLE_ASSIGNMENT(name) \
	name(const name&) = delete; \
	name& operator=(const name&) = delete;
#else
#define DISABLE_ASSIGNMENT(name) \
	name(const name&); \
	name& operator=(const name&);
#endif


/// @def thread_local
/// Thread local storage keyword.
/// A variable that is declared with the @c thread_local keyword makes the
/// value of the variable local to each thread (known as thread-local storage,
/// or TLS). Example usage:
/// @code
/// // This variable is local to each thread.
/// thread_local int variable;
/// @endcode
/// @note The @c thread_local keyword is a macro that maps to the corresponding
/// compiler directive (e.g. @c __declspec(thread)). While the C++11 standard
/// allows for non-trivial types (e.g. classes with constructors and
/// destructors) to be declared with the @c thread_local keyword, most pre-C++11
/// compilers only allow for trivial types (e.g. @c int). So, to guarantee
/// portable code, only use trivial types for thread local storage.
/// @note This directive is currently not supported on Mac OS X (it will give
/// a compiler error), since compile-time TLS is not supported in the Mac OS X
/// executable format. Also, some older versions of MinGW (before GCC 4.x) do
/// not support this directive.
/// @hideinitializer

#if(!COMPILER_SUPPORT_CPP11) && !defined(thread_local)
#if defined(__GNUC__) || defined(__INTEL_COMPILER) || defined(__SUNPRO_CC) || defined(__IBMCPP__)
#define thread_local __thread
#else
#define thread_local __declspec(thread)
#endif
#endif


/// Lock guard class.
/// The constructor locks the mutex, and the destructor unlocks the mutex, so
/// the mutex will automatically be unlocked when the lock guard goes out of
/// scope. Example usage:
/// @code
/// mutex m;
/// int counter;
///
/// void increment()
/// {
///   lock_guard<mutex> guard(m);
///   ++ counter;
/// }
/// @endcode

template <class T>
class lock_guard {
public:
	typedef T mutex_type;

	lock_guard() : mMutex(0) {}

	/// The constructor locks the mutex.
	explicit lock_guard(mutex_type &aMutex)
	{
		mMutex = &aMutex;
		mMutex->lock();
	}

	/// The destructor unlocks the mutex.
	~lock_guard()
	{
		if(mMutex)
			mMutex->unlock();
	}

private:
	mutex_type * mMutex;
};


class thread;

/// Information to pass to the new thread (what to run).
struct thread_parameter {
	void (*_workfunction)(void *); ///< Pointer to the function to be executed.
	void *   _param;               ///< Function argument for the thread function.
	thread * _thread;          ///< Pointer to the thread object.
};

/// Thread ID.
/// The thread ID is a unique identifier for each thread.
/// @see thread::get_id()
class id {
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

	inline friend std::ostream& operator <<(std::ostream &os, const id &obj)
	{
		os << obj.mId;
		return os;
	}

private:
	unsigned long int mId;
};





// Related to <ratio> - minimal to be able to support chrono.
typedef long long __intmax_t;

/// Minimal implementation of the @c ratio class. This class provides enough
/// functionality to implement some basic @c chrono classes.
template <__intmax_t N, __intmax_t D = 1> class ratio {
public:
	static double _as_double() { return double(N) / double(D); }
};

/// Minimal implementation of the @c chrono namespace.
/// The @c chrono namespace provides types for specifying time intervals.
namespace chrono {
	/// Duration template class. This class provides enough functionality to
	/// implement @c this_thread::sleep_for().
	template <class _Rep, class _Period = ratio<1> > class duration {
	private:
		_Rep rep_;
	public:
		typedef _Rep rep;
		typedef _Period period;

		/// Construct a duration object with the given duration.
		template <class _Rep2>
		explicit duration(const _Rep2& r) : rep_(r) {};

		/// Return the value of the duration object.
		rep count() const
		{
			return rep_;
		}
	};

	// Standard duration types.
	typedef duration<__intmax_t, ratio<1, 1000000000> > nanoseconds; ///< Duration with the unit nanoseconds.
	typedef duration<__intmax_t, ratio<1, 1000000> > microseconds;   ///< Duration with the unit microseconds.
	typedef duration<__intmax_t, ratio<1, 1000> > milliseconds;      ///< Duration with the unit milliseconds.
	typedef duration<__intmax_t> seconds;                            ///< Duration with the unit seconds.
	typedef duration<__intmax_t, ratio<60> > minutes;                ///< Duration with the unit minutes.
	typedef duration<__intmax_t, ratio<3600> > hours;                ///< Duration with the unit hours.
}

} //namespace FoundationKit

#endif // FoundationKit_Impl_thread_H