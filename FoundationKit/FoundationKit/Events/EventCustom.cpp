
#include "EventCustom.h"
#include "Event.h"
NS_FK_BEGIN
EventCustom::EventCustom(const std::string& eventName)
: Event(Type::CUSTOM)
, _userData(nullptr)
, _eventName(eventName)
{
}

NS_FK_END

