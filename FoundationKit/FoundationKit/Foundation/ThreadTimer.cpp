
#include "FoundationKit/Foundation/ThreadTimer.hpp"
#include "FoundationKit/Foundation/Timespan.hpp"
#include "FoundationKit/Foundation/ElapsedTimer.hpp"
NS_FK_BEGIN

int ThreadTimer::_nextValidId = 1000;

ThreadTimer::ThreadTimer()
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
ThreadTimer::ThreadTimer(ThreadTimer&& other)
{
    Move(std::forward<ThreadTimer&&>(other));
}

ThreadTimer::~ThreadTimer()
{
    Stop();
    _future.wait();
}

ThreadTimer& ThreadTimer::operator=(ThreadTimer&& other)
{
    move(std::forward<ThreadTimer&&>(other));
    return (*this);
}

ThreadTimer::Pointer ThreadTimer::Create()
{
    ThreadTimer::Pointer pTimer = ThreadTimer::Pointer(new ThreadTimer());
    return pTimer;
}

ThreadTimer& ThreadTimer::SetInterval(long value)
{
    if (value < 0)
        throw std::invalid_argument("setInterval value must be > 0");
    _interval = value;
    return (*this);
}

ThreadTimer& ThreadTimer::SetTimeScale(float value)
{
    _timeScale = value;
    return (*this);
}

ThreadTimer& ThreadTimer::SetRepeatCount(long value)
{
    _repeatCount = value;
    return (*this);
}

ThreadTimer& ThreadTimer::SetUserData(void* userData)
{
    _userData = userData;
    return (*this);
}

ThreadTimer& ThreadTimer::SetTimedEvent(const TimedEvent& callback)
{
    onTimedEvent = callback;
    return (*this);
}

long ThreadTimer::GetInterval()const
{
    return _interval.load();
}


float ThreadTimer::GetTimeScale()const
{
    return _timeScale.load();
}

long ThreadTimer::GetDeltaTime()const
{
    return _deltaTime.load();
}

long ThreadTimer::GetFrameCount()const
{
    return _frameCount.load();
}

long ThreadTimer::GetId()const
{
    return _myid;
}

void* ThreadTimer::GetUserData() const
{
    return _userData;
}

bool ThreadTimer::Start()
{
    if (onTimedEvent)
    {
        _bRunning = true;
        _deltaTimer.Reset();
        _future = std::async(std::launch::async, std::bind(&ThreadTimer::Run, this));
        return true;
    }
    return false;
}

bool ThreadTimer::Start(const TimedEvent& callback)
{
    onTimedEvent = callback;
    return Start();
}

bool ThreadTimer::Start(const TimedEvent& callback, long milliseconds, long repeatCount /*= repeat_forever*/, float timeScale /*= 1.0f*/)
{
    _interval    = milliseconds;
    _repeatCount = repeatCount;
    _timeScale   = timeScale;
    onTimedEvent = callback;
    return Start();
}

void ThreadTimer::Stop()
{
    _bRunning = false;
    Reset();
}

void ThreadTimer::Move(ThreadTimer&& other)
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

void ThreadTimer::Reset()
{
    _deltaTime   = 0;
    _frameCount  = 0;
}

void ThreadTimer::Run()
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




