#include <algorithm>
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
        for (auto& timer : _timersCache)
        {
            _timerlist.push_back(std::move(timer));
        }
        _timersCache.clear();
    }

    if (!_willErase.empty())
    {
        for (auto timerid : _willErase)
        {
            std::vector<Timer::pointer>::iterator timerIter = std::find(_timerlist.begin(), _timerlist.end(), timerid);
            if (timerIter != _timerlist.end())
            {
                _timerlist.erase(timerIter);
            }
        }
        _willErase.clear();
    }

    for (auto& timer : _timerlist)
    {
        timer->Tick(deltaTime);
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
    for (auto& timer : _timerlist)
    {
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
    for (auto& timer : _timerlist)
    {
        if (timer->GetId() == timerid)
        {
            pTimer = timer;
            break;
        }
    }
    return pTimer;
}

NS_FK_END







