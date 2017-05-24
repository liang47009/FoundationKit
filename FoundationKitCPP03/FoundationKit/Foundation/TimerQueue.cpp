
#include "FoundationKit/Foundation/TimerQueue.hpp"

NS_FK_BEGIN

TimerQueue::TimerQueue()
{

}

TimerQueue::~TimerQueue()
{

}

void TimerQueue::Tick(float deltaTime)
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
            InternalErase(*timerid);
        }
        _willErase.clear();
    }

    for (std::vector<Timer::pointer>::iterator iter =_timerlist.begin(); iter!= _timerlist.end();++iter)
    {
        (*iter)->Update(deltaTime);
    }
}

int32 TimerQueue::Enqueue(const TimerOption& timerOption)
{
    Timer::pointer pTimer = Timer::Create(timerOption);
    return Insert(pTimer);
}

int32 TimerQueue::Insert(const Timer::pointer timer)
{
    int32 timerid = timer->GetId();
    _timersCache.push_back(timer);
    return timerid;
}

void TimerQueue::Erase(const Timer::pointer timer)
{
    Erase(timer->GetId());
}

void  TimerQueue::Erase(int32 timerid)
{
    _willErase.push_back(timerid);
}

bool TimerQueue::Exist(int32 timerid)
{
    bool bExist = false;
    for (std::vector<Timer::pointer>::iterator iter =_timerlist.begin(); iter!= _timerlist.end();++iter)
    {
        Timer::pointer timer = *iter;
        if (timer->GetId() == timerid)
        {
            bExist = true;
            break;
        }
    }
    return bExist;
}

void TimerQueue::Enable(int32 timerid, bool value)
{
    Timer::pointer pTimer = GetTimer(timerid);
    if (pTimer)
    {
        pTimer->SetEnabled(value);
    }
}

void TimerQueue::Start(int32 timerid)
{
    Timer::pointer pTimer = GetTimer(timerid);
    if (pTimer)
    {
        pTimer->Start();
    }
}

void TimerQueue::Stop(int timerid)
{
    Timer::pointer pTimer = GetTimer(timerid);
    if (pTimer)
    {
        pTimer->Stop();
    }
}

Timer::pointer TimerQueue::GetTimer(int32 timerid)
{
    Timer::pointer pTimer = nullptr;
    for (std::vector<Timer::pointer>::iterator iter =_timerlist.begin(); iter!= _timerlist.end();++iter)
    {
        Timer::pointer timer = *iter;
        if (timer->GetId() == timerid)
        {
            pTimer = timer;
            break;
        }
    }
    return pTimer;
}

void TimerQueue::InternalErase(int32 timerid)
{
    for (std::vector<Timer::pointer>::iterator timerIter = _timerlist.begin(); timerIter != _timerlist.end(); ++timerIter)
    {
        if ((*timerIter)->GetId() == timerid)
        {
            _timerlist.erase(timerIter);
            break;
        }
    }
}

NS_FK_END







