/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.
 
  losemymind.libo@gmail.com

****************************************************************************/
#ifndef FOUNDATIONKIT_TIMERQUEUE_HPP
#define FOUNDATIONKIT_TIMERQUEUE_HPP
#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)
#include <vector>
#include "FoundationKit/Base/types.hpp"
#include "FoundationKit/Foundation/Timer.hpp"

NS_FK_BEGIN

enum class ETimerStatus : uint8
{
    PAUSED,
    ACTIVE,
};

class TimerQueue
{
public:
    TimerQueue();
    ~TimerQueue();
    void   tick(float deltaTime);
    int32  enqueue(const TimerOption& timerOption);
    int32  insert(const Timer::pointer timer);
    void   erase(const Timer::pointer timer);
    void   erase(int32 timerid);
    bool   exist(int32 timerid);
    void   enable(int32 timerid, bool value);
    void   start(int32 timerid);
    void   stop(int timerid);
    Timer::pointer getTimer(int32 timerid);

protected:
    void internalErase(int32 timerid);

private:
    std::vector<Timer::pointer>  _timerlist;
    std::vector<Timer::pointer>  _timersCache;
    std::vector<int32>           _willErase;
};

NS_FK_END

#endif // FOUNDATIONKIT_TIMERQUEUE_HPP
