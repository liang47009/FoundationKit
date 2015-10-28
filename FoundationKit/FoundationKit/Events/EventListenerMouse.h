#ifndef FoundationKit_EventListenerMouse_H
#define FoundationKit_EventListenerMouse_H


#include "EventListener.h"
#include "EventMouse.h"
#include <memory>

NS_FK_BEGIN

class Event;

/** @class EventListenerMouse
 * @brief Mouse event listener.
 */
class EventListenerMouse : public EventListener
{
public:

    typedef std::shared_ptr<EventListenerMouse> Pointer;

    static const std::string LISTENER_ID;
    
    /** Create a mouse event listener.
     *
     * @return An autoreleased EventListenerMouse object.
     */
    static EventListenerMouse::Pointer create();

    virtual bool checkAvailable() override;

    std::function<void(Event* event)> onMouseDown;
    std::function<void(Event* event)> onMouseUp;
    std::function<void(Event* event)> onMouseMove;
    std::function<void(Event* event)> onMouseScroll;
protected:
    EventListenerMouse();
    bool init();
};

NS_FK_END


#endif // FoundationKit_EventListenerMouse_H

