#pragma once
#include "EventListener.h"
#include "EventKeyboard.h"

NS_FK_BEGIN

class Event;

class EventListenerKeyboard : public EventListener
{
public:
    typedef std::shared_ptr<EventListenerKeyboard>    Pointer;

    static const std::string LISTENER_ID;
    
    static EventListenerKeyboard::Pointer create();
    
    virtual bool checkAvailable() override;
    
    std::function<void(EventKeyboard::KeyCode, Event*)> onKeyPressed;
    std::function<void(EventKeyboard::KeyCode, Event*)> onKeyReleased;
    std::function<void(EventKeyboard::KeyCode, Event*)> onKeyChar;
protected:
    EventListenerKeyboard();
    bool init();
};

NS_FK_END
