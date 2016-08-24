

#include "EventMouse.h"


NS_FK_BEGIN

EventMouse::EventMouse(MouseEventType mouseEventCode)
: Event(Type::MOUSE)
, _mouseEventType(mouseEventCode)
, _mouseButton(-1)
, _x(0.0f)
, _y(0.0f)
, _scrollX(0.0f)
, _scrollY(0.0f)
, _startPointCaptured(false)
{
};

// returns the current touch location in screen coordinates
Vector2 EventMouse::getLocationInView() const
{ 
    return _point; 
}

// returns the previous touch location in screen coordinates
Vector2 EventMouse::getPreviousLocationInView() const
{ 
    return _prevPoint; 
}

// returns the start touch location in screen coordinates
Vector2 EventMouse::getStartLocationInView() const
{ 
    return _startPoint; 
}

// returns the current touch location  coordinates
Vector2 EventMouse::getLocation() const
{ 
    return _point; 
}

// returns the previous touch location coordinates
Vector2 EventMouse::getPreviousLocation() const
{ 
    return _prevPoint;  
}

// returns the start touch location  coordinates
Vector2 EventMouse::getStartLocation() const
{ 
    return _startPoint;  
}

// returns the delta position between the current location and the previous location coordinates
Vector2 EventMouse::getDelta() const
{     
    return getLocation() - getPreviousLocation();
}
NS_FK_END
