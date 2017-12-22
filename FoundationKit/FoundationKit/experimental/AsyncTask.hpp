

#include <thread>
#include <mutex>
#include <list>
#include <atomic>
#include <future>
#include <condition_variable>
#include "FoundationKit/GenericPlatformMacros.hpp"
#include "FoundationKit/Foundation/Singleton.hpp"

NS_FK_BEGIN

/**
 * Enumerates possible states of slow running tasks.
 */
enum class EAsyncTaskState
{
    /** Task has been canceled. */
    Cancelled,

    /** Task has completed execution. */
    Completed,

    /** Task is empty. */
    Empty,

    /** Task execution failed. */
    Failed,

    /** Task execution is still pending. */
    Pending,

    /** Task is executing. */
    Running,
};

class AsyncTask
{
public:
    typedef std::shared_ptr<AsyncTask>   Pointer;

    virtual EAsyncTaskState GetTaskState()
    {
        return TaskState;
    }

    virtual void Cancel()
    {

    }

    virtual void Run()
    {

    }
protected:
    EAsyncTaskState          TaskState;
};


template<typename _RT, typename _FT>
class AsyncResult
{
public:
    std::future<_RT>   Future;
};


class AsyncTaskPool : public Singleton<AsyncTaskPool>
{
    friend Singleton<AsyncTaskPool>;

    AsyncTaskPool()
    {
       
    }
public:

    ~AsyncTaskPool()
    {

    }

    void Initialize()
    {
    }

  
    template<class _Fty, class... _ArgTypes>
    AsyncTask::Pointer RunTask(_Fty&& _Fnarg, _ArgTypes&&... _Args)
    {

    }


    void StopTask(const AsyncTask::Pointer& task)
    {
        task->Cancel();
    }

protected:

};


NS_FK_END





