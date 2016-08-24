#ifndef FoundationKit_EventListenerAcceleration_H
#define FoundationKit_EventListenerAcceleration_H


#include "EventListener.h"
#include <memory>
NS_FK_BEGIN

class Acceleration;

/** @class EventListenerAcceleration
 * @brief Acceleration event listener.
 * @js NA
 */
class EventListenerAcceleration : public EventListener
{
public:

    typedef std::shared_ptr<EventListenerAcceleration>  Pointer;

    static const std::string LISTENER_ID;
    
    /** Create a acceleration EventListener.
     *
     * @param callback The acceleration callback method.
     * @return An autoreleased EventListenerAcceleration object.
     */
    static Pointer create(const std::function<void(Acceleration*, Event*)>& callback);
    
    /** Destructor.
     */
    virtual ~EventListenerAcceleration();
    
    virtual bool checkAvailable() override;
    
protected:
    EventListenerAcceleration();
    
    bool init(const std::function<void(Acceleration*, Event* event)>& callback);
    
private:
    std::function<void(Acceleration*, Event*)> onAccelerationEvent;
    
    friend class LuaEventListenerAcceleration;
};

NS_FK_END

#endif // FoundationKit_EventListenerAcceleration_H

