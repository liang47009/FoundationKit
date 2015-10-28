
#include "EventListener.h"
NS_FK_BEGIN
EventListener::EventListener()
: _type(Type::UNKNOWN)
, _isRegistered(false)
, _target(nullptr)
, _fixedPriority(1)
, _paused(false)
, _isEnabled(false)
{}
    
EventListener::~EventListener() 
{

}

bool EventListener::init(Type t, const ListenerID& listenerID, const std::function<void(Event*)>& callback)
{
    _onEvent = callback;
    _type = t;
    _listenerID = listenerID;
    _isRegistered = false;
    _paused = true;
    _isEnabled = true;
    
    return true;
}

bool EventListener::checkAvailable()
{ 
	return (_onEvent != nullptr);
}

NS_FK_END