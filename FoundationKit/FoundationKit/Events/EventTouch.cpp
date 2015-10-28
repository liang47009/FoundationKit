

#include "EventTouch.h"
#include "Touch.h"

NS_FK_BEGIN

EventTouch::EventTouch()
: Event(Type::TOUCH)
{
    _touches.reserve(MAX_TOUCHES);
}

NS_FK_END
