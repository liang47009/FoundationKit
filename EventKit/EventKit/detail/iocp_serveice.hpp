
#include "FoundationKit/GenericPlatformMacros.hpp"
#include "FoundationKit/Base/error.hpp"
#include <thread>
#include <memory>

//struct OVERLAPPEDPLUS
//{
//    OVERLAPPED Overlapped;
//    OVERLAPPEDPLUS()
//    {
//        memset(&Overlapped, 0, sizeof(OVERLAPPED));
//    }
//};


class overlapped_wrapper : public OVERLAPPED
{
public:
    explicit overlapped_wrapper(std::error_code& ec)
    {
        Internal = 0;
        InternalHigh = 0;
        Offset = 0;
        OffsetHigh = 0;

        // Create a non-signalled manual-reset event, for GetOverlappedResult.
        hEvent = ::CreateEvent(0, TRUE, FALSE, 0);
        if (hEvent)
        {
            // As documented in GetQueuedCompletionStatus, setting the low order
            // bit of this event prevents our synchronous writes from being treated
            // as completion port events.
            DWORD_PTR tmp = reinterpret_cast<DWORD_PTR>(hEvent);
            hEvent = reinterpret_cast<HANDLE>(tmp | 1);
        }
        else
        {
            ec = std::error_code(::GetLastError(), std::generic_category());
        }
    }

    ~overlapped_wrapper()
    {
        if (hEvent)
        {
            ::CloseHandle(hEvent);
        }
    }
};


// Helper class for managing a HANDLE.
struct win_auto_handle
{
    HANDLE handle;
    win_auto_handle() : handle(0) {}
    ~win_auto_handle() { if (handle) ::CloseHandle(handle); }
};

class iocp_service
{
protected:
    // The IO completion port used for queueing operations.
    win_auto_handle               iocp_instance;
    // A waitable timer object used for waiting for timeouts.
    win_auto_handle               waitable_timer;
    std::shared_ptr<std::thread>  timer_thread;
    // Flag to indicate whether the service has been shut down.
    long                          shutdown_flag;
public:
    iocp_service(size_t concurrency_count)
        : iocp_instance()
    {
        iocp_instance.handle = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0,
            static_cast<DWORD>(concurrency_count < DWORD(~0)? concurrency_count : DWORD(~0)));
        if (!iocp_instance.handle)
        {
            std::error_code ec = std::error_code(::GetLastError(), std::generic_category());
            THROW_IF(ec);
        }
    }

    void shutdown()
    {
        ::InterlockedExchange(&shutdown_flag, 1);

        if (timer_thread.get())
        {
            LARGE_INTEGER timeout;
            timeout.QuadPart = 1;
            ::SetWaitableTimer(waitable_timer.handle, &timeout, 1, 0, 0, FALSE);
        }

        while (::InterlockedExchangeAdd(&outstanding_work_, 0) > 0)
        {
            op_queue<win_iocp_operation> ops;
            timer_queues_.get_all_timers(ops);
            ops.push(completed_ops_);
            if (!ops.empty())
            {
                while (win_iocp_operation* op = ops.front())
                {
                    ops.pop();
                    ::InterlockedDecrement(&outstanding_work_);
                    op->destroy();
                }
            }
            else
            {
                DWORD bytes_transferred = 0;
                dword_ptr_t completion_key = 0;
                LPOVERLAPPED overlapped = 0;
                ::GetQueuedCompletionStatus(iocp_instance.handle, &bytes_transferred,
                    &completion_key, &overlapped, gqcs_timeout_);
                if (overlapped)
                {
                    ::InterlockedDecrement(&outstanding_work_);
                    static_cast<win_iocp_operation*>(overlapped)->destroy();
                }
            }
        }

        if (timer_thread.get())
            timer_thread->join();
    }



    size_t do_one(bool block, std::error_code& ec)
    {

    }
};


