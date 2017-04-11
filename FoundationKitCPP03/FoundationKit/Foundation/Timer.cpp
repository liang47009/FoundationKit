#include "FoundationKit/Foundation/Timer.hpp"
#include "FoundationKit/Foundation/Timespan.hpp"
#include "FoundationKit/Foundation/ElapsedTimer.hpp"
#include "FoundationKit/Foundation/Logger.hpp"
NS_FK_BEGIN

int Timer::_nextValidId = 1000;

Timer::Timer()
{
    TimerOption to(false, 1000, 1.0f, 100, repeat_forever);
    init(to);
}

Timer::Timer(int interval)
{
    TimerOption to(false, interval, 1.0f, 100, repeat_forever);
    init(to);
}

Timer::Timer(const TimerOption& option)
{
    init(option);
}


Timer::~Timer()
{
    setEnabled(false);
    if (_loopThread && _loopThread->joinable())
    {
        _loopThread->join();
    }
    SAFE_DELETE(_loopThread);
}


void Timer::init(const TimerOption& option)
{
    _loopThread = nullptr;
    onTimedEvent = option.onTimedEvent;
    setEnabled(option.enable);
    setInterval(option.interval);
    setTimeScale(option.scale);
    setMaximumDeltaTime(option.maximumDeltaTime);// initialized to 100 milliseconds.
    _repeatCount = option.repeatCount;

    _deltaTime = 0;
    _frameCount = 0;
    _elapsedTime = 0;
    _myid = _nextValidId++;
    _useThread = false;
}

Timer::pointer Timer::create()
{
    Timer::pointer pTimer = std::shared_ptr<Timer>(new Timer());
    return pTimer;
}

Timer::pointer Timer::create(int interval)
{
    Timer::pointer pTimer = std::shared_ptr<Timer>(new Timer(interval));
    return pTimer;
}

Timer::pointer Timer::create(const TimerOption& option)
{
    Timer::pointer pTimer = std::shared_ptr<Timer>(new Timer(option));
    return pTimer;
}

void Timer::setOption(const TimerOption& option)
{
    this->setEnabled(option.enable)
        .setInterval(option.interval)
        .setTimeScale(option.scale)
        .setMaximumDeltaTime(option.maximumDeltaTime)
        .setRepeatCount(option.repeatCount);
    this->onTimedEvent = option.onTimedEvent;
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

void Timer::update(float deltaTime)
{
    ASSERTED(!_useThread, "The timer started by call startInThread, it running in a thread, if you want it run on update loop, you can stop it and restart.");
    internalUpdate(static_cast<int>(deltaTime * 1000));
}

void Timer::start()
{
    setEnabled(true);
}

void Timer::stop()
{
    setEnabled(false);
    reset();
}

void Timer::startInThread()
{
    setEnabled(true);
    _useThread = true;
    _loopThread = new std::thread(&Timer::threadFun, this);
}

void Timer::threadLoop()
{
    ElapsedTimer et;
    while (_enable)
    {
        internalUpdate(static_cast<int>(et.milliseconds()));
        et.reset();
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

void Timer::threadFun(void* pThis)
{
    Timer* pTimer = (Timer*)pThis;
    pTimer->threadLoop();
}

void Timer::reset()
{
    _deltaTime = 0;
    _frameCount = 0;
    _elapsedTime = 0;
}

void Timer::internalUpdate(int deltaTime)
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
        timerArgs.signalTime = DateTime::now();
        timerArgs.timerId = _myid;
        onTimedEvent(timerArgs);
        _elapsedTime = 0;
        _frameCount += 1;
        if (_frameCount >= _repeatCount && _repeatCount != repeat_forever)
        {
            stop();
        }
    }
}


NS_FK_END



