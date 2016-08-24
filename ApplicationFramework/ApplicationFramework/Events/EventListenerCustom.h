#pragma once

#include "EventListener.h"

NS_FK_BEGIN

class EventCustom;

/**
 *  Usage:
 *        auto dispatcher = Director::getInstance()->getEventDispatcher();
 *     Adds a listener:
 *
 *        auto callback = [](EventCustom* event){ do_some_thing(); };
 *        auto listener = EventListenerCustom::create(callback);
 *        dispatcher->addEventListenerWithSceneGraphPriority(listener, one_node);
 *
 *     Dispatchs a custom event:
 *
 *        EventCustom event("your_event_type");
 *        dispatcher->dispatchEvent(&event);
 *
 *     Removes a listener
 *
 *        dispatcher->removeEventListener(listener);
 */
class  EventListenerCustom : public EventListener
{
public:
    typedef std::shared_ptr<EventListenerCustom>    Pointer;

    /** Creates an event listener with type and callback.
     *  @param eventType The type of the event.
     *  @param callback The callback function when the specified event was emitted.
     */
    static EventListenerCustom::Pointer create(const std::string& eventName, const std::function<void(EventCustom*)>& callback);
    
    /// Overrides
    virtual bool checkAvailable() override;
    
protected:
    /** Constructor */
    EventListenerCustom();
    
    /** Initializes event with type and callback function */
    bool init(const ListenerID& listenerId, const std::function<void(EventCustom*)>& callback);
    
protected:
    std::function<void(EventCustom*)> _onCustomEvent;
    
};

NS_FK_END
