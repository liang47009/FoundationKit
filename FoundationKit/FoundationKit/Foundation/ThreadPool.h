/****************************************************************************
  Copyright (c) 2014-2015 libo.
 
  losemymind.libo@gmail.com

****************************************************************************/

#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <future>
#include <functional>
#include <condition_variable>
#include "FoundationKit/FoundationKitDefines.h"
NS_FK_BEGIN


/** 
 * A simple C++11 Thread Pool implementation
 * Basic usage:
 * @code
 * Foundation::ThreadPool pool;
 * int poolTest(){return 100;}
 * auto result = pool.enqueue([](int n, string str){return str;}, 10, "sfsdfsdf");
 * auto result0 = pool.enqueue(poolTest);
 * std::cout << result.get() << std::endl;
 * std::cout << result0.get() << std::endl;
 * @endCode
 */

class ThreadPool final
{
public:
    ThreadPool();
    ThreadPool(size_t nThreadCount);

    // add new work item  to the pool
    template< typename F, typename... Args>
    auto enqueue(F&& f, Args&&... args)
        ->std::future< typename std::result_of<F(Args...)>::type >
    {
        using return_type = typename std::result_of<F(Args...)>::type;

        auto  task = std::make_shared<std::packaged_task<return_type()> >(
            std::bind(std::forward<F>(f), std::forward<Args>(args)...));

        std::future<return_type> return_future = task->get_future();
        {
            std::unique_lock<std::mutex> lock(m_queueMutex);
            if (m_stop)
            {
                throw std::runtime_error("The ThreadPool is stoped, can not enqueue task.");
            }
            m_tasks.emplace([task](){(*task)(); });
        }
        m_condition.notify_one();
        return return_future;
    }

    ~ThreadPool();
private:
    bool init(size_t nThreadCount);
private:
    std::vector< std::thread >          m_threads;
    std::queue< std::function<void()> > m_tasks;
    std::mutex                          m_queueMutex;
    std::condition_variable             m_condition;
    bool                                m_stop;
};

NS_FK_END
