#pragma once

#include <string>
#include "Event.h"

NS_FK_BEGIN
class  EventCustom : public Event
{
public:
    /** Constructor */
    EventCustom(const std::string& eventName);
    
    /** Sets user data */
    inline void setUserData(void* data) { _userData = data; };
    
    /** Gets user data */
    inline void* getUserData() const { return _userData; };
    
    /** Gets event name */
    inline const std::string& getEventName() const { return _eventName; };
protected:
    void* _userData;       ///< User data
    std::string _eventName;
};

NS_FK_END