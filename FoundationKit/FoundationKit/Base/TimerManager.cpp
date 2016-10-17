#include <cassert>
#include "TimerManager.hpp"
#include "FoundationKit/Foundation/Logger.h"

NS_FK_BEGIN

int32 TimerManager::_nextValidId = 1000;

TimerManager::TimerManager()
{

}

TimerManager::~TimerManager()
{

}

int64 TimerManager::addTimer(const TimerElapsedEvent& callback, int64 interval, uint32 repeat, bool paused)
{
    TimerData  newTimerData;
    newTimerData.elapsed = callback;
    newTimerData.idIndex = _nextValidId++;
    newTimerData.interval = interval;
    newTimerData.repeat = repeat;
    newTimerData.state = (paused ? ETimerStatus::PAUSED : ETimerStatus::ACTIVE);
    if (paused)
    {
        _pausedTimerList.push_back(newTimerData);
    }
    else
    {
        _activeTimerList.push_back(newTimerData);
    }
    return newTimerData.idIndex;
}

void TimerManager::removeTimer(int32 timerId)
{
    _willRemoveTimers.push_back(timerId);
}

void TimerManager::pauseTimer(int32 timerId)
{
    size_t timerIdx;
    TimerData const* const timerData = findTimer(timerId, timerIdx);
    if (timerData && timerData->state == ETimerStatus::ACTIVE)
    {
        TimerData temp = *timerData;
        temp.state = ETimerStatus::PAUSED;
        _swapTimerList.push_back(temp);
        removeTimer(timerId);
    }
}

void TimerManager::resumeTimer(int32 timerId)
{
    size_t timerIdx;
    TimerData const* const timerData = findTimer(timerId, timerIdx);
    if (timerData && timerData->state == ETimerStatus::PAUSED)
    {
        TimerData temp = *timerData;
        temp.state = ETimerStatus::ACTIVE;
        _swapTimerList.push_back(temp);
        removeTimer(timerId);
    }
}

bool TimerManager::isTimerActive(int32 timerId)
{
    size_t activeTimerIdx = findTimerInList(_activeTimerList, timerId);
    return (activeTimerIdx != INDEX_NONE);
}

bool TimerManager::isTimerPaused(int32 timerId)
{
    size_t pausedTimerIdx = findTimerInList(_pausedTimerList, timerId);
    return (pausedTimerIdx != INDEX_NONE);
}

bool TimerManager::timerExists(int32 timerId)
{
    size_t timerIdx;
    TimerData const* const timerData = findTimer(timerId, timerIdx);
    return (timerData != nullptr);
}

int64 TimerManager::getTimerElapsed(int32 timerId)
{
    size_t timerIdx;
    TimerData const* const timerData = findTimer(timerId, timerIdx);
    if (timerData)
    {
        return timerData->timer.milliseconds();
    }
    return -1;
}

int64 TimerManager::getTimerInterval(int32 timerId)
{
    size_t timerIdx;
    TimerData const* const timerData = findTimer(timerId, timerIdx);
    if (timerData)
    {
        return timerData->interval;
    }
    return -1;
}

void TimerManager::update(float deltaTime)
{
    // First remove timers.
    for (auto timerId : _willRemoveTimers)
    {
        size_t timerIdx;
        TimerData const* const timerData = findTimer(timerId, timerIdx);
        if (timerData)
        {
            switch (timerData->state)
            {
            case ETimerStatus::ACTIVE:
                _activeTimerList.erase(_activeTimerList.begin() + timerIdx);
                break;
            case ETimerStatus::PAUSED:
                _pausedTimerList.erase(_pausedTimerList.begin() + timerIdx);
                break;
            default:
                assert(false);
                break;
            }
        }
    }
    _willRemoveTimers.clear();

    // Swap timer
    for (auto& timerData : _swapTimerList)
    {
        timerData.timer.reset();
        switch (timerData.state)
        {
        case ETimerStatus::ACTIVE:
            _activeTimerList.push_back(timerData);
            break;
        case ETimerStatus::PAUSED:
            _pausedTimerList.push_back(timerData);
            break;
        default:
            assert(false);
            break;
        }
    }
    _swapTimerList.clear();

    for (auto& timerData : _activeTimerList)
    {
        if (timerData.timer.milliseconds() >= timerData.interval)
        {
            ElapsedEventArgs e;
            e.signalTime = DateTime::now();
            e.timerId = timerData.idIndex;
            timerData.elapsed(&e);
            timerData.timer.reset();
        }
    }
}

TimerData const* TimerManager::findTimer(int32 timerId, size_t& outIndex)
{
    size_t activeTimerIdx = findTimerInList(_activeTimerList, timerId);
    if (activeTimerIdx != INDEX_NONE)
    {
        outIndex = activeTimerIdx;
        return &_activeTimerList[activeTimerIdx];
    }

    size_t pausedTimerIdx = findTimerInList(_pausedTimerList, timerId);
    if (pausedTimerIdx != INDEX_NONE)
    {
        outIndex = pausedTimerIdx;
        return &_pausedTimerList[pausedTimerIdx];
    }
    outIndex = INDEX_NONE;
    return nullptr;
}

size_t TimerManager::findTimerInList(const std::vector<TimerData>& searchArray, int32 const& timerId)
{
    for (size_t idx = 0; idx < searchArray.size(); ++idx)
    {
        if (searchArray[idx].idIndex == timerId)
        {
            return idx;
        }
    }
    return INDEX_NONE;
}



NS_FK_END


