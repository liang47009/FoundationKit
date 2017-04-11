
#include "FoundationKit/Foundation/TimerQueue.hpp"

NS_FK_BEGIN

TimerQueue::TimerQueue()
{

}

TimerQueue::~TimerQueue()
{

}

void TimerQueue::tick(float deltaTime)
{
    if (!_timersCache.empty())
    {
        for (std::vector<Timer::pointer>::iterator iter =_timersCache.begin(); iter!= _timersCache.end();++iter)
        {
            _timerlist.push_back(std::move(*iter));
        }
        _timersCache.clear();
    }

    if (!_willErase.empty())
    {
        for (std::vector<int32>::iterator timerid = _willErase.begin(); timerid!= _willErase.end();++timerid)
        {
            internalErase(*timerid);
        }
        _willErase.clear();
    }

    for (std::vector<Timer::pointer>::iterator iter =_timerlist.begin(); iter!= _timerlist.end();++iter)
    {
        (*iter)->update(deltaTime);
    }
}

int32 TimerQueue::enqueue(const TimerOption& timerOption)
{
    Timer::pointer pTimer = Timer::create(timerOption);
    return insert(pTimer);
}

int32 TimerQueue::insert(const Timer::pointer timer)
{
    int32 timerid = timer->get_id();
    _timersCache.push_back(timer);
    return timerid;
}

void TimerQueue::erase(const Timer::pointer timer)
{
    erase(timer->get_id());
}

void  TimerQueue::erase(int32 timerid)
{
    _willErase.push_back(timerid);
}

bool TimerQueue::exist(int32 timerid)
{
    bool bExist = false;
    for (std::vector<Timer::pointer>::iterator iter =_timerlist.begin(); iter!= _timerlist.end();++iter)
    {
        Timer::pointer timer = *iter;
        if (timer->get_id() == timerid)
        {
            bExist = true;
            break;
        }
    }
    return bExist;
}

void TimerQueue::enable(int32 timerid, bool value)
{
    Timer::pointer pTimer = getTimer(timerid);
    if (pTimer)
    {
        pTimer->setEnabled(value);
    }
}

void TimerQueue::start(int32 timerid)
{
    Timer::pointer pTimer = getTimer(timerid);
    if (pTimer)
    {
        pTimer->start();
    }
}

void TimerQueue::stop(int timerid)
{
    Timer::pointer pTimer = getTimer(timerid);
    if (pTimer)
    {
        pTimer->stop();
    }
}

Timer::pointer TimerQueue::getTimer(int32 timerid)
{
    Timer::pointer pTimer = nullptr;
    for (std::vector<Timer::pointer>::iterator iter =_timerlist.begin(); iter!= _timerlist.end();++iter)
    {
        Timer::pointer timer = *iter;
        if (timer->get_id() == timerid)
        {
            pTimer = timer;
            break;
        }
    }
    return pTimer;
}

void TimerQueue::internalErase(int32 timerid)
{
    for (std::vector<Timer::pointer>::iterator timerIter = _timerlist.begin(); timerIter != _timerlist.end(); ++timerIter)
    {
        if ((*timerIter)->get_id() == timerid)
        {
            _timerlist.erase(timerIter);
            break;
        }
    }
}

NS_FK_END







