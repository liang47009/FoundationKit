#include "Timer.h"
#include "Timespan.h"
#include "ElapsedTimer.h"
NS_FK_BEGIN

int Timer::_nextValidId = 0;

Timer::Timer()
: onTimedEvent(nullptr)
{
    setEnabled(false);
    setInterval(1000);
    setTimeScale(1.0f);
    setMaximumDeltaTime(100);// initialized to 100 milliseconds.
    _deltaTime = 0;
    _frameCount = 0;
    _elapsedTime = 0;
    _repeatCount = repeat_forever;
    _myid = _nextValidId++;
}

Timer::Timer(int interval)
{
    setEnabled(false);
    setInterval(interval);
    setTimeScale(1.0f);
    setMaximumDeltaTime(100);// initialized to 100 milliseconds.
    _deltaTime = 0;
    _frameCount = 0;
    _elapsedTime = 0;
    _repeatCount = repeat_forever;
    _myid = _nextValidId++;
}

Timer::~Timer()
{
    setEnabled(false);
    if (_loopThread.joinable())
    {
        _loopThread.join();
    }
}

Timer::Timer(Timer&& other)
{
    move(std::forward<Timer&&>(other));
}

Timer& Timer::operator=(Timer&& other)
{
    move(std::forward<Timer&&>(other));
    return (*this);
}

Timer& Timer::setEnabled(bool value)
{
    _enable = value;
    return (*this);
}
Timer& Timer::setInterval(int value)
{
    if (value < 0)
        throw std::invalid_argument("setInterval value must be > 0");
    _interval = value;
    return (*this);
}

Timer& Timer::setTimeScale(float value)
{
    _timeScale = value;
    return (*this);
}

Timer& Timer::setMaximumDeltaTime(int value)
{
    _maximumDeltaTime = value;
    return (*this);
}

Timer& Timer::setRepeatCount(int value)
{
    _repeatCount = value;
    return (*this);
}

bool Timer::getEnabled()const
{
    return _enable;
}

int Timer::getInterval()const
{
    return _interval;
}


float Timer::getTimeScale()const
{
    return _timeScale;
}


int Timer::getMaximumDeltaTime()const
{
    return _maximumDeltaTime;
}

int Timer::getDeltaTime()const
{
    return _deltaTime;
}

int Timer::getFrameCount()const
{
    return _frameCount;
}

int Timer::get_id()const
{
    return _myid;
}

void Timer::update(int deltaTime)
{
    if (!_enable) return;

    if (deltaTime > _maximumDeltaTime)
    {
        deltaTime = _maximumDeltaTime;
    }

    _deltaTime    = deltaTime;
    _elapsedTime += deltaTime;

    if (onTimedEvent&& _elapsedTime > _interval)
    {
        onTimedEvent(static_cast<int>(_elapsedTime*_timeScale));
        _elapsedTime = 0;
        _frameCount += 1;
        if (_frameCount >= _repeatCount && _repeatCount != repeat_forever)
        {
            _frameCount = 0;
            stop();
        }
    }
}

void Timer::start()
{
    setEnabled(true);
}

void Timer::stop()
{
    setEnabled(false);
}

void Timer::startInThread()
{
    setEnabled(true);
    _loopThread = std::thread([&]()
    {
        ElapsedTimer et;
        while (_enable)
        {
            update(static_cast<int>(et.milliseconds()));
            et.reset();
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    });
}

void Timer::move(Timer&& other)
{
    setEnabled(other.getEnabled());
    setInterval(other.getInterval());
    setTimeScale(other.getTimeScale());
    setMaximumDeltaTime(other.getMaximumDeltaTime());// initialized to 100 milliseconds.
    _deltaTime = other._deltaTime;
    _frameCount = other._frameCount;
    _elapsedTime = other._elapsedTime;
    _repeatCount = other._repeatCount;
    _myid = other._myid;
    _loopThread = std::move(other._loopThread);
    other._myid = involid_id;
}

NS_FK_END


