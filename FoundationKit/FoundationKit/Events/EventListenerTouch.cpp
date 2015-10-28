
#include "EventListenerTouch.h"
#include "EventDispatcher.h"
#include "EventTouch.h"
#include "Touch.h"
#include "FoundationKit/Foundation/Logger.h"
#include <algorithm>

NS_FK_BEGIN

const std::string EventListenerTouchOneByOne::LISTENER_ID = "losemymind.libo.EventListenerTouchOneByOne";

EventListenerTouchOneByOne::EventListenerTouchOneByOne()
: onTouchBegan(nullptr)
, onTouchMoved(nullptr)
, onTouchEnded(nullptr)
, onTouchCancelled(nullptr)
, _needSwallow(false)
{
}

EventListenerTouchOneByOne::~EventListenerTouchOneByOne()
{
    LOG_INFO("In the destructor of EventListenerTouchOneByOne, %p", this);
}

bool EventListenerTouchOneByOne::init()
{
    if (EventListener::init(Type::TOUCH_ONE_BY_ONE, LISTENER_ID, nullptr))
    {
        return true;
    }
    
    return false;
}

void EventListenerTouchOneByOne::setSwallowTouches(bool needSwallow)
{
    _needSwallow = needSwallow;
}

bool EventListenerTouchOneByOne::isSwallowTouches()
{
    return _needSwallow;
}

EventListenerTouchOneByOne::Pointer EventListenerTouchOneByOne::create()
{
    auto ret = new (std::nothrow) EventListenerTouchOneByOne();
    if (ret && ret->init())
    {
    }
    else
    {
        FK_SAFE_DELETE(ret);
    }
    return EventListenerTouchOneByOne::Pointer(ret);

}

bool EventListenerTouchOneByOne::checkAvailable()
{
    // EventDispatcher will use the return value of 'onTouchBegan' to determine whether to pass following 'move', 'end'
    // message to 'EventListenerTouchOneByOne' or not. So 'onTouchBegan' needs to be set.
    if (onTouchBegan == nullptr)
    {
        FKASSERT(false, "Invalid EventListenerTouchOneByOne!");
        return false;
    }
    
    return true;
}

const std::string EventListenerTouchAllAtOnce::LISTENER_ID = "losemymind.libo.EventListenerTouchAllAtOnce";

EventListenerTouchAllAtOnce::EventListenerTouchAllAtOnce()
: onTouchesBegan(nullptr)
, onTouchesMoved(nullptr)
, onTouchesEnded(nullptr)
, onTouchesCancelled(nullptr)
{
}

EventListenerTouchAllAtOnce::~EventListenerTouchAllAtOnce()
{
    LOG_DEBUG("In the destructor of EventListenerTouchAllAtOnce, %p", this);
}

bool EventListenerTouchAllAtOnce::init()
{
    if (EventListener::init(Type::TOUCH_ALL_AT_ONCE, LISTENER_ID, nullptr))
    {
        return true;
    }
    
    return false;
}

EventListenerTouchAllAtOnce::Pointer EventListenerTouchAllAtOnce::create()
{
    auto ret = new (std::nothrow) EventListenerTouchAllAtOnce();
    if (ret && ret->init())
    {
    }
    else
    {
        FK_SAFE_DELETE(ret);
    }
    return EventListenerTouchAllAtOnce::Pointer(ret);
}

bool EventListenerTouchAllAtOnce::checkAvailable()
{
    if (onTouchesBegan == nullptr && onTouchesMoved == nullptr
        && onTouchesEnded == nullptr && onTouchesCancelled == nullptr)
    {
        FKASSERT(false, "Invalid EventListenerTouchAllAtOnce!");
        return false;
    }
    
    return true;
}


NS_FK_END
