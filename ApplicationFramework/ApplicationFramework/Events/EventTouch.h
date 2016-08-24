
#ifndef FoundationKit_EventTouch_H
#define FoundationKit_EventTouch_H

#include "Event.h"
#include "Touch.h"
#include <vector>


NS_FK_BEGIN

#define TOUCH_PERF_DEBUG 1

/** @class EventTouch
 * @brief Touch event.
 */
class EventTouch : public Event
{
public:
    static const int MAX_TOUCHES = 15;
    
    /** EventCode Touch event code.*/
    enum class EventCode
    {
        BEGAN,
        MOVED,
        ENDED,
        CANCELLED
    };

    /** 
     * Constructor.
     */
    EventTouch();

    /** Get event code.
     *
     * @return The code of the event.
     */
    inline EventCode getEventCode() const { return _eventCode; };
    
    /** Get the touches.
     *
     * @return The touches of the event.
     */
    inline const std::vector<Touch::Pointer>& getTouches() const { return _touches; };

#if TOUCH_PERF_DEBUG
    /** Set the event code.
     * 
     * @param eventCode A given EventCode.
     */
    void setEventCode(EventCode eventCode) { _eventCode = eventCode; };
    /** Set the touches
     *
     * @param touches A given touches vector.
     */
    void setTouches(const std::vector<Touch::Pointer>& touches) { _touches = touches; };
#endif
    
private:
    EventCode _eventCode;
    std::vector<Touch::Pointer> _touches;

    friend class GLView;
};


NS_FK_END

#endif // FoundationKit_EventTouch_H

