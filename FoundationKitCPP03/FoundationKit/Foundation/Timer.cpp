#include "FoundationKit/Foundation/Timer.hpp"
#include "FoundationKit/Foundation/Timespan.hpp"
#include "FoundationKit/Foundation/ElapsedTimer.hpp"
#include "FoundationKit/Foundation/Logger.hpp"
NS_FK_BEGIN

int Timer::_nextValidId = 1000;

Timer::Timer()
{
    TimerOption to(false, 1000, 1.0f, 100, repeat_forever);
    Init(to);
}

Timer::Timer(int interval)
{
    TimerOption to(false, interval, 1.0f, 100, repeat_forever);
    Init(to);
}

Timer::Timer(const TimerOption& option)
{
    Init(option);
}


Timer::~Timer()
{
    SetEnabled(false);
}


void Timer::Init(const TimerOption& option)
{
    onTimedEvent = option.onTimedEvent;
    SetEnabled(option.enable);
    SetInterval(option.interval);
    SetTimeScale(option.scale);
    SetMaximumDeltaTime(option.maximumDeltaTime);// initialized to 100 milliseconds.
    _repeatCount = option.repeatCount;

    _deltaTime = 0;
    _frameCount = 0;
    _elapsedTime = 0;
    _myid = _nextValidId++;
}

Timer::pointer Timer::Create()
{
    Timer::pointer pTimer = std::shared_ptr<Timer>(new Timer());
    return pTimer;
}

Timer::pointer Timer::Create(int interval)
{
    Timer::pointer pTimer = std::shared_ptr<Timer>(new Timer(interval));
    return pTimer;
}

Timer::pointer Timer::Create(const TimerOption& option)
{
    Timer::pointer pTimer = std::shared_ptr<Timer>(new Timer(option));
    return pTimer;
}

void Timer::SetOption(const TimerOption& option)
{
    this->SetEnabled(option.enable)
        .SetInterval(option.interval)
        .SetTimeScale(option.scale)
        .SetMaximumDeltaTime(option.maximumDeltaTime)
        .SetRepeatCount(option.repeatCount);
    this->onTimedEvent = option.onTimedEvent;
}

Timer& Timer::SetEnabled(bool value)
{
    _enable = value;
    return (*this);
}
Timer& Timer::SetInterval(int value)
{
    if (value < 0)
        throw std::invalid_argument("setInterval value must be > 0");
    _interval = value;
    return (*this);
}

Timer& Timer::SetTimeScale(float value)
{
    _timeScale = value;
    return (*this);
}

Timer& Timer::SetMaximumDeltaTime(int value)
{
    _maximumDeltaTime = value;
    return (*this);
}

Timer& Timer::SetRepeatCount(int value)
{
    _repeatCount = value;
    return (*this);
}

bool Timer::GetEnabled()const
{
    return _enable;
}

int Timer::GetInterval()const
{
    return _interval;
}


float Timer::GetTimeScale()const
{
    return _timeScale;
}


int Timer::GetMaximumDeltaTime()const
{
    return _maximumDeltaTime;
}

int Timer::GetDeltaTime()const
{
    return _deltaTime;
}

int Timer::GetFrameCount()const
{
    return _frameCount;
}

int Timer::GetId()const
{
    return _myid;
}

void Timer::Update(float deltaTime)
{
    if (!_enable) return;

    if (deltaTime > _maximumDeltaTime)
    {
        deltaTime = _maximumDeltaTime;
    }

    _deltaTime = deltaTime;
    _elapsedTime += deltaTime;

    if (onTimedEvent && _elapsedTime > _interval)
    {
        TimedEventArgs timerArgs;
        timerArgs.deltaTime = static_cast<int>(_elapsedTime*_timeScale);
        timerArgs.signalTime = DateTime::Now();
        timerArgs.timerId = _myid;
        onTimedEvent(timerArgs);
        _elapsedTime = 0;
        _frameCount += 1;
        if (_frameCount >= _repeatCount && _repeatCount != repeat_forever)
        {
            Stop();
        }
    }
}

void Timer::Start()
{
    SetEnabled(true);
}

void Timer::Stop()
{
    SetEnabled(false);
    Reset();
}


void Timer::Reset()
{
    _deltaTime = 0;
    _frameCount = 0;
    _elapsedTime = 0;
}


NS_FK_END




