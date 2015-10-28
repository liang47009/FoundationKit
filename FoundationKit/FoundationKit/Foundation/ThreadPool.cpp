
#include "ThreadPool.h"

NS_FK_BEGIN

ThreadPool::ThreadPool():
    m_stop(false)
{
    init(0);
}

ThreadPool::ThreadPool( size_t nThreadCount ):
    m_stop(false)
{
    init(nThreadCount);
}

ThreadPool::~ThreadPool()
{
    {
        std::unique_lock<std::mutex> lock(m_queueMutex);
        m_stop = true;
    }
    m_condition.notify_all();
    for (std::thread& _thread : m_threads)
    {
        _thread.join();
    }
}

bool ThreadPool::init( size_t nThreadCount )
{
    if (nThreadCount == 0)
    {
        nThreadCount = std::thread::hardware_concurrency()*2;
    }

    for (size_t i = 0; i < nThreadCount; ++i)
    {
        m_threads.emplace_back([this]{
            for (;;)
            {
                std::function<void()> task;
                {
                    std::unique_lock<std::mutex> lock(this->m_queueMutex);
                    this->m_condition.wait(lock, 
                        [this]{return this->m_stop || !this->m_tasks.empty();});
                    if (this->m_stop && this->m_tasks.empty())
                        return;
                    task = std::move(this->m_tasks.front());
                    this->m_tasks.pop();
                }
                task();
            }
        });
    }
    return true;
}
NS_FK_END