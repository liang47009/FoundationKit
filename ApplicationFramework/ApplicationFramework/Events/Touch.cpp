

#include "Touch.h"


NS_FK_BEGIN

// returns the current touch location in screen coordinates
Vector2 Touch::getLocationInView() const
{ 
    return _point; 
}

// returns the previous touch location in screen coordinates
Vector2 Touch::getPreviousLocationInView() const
{ 
    return _prevPoint; 
}

// returns the start touch location in screen coordinates
Vector2 Touch::getStartLocationInView() const
{ 
    return _startPoint; 
}

// returns the current touch location coordinates
Vector2 Touch::getLocation() const
{ 
    return _point; 
}

// returns the previous touch location coordinates
Vector2 Touch::getPreviousLocation() const
{ 
    return _prevPoint;  
}

// returns the start touch location coordinates
Vector2 Touch::getStartLocation() const
{ 
    return _startPoint;  
}

// returns the delta position between the current location and the previous location coordinates
Vector2 Touch::getDelta() const
{     
    return getLocation() - getPreviousLocation();
}

NS_FK_END
