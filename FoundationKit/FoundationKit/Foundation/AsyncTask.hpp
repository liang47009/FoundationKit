

#include <thread>
#include <mutex>
#include <condition_variable>
#include "FoundationKit/Foundation/FunctionHandler.hpp"

NS_FK_BEGIN

/**
 * Enumerates possible states of slow running tasks.
 */
enum class AsyncTaskState
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

/**
 * Interface for asynchronous tasks.
 *
 * A asynchronous task is a unit of work that runs in parallel to the caller and may take a
 * considerable amount of time to complete, i.e. several seconds, minutes or even hours.
 * This interface provides mechanisms for tracking and canceling such tasks.
 */
class IAsyncTask
{
public:

    /**
    * Cancel this task.
    *
    * If the task is already running, it should make a best effort to abort execution
    * as soon as possible. This method should be implemented in such a way that it returns
    * immediately and does not block the caller.
    */
    virtual void Cancel() = 0;

    /**
    * Gets the current state of the task.
    *
    * @return Task state.
    */
    virtual AsyncTaskState GetTaskState() = 0;

public:

    /** Virtual destructor. */
    virtual ~IAsyncTask() { }

protected:
    FunctionHandlerPointer  TaskSelector;
};


class AsyncTaskPool
{
public:
    AsyncTaskPool();
    ~AsyncTaskPool();
};


NS_FK_END





