#include "Timer.h"
#include "Timespan.h"
#include "ElapsedTimer.h"
NS_FK_BEGIN

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
}

Timer::Timer(int interval)
{
    _interval = interval;
}

Timer::~Timer()
{
    setEnabled(false);

    if (_loopThread.joinable())
    {
        _loopThread.join();
    }
}

bool Timer::getEnabled()
{
    return _enable;
}

void Timer::setEnabled(bool value)
{
    _enable = value;
}

int Timer::getInterval()
{
    return _interval;
}

void Timer::setInterval(int value)
{
    _interval = value;
}

float Timer::getTimeScale()
{
    return _timeScale;
}

void Timer::setTimeScale(float value)
{
    _timeScale = value;
}

int Timer::getMaximumDeltaTime()
{
    return _maximumDeltaTime;
}

void Timer::setMaximumDeltaTime(int value)
{
    _maximumDeltaTime = value;
}

int Timer::getDeltaTime()
{
    return _deltaTime;
}

int Timer::getFrameCount()
{
    return _frameCount;
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
        onTimedEvent(_elapsedTime);
        _elapsedTime = 0;
        _frameCount += 1;
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
            update(et.milliseconds());
            et.reset();
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    });
}


NS_FK_END


