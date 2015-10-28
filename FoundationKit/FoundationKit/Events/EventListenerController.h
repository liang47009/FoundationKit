#pragma once

#include "EventListener.h"


NS_FK_BEGIN

class Event;
class Controller;

/** @class EventListenerController
 * @param Controller event listener.
 * @js NA
 */
class EventListenerController : public EventListener
{
public:
    static const std::string LISTENER_ID;
    
    typedef std::shared_ptr<EventListenerController>    Pointer;

    /** Create a controller event listener.
     *
     * @return An autoreleased EventListenerController object.
     */
    static EventListenerController::Pointer create();
    
    /// Overrides
    virtual bool checkAvailable() override;

  
	std::function<void(Controller*, Event*)> onConnected;
	std::function<void(Controller*, Event*)> onDisconnected;
    
    std::function<void(Controller*, int, Event*)> onKeyDown;
    std::function<void(Controller*, int, Event*)> onKeyUp;
	std::function<void(Controller*, int, Event*)> onKeyRepeat;
    
	std::function<void(Controller*, int, Event*)> onAxisEvent;
    
protected:
    bool init();
};

NS_FK_END
