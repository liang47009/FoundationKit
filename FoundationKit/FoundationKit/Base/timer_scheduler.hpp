#ifndef LOSEMYMIND_TIMER_SCHEDULER_H
#define LOSEMYMIND_TIMER_SCHEDULER_H


#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <functional>
#include <unordered_map>
#include "FoundationKit/Base/Timer.h"
#include "FoundationKit/Base/Types.h"
#include "FoundationKit/Foundation/Logger.h"
#include "FoundationKit/FoundationMacros.h"

NS_FK_BEGIN

#define SCHEDULER_REPEAT_FOREVER (UINT_MAX -1)
typedef std::function<void(float, void*)>  TimerCallback;
class timer_scheduler;
static void removeTimerDelegateFromScheduler(timer_scheduler* ts,const std::string& name);
class TimerDelegate
{
    friend class timer_scheduler;

    /// milliseconds
    float            _interval;
    float            _timeScale;
    float            _delay;
    uint32           _repeat;
    void*            _userData;
    std::string      _name;
    Timer            _timer;
    TimerCallback    _callback;
    bool             _useDelay;
    bool             _runForever;
    bool             _paused;
    uint32           _timesExecuted;
    timer_scheduler* _timer_scheduler;

    TimerDelegate()
        : _interval(0)
        , _timeScale(1.0f)
        , _delay(0.0f)
        , _repeat(1)
        , _userData(nullptr)
        , _useDelay(false)
        , _runForever(false)
        , _paused(false)
        , _timesExecuted(0)
    {

    }

    TimerDelegate(const std::string& name, const TimerCallback& callback, float interval, uint32 repeat = 1, float delay = 0.0f, float timeScale = 1.0f)
        : _interval(interval)
        , _timeScale(timeScale)
        , _delay(delay)
        , _repeat(repeat)
        , _userData(nullptr)
        , _name(name)
        , _callback(callback)
        , _useDelay((_delay > 0.0f) ? true : false)
        , _runForever((repeat == SCHEDULER_REPEAT_FOREVER) ? true : false)
        , _paused(false)
        , _timesExecuted(0)
    {
        _timer.reset();
    }

public:
    void setup(const std::string& name, const TimerCallback& callback, float interval, uint32 repeat = 1, float delay = 0.0f, float timeScale = 1.0f)
    {
        _interval = interval;
        _timeScale = timeScale;
        _delay = delay;
        _repeat = repeat;
        _name = name;
        _callback = callback;
        _useDelay = (_delay > 0.0f) ? true : false;
        _runForever = (repeat == SCHEDULER_REPEAT_FOREVER) ? true : false;
        _timesExecuted = 0;
        _timer.reset();
    }

    void setUserData(void* userData)
    {
        _userData = userData;
    }

    void* getUserData()
    {
        return _userData;
    }

    void setTimeScale(float timeScale)
    {
        _timeScale = timeScale;
    }

    float getTimeScale()
    {
        return _timeScale;
    }

    void setPaused(bool paused)
    {
        _paused = paused;
    }

    bool getPaused()
    {
        return _paused;
    }

    void update(float deltaTime)
    {
        if (_paused) return;
        if (_useDelay)
        {
            if (getElapsed() >= _delay)
            {
                _callback(getElapsed(), _userData);
                _timer.reset();
                _timesExecuted += 1;
            }
            return;
        }

        if (getElapsed() >= _interval)
        {
            _callback(getElapsed(), _userData);
            _timesExecuted += 1;
            _timer.reset();
        }

        if (!_runForever && _timesExecuted > _repeat)
        {    
            // remove it from timer_scheduler
            removeTimerDelegateFromScheduler(_timer_scheduler, _name);
        }
    }

private:
    float getElapsed()
    {
        return _timer.millisecondsf()*_timeScale;
    }

    void setTimerScheduler(timer_scheduler* ts)
    {
        _timer_scheduler = ts;
    }
    
};


class timer_scheduler
{

public:

    timer_scheduler()
    {

    }

    virtual~timer_scheduler()
    {
        for (auto iter : _timerDelegates)
        {
            SAFE_DELETE(iter.second.second);
        }
        _timerDelegates.clear();
    }


    void schedule(const std::string& name, const TimerCallback& callback, float interval, void* userData, bool paused)
    {
        schedule(name, callback, interval, userData, SCHEDULER_REPEAT_FOREVER, paused);
    }

    void schedule(const std::string& name, const TimerCallback& callback, float interval, void* userData = nullptr, uint32 repeat = 1, bool paused = false, float delay = 0.0f, float timeScale = 1.0f)
    {
        LOG_ASSERT(!name.empty(), "key should not be empty!");
        if (!timerDelegateIsExsit(name))
        {
            TimerDelegate* td = new TimerDelegate(name,callback, interval, repeat, delay, timeScale);
            td->setUserData(userData);
            td->setPaused(paused);
            addTimerDelegate(name, td);
        }
    }


    void unschedule(const std::string& name)
    {
        delTimerDelegatge(name);
    }

    void update(float deltaTime)
    {
        for (TimerDelegateMap::iterator iter = _timerDelegates.begin(); iter != _timerDelegates.end(); ++iter)
        {
            if (iter->second.first == false)
            {
                SAFE_DELETE(iter->second.second);
                iter = _timerDelegates.erase(iter);
            }
        }

        for (auto iter : _timerDelegates)
        {
            iter.second.second->update(deltaTime);
        }
    }

private:

    bool timerDelegateIsExsit(const std::string& name)
    {
        return (_timerDelegates.find(name) != _timerDelegates.end());
    }

    void addTimerDelegate(const std::string& name, TimerDelegate* td)
    {
        if (_timerDelegates.find(name) != _timerDelegates.end())
        {
            LOG_WARN("The [TimerDelegate] of name:%s is exsited, it will be replace.", name.c_str());
        }
        _timerDelegates[name] = std::make_pair(true, td);
    }

    void delTimerDelegatge(const std::string& name)
    {
        auto iter = _timerDelegates.find(name);
        if (iter != _timerDelegates.end())
        {
            iter->second.first = false;
        }
    }

private:
    // pair first: The TimerDelegate is valid
    typedef std::pair<bool, TimerDelegate*>  TimerDelegatePair;
    typedef std::unordered_map<std::string, TimerDelegatePair> TimerDelegateMap;
    TimerDelegateMap   _timerDelegates;
};

void removeTimerDelegateFromScheduler(timer_scheduler* ts, const std::string& name)
{
    ts->unschedule(name);
}

NS_FK_END










#endif // LOSEMYMIND_TIMER_SCHEDULER_H
