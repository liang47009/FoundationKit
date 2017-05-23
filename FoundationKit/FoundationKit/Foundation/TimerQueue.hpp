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
    void   Tick(float deltaTime);
    int32  Enqueue(const TimerOption& timerOption);
    int32  Insert(const Timer::pointer timer);
    void   Erase(const Timer::pointer timer);
    void   Erase(int32 timerid);
    bool   Exist(int32 timerid);
    void   Enable(int32 timerid, bool value);
    void   Start(int32 timerid);
    void   Stop(int timerid);
    Timer::pointer GetTimer(int32 timerid);

protected:
    void InternalErase(int32 timerid);

private:
    std::vector<Timer::pointer>  _timerlist;
    std::vector<Timer::pointer>  _timersCache;
    std::vector<int32>           _willErase;
};

NS_FK_END

#endif // FOUNDATIONKIT_TIMERQUEUE_HPP
