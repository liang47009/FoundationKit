/****************************************************************************
  Copyright (c) 2013-2014 libo.
 
  losemymind.libo@gmail.com

****************************************************************************/

#include <condition_variable>
#include "WorkQueue.h"

NS_FK_BEGIN

static std::mutex		            s_SleepMutex;
static std::condition_variable		s_SleepCondition;
static bool                         s_need_quit = false;

WorkQueue::WorkQueue()
{
    size_t nThreadCount = std::thread::hardware_concurrency()*2;
    createThreads(nThreadCount);
}

WorkQueue::~WorkQueue()
{
    s_need_quit = true;
}

void WorkQueue::createThreads( size_t nThreadCount )
{
    s_need_quit = false;
    for (size_t i = 0; i < nThreadCount; ++i)
    {
        auto t = std::make_shared<std::thread>(std::bind(&WorkQueue::run, this));
        t->detach();
        m_threads.push_back(t);
    }
}

void WorkQueue::run()
{
    while (true)
    {
        if (s_need_quit)
        {
            break;
        }

        Runnable* pRunnable = nullptr;

        m_lock.lock();
        if (!m_works.empty())
        {
            pRunnable = m_works.front();
            m_works.pop();
        }
        m_lock.unlock();

        if (pRunnable == nullptr)
        {
            std::unique_lock<std::mutex> lk(s_SleepMutex); 
            s_SleepCondition.wait(lk);
            //std::this_thread::sleep_for(std::chrono::milliseconds(10));
            continue;
        }

        pRunnable->run();
    }
}

void WorkQueue::addWork( Runnable* pWork )
{
    m_lock.lock();
    m_works.push(pWork);
    m_lock.unlock();
    s_SleepCondition.notify_one();
}

NS_FK_END
