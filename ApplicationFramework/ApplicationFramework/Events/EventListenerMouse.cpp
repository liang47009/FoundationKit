

#include "EventListenerMouse.h"

NS_FK_BEGIN

const std::string EventListenerMouse::LISTENER_ID = "losemymind.libo.EventListenerMouse";

bool EventListenerMouse::checkAvailable()
{
    return true;
}

EventListenerMouse::Pointer EventListenerMouse::create()
{
    auto ret = new (std::nothrow) EventListenerMouse();
    if (ret && ret->init())
    {

    }
    else
    {
        SAFE_DELETE(ret);
    }
    return EventListenerMouse::Pointer(ret);
}

EventListenerMouse::EventListenerMouse()
: onMouseDown(nullptr)
, onMouseUp(nullptr)
, onMouseMove(nullptr)
, onMouseScroll(nullptr)
{
}

bool EventListenerMouse::init()
{
    auto listener = [this](Event* event){
        auto mouseEvent = static_cast<EventMouse*>(event);
        switch (mouseEvent->_mouseEventType)
        {
            case EventMouse::MouseEventType::MOUSE_DOWN:
                if(onMouseDown != nullptr)
                    onMouseDown(event);
                break;
            case EventMouse::MouseEventType::MOUSE_UP:
                if(onMouseUp != nullptr)
                    onMouseUp(event);
                break;
            case EventMouse::MouseEventType::MOUSE_MOVE:
                if(onMouseMove != nullptr)
                    onMouseMove(event);
                break;
            case EventMouse::MouseEventType::MOUSE_SCROLL:
                if(onMouseScroll != nullptr)
                    onMouseScroll(event);
                break;
            default:
                break;
        }
    };

    if (EventListener::init(Type::MOUSE, LISTENER_ID, listener))
    {
        return true;
    }

    return false;
}

NS_FK_END
