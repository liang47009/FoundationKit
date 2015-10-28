
#include "Event.h"
NS_FK_BEGIN
Event::Event(Type type)
: _type(type)
, _isStopped(false)
, _currentTarget(nullptr)
{
}

Event::~Event()
{
}

NS_FK_END