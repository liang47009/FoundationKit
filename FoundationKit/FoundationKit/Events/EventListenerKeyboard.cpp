
#include "EventListenerKeyboard.h"


NS_FK_BEGIN

const std::string EventListenerKeyboard::LISTENER_ID = "losemymind.libo.KeyboardListener";

bool EventListenerKeyboard::checkAvailable()
{
    if (onKeyPressed == nullptr && onKeyReleased == nullptr && onKeyChar == nullptr)
    {
        FKASSERT(false, "Invalid EventListenerKeyboard!");
        return false;
    }
    
    return true;
}

EventListenerKeyboard::Pointer EventListenerKeyboard::create()
{
    auto ret = new (std::nothrow) EventListenerKeyboard();
    if (ret && ret->init())
    {
        
    }
    else
    {
        FK_SAFE_DELETE(ret);
    }
    return EventListenerKeyboard::Pointer(ret);
}

EventListenerKeyboard::EventListenerKeyboard()
: onKeyPressed(nullptr)
, onKeyReleased(nullptr)
, onKeyChar(nullptr)
{
}

bool EventListenerKeyboard::init()
{
    auto listener = [this](Event* event){
        auto keyboardEvent = static_cast<EventKeyboard*>(event);
        if (keyboardEvent->_isPressed)
        {
            if (onKeyPressed != nullptr)
                onKeyPressed(keyboardEvent->_keyCode, event);
        }
        else
        {
            if (onKeyReleased != nullptr)
                onKeyReleased(keyboardEvent->_keyCode, event);
        }
    };

    if (EventListener::init(Type::KEYBOARD, LISTENER_ID, listener))
    {
        return true;
    }

    return false;
}

NS_FK_END
