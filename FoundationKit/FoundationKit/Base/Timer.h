/****************************************************************************
Copyright (c) 2016 libo All rights reserved.

losemymind.libo@gmail.com

****************************************************************************/
#ifndef LOSEMYMIND_TIMER_H
#define LOSEMYMIND_TIMER_H

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <functional>
#include <thread>
#include <atomic>
#include "FoundationKit/FoundationMacros.h"
#include "FoundationKit/Base/Types.h"

NS_FK_BEGIN

// expressed in milliseconds.
class Timer final
{
public:
    typedef std::function<void(int deltaTime)>  TimedEvent;
    TimedEvent onTimedEvent;

public:
    Timer();
    explicit Timer(int interval);
    ~Timer();

    // Gets a value indicating whether the Timer should raise the timed event.
    bool getEnabled();
    // Sets a value indicating whether the Timer should raise the timed event.
    void setEnabled(bool value);

    // Gets the interval, expressed in milliseconds, at which to raise the timed event.
    int  getInterval();
    // Sets the interval, expressed in milliseconds, at which to raise the timed event.
    void setInterval(int value);

    // The scale at which the time is passing. This can be used for slow motion effects.
    float getTimeScale();
    // The scale at which the time is passing. This can be used for slow motion effects.
    void  setTimeScale(float value);

    // The maximum time a frame can take, expressed in milliseconds.
    int   getMaximumDeltaTime();
    // The maximum time a frame can take, expressed in milliseconds.
    void  setMaximumDeltaTime(int value);

    // The time in milliseconds it took to complete the last frame (Read Only).
    int   getDeltaTime();
    // The time in milliseconds it took to complete the last frame (Read Only).
    //void  setDeltaTime(float value);

    // The total number of frames that have passed (Read Only).
    int   getFrameCount();
    // The total number of frames that have passed (Read Only).
    //void  setFrameCount();

    //expressed in milliseconds
    void update(int deltaTime);
    void start();
    void stop();
    void startInThread();

private:
    std::atomic_bool _enable;
    std::atomic_int  _interval;
    float            _timeScale;
    std::atomic_int  _maximumDeltaTime; // default to 100 seconds.
    std::atomic_int  _deltaTime;
    std::atomic_int  _frameCount;
    std::atomic_int  _elapsedTime;
    std::thread      _loopThread;
};

NS_FK_END

#endif // LOSEMYMIND_TIMER_H

