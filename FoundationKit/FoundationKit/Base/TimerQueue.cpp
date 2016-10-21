
#include "TimerQueue.hpp"

NS_FK_BEGIN

TimerQueue::TimerQueue()
{

}


TimerQueue::~TimerQueue()
{
}


int TimerQueue::enqueue(const TimerOption& timerOption)
{
    UNUSED_ARG(timerOption);
    return Timer::involid_id;
}

int TimerQueue::insert()
{
    return Timer::involid_id;
}

void TimerQueue::erase()
{

}


NS_FK_END








