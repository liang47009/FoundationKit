

#include "EventListenerAcceleration.h"
#include "EventAcceleration.h"


NS_FK_BEGIN

const std::string EventListenerAcceleration::LISTENER_ID = "losemymind.libo.EventListenerAcceleration";

EventListenerAcceleration::EventListenerAcceleration()
{

}

EventListenerAcceleration::~EventListenerAcceleration()
{

}

EventListenerAcceleration::Pointer EventListenerAcceleration::create(const std::function<void(Acceleration*, Event*)>& callback)
{
    EventListenerAcceleration* ret = new (std::nothrow) EventListenerAcceleration();
    if (ret && ret->init(callback))
    {
    }
    else
    {
        SAFE_DELETE(ret);
    }
    
    return EventListenerAcceleration::Pointer(ret);
}

bool EventListenerAcceleration::init(const std::function<void(Acceleration*, Event* event)>& callback)
{
    auto listener = [this](Event* event){
        auto accEvent = static_cast<EventAcceleration*>(event);
        this->onAccelerationEvent(&accEvent->_acc, event);
    };
    
    if (EventListener::init(Type::ACCELERATION, LISTENER_ID, listener))
    {
        onAccelerationEvent = callback;
        return true;
    }
    
    return false;
}


bool EventListenerAcceleration::checkAvailable()
{
    return true;
}

NS_FK_END


