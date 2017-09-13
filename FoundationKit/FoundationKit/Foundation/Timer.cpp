
#include "FoundationKit/Foundation/Timer.hpp"
#include "FoundationKit/Foundation/Timespan.hpp"
#include "FoundationKit/Foundation/ElapsedTimer.hpp"
NS_FK_BEGIN

int Timer::_nextValidId = 1000;

Timer::Timer()
{
    _interval = 1000;
    _deltaTime = 0;
    _repeatCount = repeat_forever;
    _frameCount = 0;
    _bRunning = false;
    //_deltaTimer;
    _timeScale = 1.0f;
    _myid = _nextValidId++;
}
Timer::Timer(Timer&& other)
{
    Move(std::forward<Timer&&>(other));
}

Timer::~Timer()
{
    Stop();
    _future.wait();
}

Timer& Timer::operator=(Timer&& other)
{
    move(std::forward<Timer&&>(other));
    return (*this);
}

Timer::Pointer Timer::Create()
{
    Timer::Pointer pTimer = Timer::Pointer(new Timer());
    return pTimer;
}

Timer& Timer::SetInterval(long value)
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

Timer& Timer::SetRepeatCount(long value)
{
    _repeatCount = value;
    return (*this);
}

Timer& Timer::SetUserData(void* userData)
{
    _userData = userData;
    return (*this);
}

Timer& Timer::SetTimedEvent(const TimedEvent& callback)
{
    onTimedEvent = callback;
    return (*this);
}

long Timer::GetInterval()const
{
    return _interval.load();
}


float Timer::GetTimeScale()const
{
    return _timeScale.load();
}

long Timer::GetDeltaTime()const
{
    return _deltaTime.load();
}

long Timer::GetFrameCount()const
{
    return _frameCount.load();
}

long Timer::GetId()const
{
    return _myid;
}

void* Timer::GetUserData() const
{
    return _userData;
}

bool Timer::Start()
{
    if (onTimedEvent)
    {
        _bRunning = true;
        _deltaTimer.Reset();
        _future = std::async(std::launch::async, std::bind(&Timer::Run, this));
        return true;
    }
    return false;
}

bool Timer::Start(const TimedEvent& callback)
{
    onTimedEvent = callback;
    return Start();
}

bool Timer::Start(const TimedEvent& callback, long milliseconds, long repeatCount /*= repeat_forever*/, float timeScale /*= 1.0f*/)
{
    _interval    = milliseconds;
    _repeatCount = repeatCount;
    _timeScale   = timeScale;
    onTimedEvent = callback;
    return Start();
}

void Timer::Stop()
{
    _bRunning = false;
    Reset();
}

void Timer::Move(Timer&& other)
{
    SetInterval(other.GetInterval());
    SetTimeScale(other.GetTimeScale());
    _deltaTime   = other._deltaTime.load();
    _frameCount  = other._frameCount.load();
    _repeatCount = other._repeatCount.load();
    _myid        = other._myid;
    other._myid  = involid_id;
    other.Stop();
}

void Timer::Reset()
{
    _deltaTime   = 0;
    _frameCount  = 0;
}

void Timer::Run()
{
    do 
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        //std::this_thread::yield();
        long  elapsedTime   = static_cast<long>(_deltaTimer.Milliseconds());
        long  finalInterval = static_cast<long>(_interval*_timeScale);
        if (onTimedEvent &&  elapsedTime  >= finalInterval)
        {
            _deltaTimer.Reset();
            _deltaTime   = elapsedTime;
            _frameCount += 1;
            onTimedEvent(shared_from_this());
            if (_frameCount >= _repeatCount && _repeatCount != repeat_forever)
            {
                Stop();
                break;
            }
        }
    } while (_bRunning == true);
}

NS_FK_END




