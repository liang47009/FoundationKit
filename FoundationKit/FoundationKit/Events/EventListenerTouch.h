
#ifndef FoundationKit_EventListenerTouch_H
#define FoundationKit_EventListenerTouch_H

#include "EventListener.h"
#include <vector>
#include "Touch.h"

NS_FK_BEGIN

class Touch;

/** @class EventListenerTouchOneByOne
 * @brief Single touch event listener.
 */
class EventListenerTouchOneByOne : public EventListener
{
public:
    typedef std::shared_ptr<EventListenerTouchOneByOne>    Pointer;
    static const std::string LISTENER_ID;
    
    /** Create a one by one touch event listener.
     */
    static EventListenerTouchOneByOne::Pointer create();
    
    /**
     * Destructor.
     */
    virtual ~EventListenerTouchOneByOne();
    
    /** Whether or not to swall touches.
     *
     * @param needSwallow True if needs to swall touches.
     */
    void setSwallowTouches(bool needSwallow);
    /** Is swall touches or not.
     *
     * @return True if needs to swall touches.
     */
    bool isSwallowTouches();

    virtual bool checkAvailable() override;
    //

public:

    typedef std::function<bool(Touch::Pointer, Event*)> ccTouchBeganCallback;
    typedef std::function<void(Touch::Pointer, Event*)> ccTouchCallback;

    ccTouchBeganCallback onTouchBegan;
    ccTouchCallback onTouchMoved;
    ccTouchCallback onTouchEnded;
    ccTouchCallback onTouchCancelled;
    
public:
    EventListenerTouchOneByOne();
    bool init();
    
private:
    std::vector<Touch::Pointer> _claimedTouches;
    bool _needSwallow;
    
    friend class EventDispatcher;
};

/** @class EventListenerTouchAllAtOnce
 * @brief Multiple touches event listener.
 */
class EventListenerTouchAllAtOnce : public EventListener
{
public:
    typedef std::shared_ptr<EventListenerTouchAllAtOnce>    Pointer;
    static const std::string LISTENER_ID;
    
    /** Create a all at once event listener.
     *
     * @return An autoreleased EventListenerTouchAllAtOnce object.
     */
    static EventListenerTouchAllAtOnce::Pointer create();
    /** Destructor.
     */
    virtual ~EventListenerTouchAllAtOnce();
    
    virtual bool checkAvailable() override;
    //
public:

    typedef std::function<void(const std::vector<Touch::Pointer>&, Event*)> ccTouchesCallback;

    ccTouchesCallback onTouchesBegan;
    ccTouchesCallback onTouchesMoved;
    ccTouchesCallback onTouchesEnded;
    ccTouchesCallback onTouchesCancelled;
    
public:
    EventListenerTouchAllAtOnce();
    bool init();
private:
    
    friend class EventDispatcher;
};

NS_FK_END

#endif // FoundationKit_EventListenerTouch_H

